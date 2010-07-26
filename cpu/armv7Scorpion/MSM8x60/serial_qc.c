/*
 * Copyright (c) 2010, Code Aurora Forum. All rights reserved.
 *
 * (C) Copyright 2000
 * Rob Taylor, Flying Pig Systems. robt@flyingpig.com.
 *
 * (C) Copyright 2004
 * ARM Ltd.
 * Philippe Robin, <philippe.robin@arm.com>
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

/*
 * Simple U-Boot driver for the QUALCOMM UART.
 */

#include <common.h>

#ifdef CFG_QC_SERIAL

#define UART_MR1                ( UART_BASE + 0x00 )
#define UART_MR2                ( UART_BASE + 0x04 )
#define UART_CSR                ( UART_BASE + 0x08 )
#define UART_TF                 ( UART_BASE + 0x0C )
#define UART_CR                 ( UART_BASE + 0x10 )
#define UART_IMR                ( UART_BASE + 0x14 )
#define UART_IPR                ( UART_BASE + 0x18 )
#define UART_TFWR               ( UART_BASE + 0x1C )
#define UART_RFWR               ( UART_BASE + 0x20 )
#define UART_HCR                ( UART_BASE + 0x24 )
#define UART_MREG               ( UART_BASE + 0x28 )
#define UART_NREG               ( UART_BASE + 0x2C )
#define UART_DREG               ( UART_BASE + 0x30 )
#define UART_MNDREG             ( UART_BASE + 0x34 )
#define UART_IRDA               ( UART_BASE + 0x38 )
#define UART_MISR_MODE          ( UART_BASE + 0x40 )
#define UART_MISR_RESET         ( UART_BASE + 0x44 )
#define UART_MISR_EXPORT        ( UART_BASE + 0x48 )
#define UART_MISR_VAL           ( UART_BASE + 0x4C )
#define UART_TEST_CTRL          ( UART_BASE + 0x50 )
#define UART_SR                 ( UART_BASE + 0x08 )
#define UART_RF                 ( UART_BASE + 0x0C )
#define UART_MISR               ( UART_BASE + 0x10 )
#define UART_ISR                ( UART_BASE + 0x14 )


#define NUM_BAUD_ENTRY  5

struct baud_struct {
    unsigned int desired_baudrate;
    unsigned short csr_clk_sel_val;
};

struct  baud_struct  baud_set[NUM_BAUD_ENTRY] = {
                   {9600,   0x0099},
                   {19200,  0x00BB},
                   {38400,  0x00DD},
                   {57600,  0x00EE},
                   {115200, 0x00FF}
};


static void qc_putc (char c);
static int qc_getc (void);
static int qc_check_tx_ready (void);
static int qc_check_rx_ready (void);

