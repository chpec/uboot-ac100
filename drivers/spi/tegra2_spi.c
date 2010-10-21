/*
 * Copyright (c) 2010 NVIDIA Corporation
 * With help from the mpc8xxx SPI driver
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>

#include <malloc.h>
#include <spi.h>
#include <asm/arch/tegra2_spi.h>
#include <asm/arch/gpio.h>

#define SPI_TIMEOUT	1000

/* On Seaboard: GPIO_PI3 = Port I = 8, bit = 3 */
#define UART_DISABLE_PORT	8
#define UART_DISABLE_BIT	3

int spi_cs_is_valid(unsigned int bus, unsigned int cs)
{
	/* Tegra2 SPI-Flash - only 1 device ('bus/cs') */
	if (bus > 0 && cs != 0)
		return 0;
	else
		return 1;
}


struct spi_slave *spi_setup_slave(unsigned int bus, unsigned int cs,
		unsigned int max_hz, unsigned int mode)
{
	struct spi_slave *slave;

	if (!spi_cs_is_valid(bus, cs))
		return NULL;

	slave = malloc(sizeof(struct spi_slave));
	if (!slave)
		return NULL;

	slave->bus = bus;
	slave->cs = cs;

	/*
	 * Currently, Tegra2 SFLASH uses mode 0 & a 6MHz clock.
	 * Use 'mode' and 'maz_hz' to change that here, if needed.
	 */

	return slave;
}

void spi_free_slave(struct spi_slave *slave)
{
	free(slave);
}

void spi_init(void)
{
	volatile spi_tegra_t *spi = (spi_tegra_t *)TEGRA2_SPI_BASE;
	u32 val;

	/* Enable UART via GPIO_PI3 (port 8, bit 3) so serial console works */
	tg2_gpio_direction_output(UART_DISABLE_PORT, UART_DISABLE_BIT, 0);

	/*
	 * SPI reset/clocks init - reset SPI, set clocks, release from reset
	 */
	NV_CLK_RST_READ(0x08, val);
	NV_CLK_RST_WRITE(0x08, (val | 0x800));
	debug("spi_init: ClkRst = %08x\n", val);

	NV_CLK_RST_READ(0x14, val);
	NV_CLK_RST_WRITE(0x14, (val | 0x800));
	debug("spi_init: ClkEnable = %08x\n", val);

	/* Change default SPI clock from 12MHz to 6MHz, same as BootROM */
	NV_CLK_RST_READ(0x114, val);
	NV_CLK_RST_WRITE(0x114, (val | 0x2));
	debug("spi_init: ClkSrc = %08x\n", val);

	NV_CLK_RST_READ(0x08, val);
	NV_CLK_RST_WRITE(0x08, (val & 0xFFFFF7FF));
	debug("spi_init: ClkRst final = %08x\n", val);

	/* Clear stale status here */

	spi->status = SPI_STAT_RDY | SPI_STAT_RXF_FLUSH | SPI_STAT_TXF_FLUSH | \
		SPI_STAT_RXF_UNR | SPI_STAT_TXF_OVF;
	debug("spi_init: STATUS = %08x\n", spi->status);

	/*
	 * Use sw-controlled CS, so we can clock in data after ReadID, etc.
	 */

	spi->command = (spi->command | SPI_CMD_CS_SOFT);
	debug("spi_init: COMMAND = %08x\n", spi->command);

	/*
	 * SPI pins on Tegra2 are muxed - change pinmux last due to UART issue
	 */

	val = NV_READ32(NV_ADDRESS_MAP_APB_MISC_BASE + 0x88);
	val |= 0xC0000000;
	NV_WRITE32((NV_ADDRESS_MAP_APB_MISC_BASE + 0x88), val);
	debug("spi_init: PinMuxRegC = %08x\n", val);

	val = NV_READ32(NV_ADDRESS_MAP_APB_MISC_BASE + 0x20);
	val &= 0xFFFFFFFE;
	NV_WRITE32((NV_ADDRESS_MAP_APB_MISC_BASE + 0x20), val);
	debug("spi_init: TriStateReg = %08x\n", val);

	/*
	 * DEBUG:
	 * Don't set PinMux bits 3:2 to SPI here or subsequent UART data
	 * won't go out! It'll be correctly set in the actual SPI driver
	 * before/after any transactions.
	 */
	val = NV_READ32(NV_ADDRESS_MAP_APB_MISC_BASE + 0x84);
	val |= 0x0000000C;
	debug("spi_init: PinMuxRegB = %08x\n", val);
/*	NV_WRITE32((NV_ADDRESS_MAP_APB_MISC_BASE + 0x84), val); */
}