int serial_init (void)
{
    return 0;
    uint32_t csr_clk_sel_val = 0;
    uint32_t mreg_val;
    uint32_t nreg_val;
    uint32_t dreg_val;
    uint32_t mndreg_val;
    int i;
    uint32_t reg;

    /*
     * Get the clock source for the UART from UART_NS_REG.
     */
    reg = IO_READ32(UART_NS_REG);
    switch (UART_BASE)
    {
    case UART1_BASE:
        reg &= UART_NS_REG__UART1_SRC_SEL___M;
        reg = reg >> UART_NS_REG__UART1_SRC_SEL___S;
        break;
    case UART2_BASE:
        reg &= UART_NS_REG__UART2_SRC_SEL___M;
        reg = reg >> UART_NS_REG__UART2_SRC_SEL___S;
        break;
    case UART3_BASE:
        reg &= UART_NS_REG__UART3_SRC_SEL___M;
        reg = reg >> UART_NS_REG__UART3_SRC_SEL___S;
        break;
    default:
        return (-1);
    }

    /*
     * Determine if TCXO (19.2MHz) or TCXO/4 (4.8MHz) is the UART clock source.
     * Use recommended m,n,d,mnd register settings.
     */
    switch (reg)
    {
    case 0:  // TCXO clock source
        mreg_val = 0x0006;
        nreg_val = 0x00F1;
        dreg_val = 0x000F;
        mndreg_val = 0x001A;
        break;
    case 1:  // TCXO/4 clock source
        mreg_val = 0x0018;
        nreg_val = 0x00F6;
        dreg_val = 0x000F;
        mndreg_val = 0x000A;
        break;
    default:
        return(-1);
    }
    /*
     * Determine the proper setting for the CSR register based on the desired baud rate.
     */
    for (i=0;i<NUM_BAUD_ENTRY;i++)
    {
        if (baud_set[i].desired_baudrate == CONFIG_BAUDRATE)
        {
            csr_clk_sel_val = baud_set[i].csr_clk_sel_val;
        }
    }
    if (csr_clk_sel_val == 0)
    {
        return(-1);
    }

    /* Init to the desired baud rate */
    IO_WRITE32(UART_MREG,   mreg_val);	// M/N Counter Clock Setup
    IO_WRITE32(UART_NREG,   nreg_val);	// M/N Counter Clock Setup
    IO_WRITE32(UART_DREG,   dreg_val);  // M/N Counter Clock Setup
    IO_WRITE32(UART_MNDREG, mndreg_val);// M/N Counter Clock Setup
    IO_WRITE32(UART_CR,     0x0015);    // Reset Rx
    IO_WRITE32(UART_CR,     0x0025);    // Reset Tx
    IO_WRITE32(UART_CR,     0x0035);    // Reset Error status
    IO_WRITE32(UART_CR,     0x0045);    // Rest Rx Break interrupt
    IO_WRITE32(UART_CR,     0x0075);    // Rest
    IO_WRITE32(UART_CR,     0x00D5);    // Rest
    IO_WRITE32(UART_IPR,    0x07BF);	// Stale_Timeout is 630 * Bit rate
    IO_WRITE32(UART_IMR,    0x0002);    // mask all but RXHUNT interrupt
    IO_WRITE32(UART_RFWR,   115);		// Rx watermark = (58 x 2) - 1 = 115
    IO_WRITE32(UART_TFWR,   400); 		// Tx watermark = 400
    IO_WRITE32(UART_CSR,    csr_clk_sel_val); 	// Set UART Baud Rate
    IO_WRITE32(UART_IRDA,   0x0000);  	// IRDA = 0x1 , RS232 = 0x0
    IO_WRITE32(UART_HCR,    0x001E);	// HUNT character is 254 = 0xFE
    IO_WRITE32(UART_MR1,    0x07B4);    // RFR/ on and CTS/ off, 500 character RFR/ threshhold
    IO_WRITE32(UART_MR2,    0x0034);    // 8N1 format
    IO_WRITE32(UART_CR,     0x0005);    // Enable Tx and Rx
    return 0;
}

void serial_putc (const char c)
{
	if (c == '\n') {
      qc_putc ('\r');
    }

	qc_putc (c);
}

void serial_puts (const char *s)
{
	while (*s) {
	  serial_putc (*s++);
	}
}

int serial_getc (void)
{
	return qc_getc ();
}

int serial_tstc (void)
{
	return qc_check_rx_ready ();
}

void serial_setbrg (void)
{
}

static void qc_putc (char c)
{
	/* Wait until there is room for a char in the TX FIFO */
	while (!qc_check_tx_ready ());

    /* send the character */
    IO_WRITE32(UART_TF, c);
}

static int qc_getc (void)
{
	int data;

	/* Wait until there is a char in the RX FIFO */
	while (!qc_check_rx_ready ());

    /* read the character */
    data = IO_READ32(UART_RF);

    return  data;
}

static int qc_check_tx_ready (void)
{
    int data;

    data = IO_READ32(UART_SR);

    return (data & UART_SR__TXRDY___M);
}

static int qc_check_rx_ready (void)
{
    int data;

    data = IO_READ32(UART_SR);

    return (data & UART_SR__RXRDY___M);
}

#endif