int spi_claim_bus(struct spi_slave *slave)
{
	/* Move bulk of spi_init code here? */
	return 0;
}

void spi_release_bus(struct spi_slave *slave)
{
	/* Release UART_DISABLE and set pinmux to UART4 here? */
}

void spi_cs_activate(struct spi_slave *slave)
{
	volatile spi_tegra_t *spi = (spi_tegra_t *)TEGRA2_SPI_BASE;
	u32 val;

	/*
	 * Delay here to clean up comms - spurious chars seen around SPI xfers.
	 * Fine-tune later.
	 */
	udelay(1000);

	/*
	 * We need to dynamically change the pinmux, shared w/UART RXD/CTS!
	 */

	val = NV_READ32(NV_ADDRESS_MAP_APB_MISC_BASE + 0x84);
	val |= 0x0000000C;					/* 3 = SFLASH */
	NV_WRITE32((NV_ADDRESS_MAP_APB_MISC_BASE + 0x84), val);
	debug("spi_init: PinMuxRegB = %08x\n", val);

	/* On Seaboard, MOSI/MISO are shared w/UART.
	 * Use GPIO I3 (UART_DISABLE) to tristate UART during SPI activity.
	 * Enable UART later (cs_deactivate) so we can use it for U-Boot comms.
	 */

	debug("spi_cs_activate: DISABLING UART!\n");
	tg2_gpio_direction_output(UART_DISABLE_PORT, UART_DISABLE_BIT, 1);

	/*
	 * CS is negated on Tegra, so drive a 1 to get a 0
	 */
	spi->command |= SPI_CMD_CS_VAL;

	debug("spi_cs_activate: CS driven %s\n",
	 (spi->command & SPI_CMD_CS_VAL) ? "LOW" : "HIGH");
}

void spi_cs_deactivate(struct spi_slave *slave)
{
	volatile spi_tegra_t *spi = (spi_tegra_t *) TEGRA2_SPI_BASE;
	u32 val;

	/*
	 * Delay here to clean up comms - spurious chars seen around SPI xfers.
	 * Fine-tune later.
	 */
	udelay(1000);

	/*
	 * Looks like we may also need to dynamically change the pinmux,
	 *  shared w/UART RXD/CTS!
	 */

	val = NV_READ32(NV_ADDRESS_MAP_APB_MISC_BASE + 0x84);
	val &= 0xFFFFFFF3;					/* 0 = UART */
	NV_WRITE32((NV_ADDRESS_MAP_APB_MISC_BASE + 0x84), val);
	debug("spi_init: PinMuxRegB = %08x\n", val);

	/* On Seaboard, MOSI/MISO are shared w/UART.
	 * GPIO I3 (UART_DISABLE) was used to tristate UART in cs_activate.
	 * Enable UART here by setting that GPIO to 0 so we can do U-Boot comms.
	 */

	 tg2_gpio_direction_output(UART_DISABLE_PORT, UART_DISABLE_BIT, 0);
	 debug("spi_cs_deactivate: ENABLED UART!\n");
	/*
	 * CS is negated on Tegra, so drive a 0 to get a 1
	*/
	spi->command &= ~SPI_CMD_CS_VAL;

	debug("spi_cs_deactivate: CS driven %s\n",
	 (spi->command & SPI_CMD_CS_VAL) ? "LOW" : "HIGH");
}

int spi_xfer(struct spi_slave *slave, unsigned int bitlen, const void *dout,
		void *din, unsigned long flags)
{
	volatile spi_tegra_t *spi = (spi_tegra_t *)TEGRA2_SPI_BASE;
	unsigned int tmpdin, status;
	int numBytes = (bitlen + 7) / 8;
	int ret, tm, isRead = 0;

	debug("spi_xfer: slave %u:%u dout %08X din %08X bitlen %u\n",
	      slave->bus, slave->cs, *(uint *) dout, *(uint *) din, bitlen);

	ret = 0;

	status = spi->status;
	spi->status = spi->status;	/* Clear all SPI events via R/W */
	debug("spi_xfer entry: STATUS = %08x\n", status);

	spi->command |= (SPI_CMD_TXEN | SPI_CMD_RXEN);
	debug("spi_xfer: COMMAND = %08x\n", spi->command);

	if (flags & SPI_XFER_BEGIN)
		spi_cs_activate(slave);

	/* handle data in 32-bit chunks */
	while (numBytes > 0) {
		u32 tmpdout = 0;
		int bytes = (numBytes >  4) ?  4 : numBytes;
		int bits  = (bitlen   > 32) ? 32 : bitlen;
		int i;

		if (dout != NULL) {
			for (i = 0; i < bytes; ++i)
				tmpdout = (tmpdout << 8) | ((u8 *) dout)[i];
		}

		numBytes -= bytes;
		dout     += bytes;
		bitlen   -= bits;

		spi->command = (spi->command & ~SPI_CMD_BIT_LENGTH_MASK);
		spi->command |= bits - 1;

		spi->tx_fifo = tmpdout;	/* Write the data out */
		debug("spi_xfer: ... %08x written\n", tmpdout);

		spi->command |= SPI_CMD_GO;		/* Initiate transfer */

		/*
		 * Wait for SPI transmit FIFO to empty, or to time out.
		 * The RX FIFO status will be read and cleared last
		 */
		for (tm = 0, isRead = 0; tm < SPI_TIMEOUT; ++tm) {
			status = spi->status;
			debug("spi_xfer loop: STATUS = %08x\n", status);

			while (spi->status & SPI_STAT_BSY) {
				debug("spi_xfer: wait for !BSY,_status: %08x\n",
				 spi->status);
				status = spi->status;

				tm++;
				if (tm > SPI_TIMEOUT) {
					tm = 0;
					break;
				}
			}

			while (!(spi->status & SPI_STAT_RDY)) {
				debug("spi_xfer: wait for RDY, status: %08x\n",
				 spi->status);
				status = spi->status;

				tm++;
				if (tm > SPI_TIMEOUT) {
					tm = 0;
					break;
				}
			}

			if (!(spi->status & SPI_STAT_RXF_EMPTY)) {
				tmpdin = spi->rx_fifo;
				isRead = 1;
				status = spi->status;

				/* swap bytes read in */
				if (din != NULL) {
					for (i = bytes - 1; i >= 0; --i) {
						((u8 *)din)[i] = (tmpdin & 0xff);
						tmpdin >>= 8;
					}
					din += bytes;
				}
			}

			/* We can exit when we've had both RX and TX activity */
			if (isRead && (spi->status & SPI_STAT_TXF_EMPTY))
				break;
		}

		if (tm >= SPI_TIMEOUT) {
			ret = -1;
			debug("spi_xfer: Time out during SPI transfer!\n");
		}

		debug("spi_xfer: transfer ended. Value=%08x\n", tmpdin);

		status = spi->status;
		debug("spi_xfer: STATUS at end = %08x\n", status);
		spi->status = status;			/* ACK RDY, etc. bits */
	}

	if (flags & SPI_XFER_END)
		spi_cs_deactivate(slave);

	debug("spi_xfer: transfer ended. Value=%08x, status = %08x\n",
	 tmpdin, status);
	return ret;
}
