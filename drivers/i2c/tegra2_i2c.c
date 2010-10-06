/*
 *  (C) Copyright 2010 
 *  NVIDIA Corporation <www.nvidia.com>
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
#include <asm/io.h>
#include <asm/arch/nv_drf.h>
#include <asm/arch/nvboot_error.h>
#include <asm/arch/nvcommon.h>
#include <asm/arch/tegra2.h>
#include <asm/arch/nv_hardware_access.h>
#include <asm/arch/i2c.h>

#include "../board/tegra2/common/sdmmc/nvboot_clocks_int.h"
#include "../board/tegra2/common/sdmmc/nvboot_reset.h"
#include "../board/tegra2/common/sdmmc/nvboot_util.h"

#define NVB_READ32	NV_READ32_
#define NVB_WRITE32	NV_WRITE32_

/* define DO_TPM_TEST to enable TPM testing */
//#define DO_TPM_TEST

#ifdef DO_TPM_TEST
int do_i2c_tpmwd ( void );
int do_i2c_tpmrd ( void );
int do_i2c_tpmrs ( void );
#endif

static unsigned int i2c_bus_num __attribute__ ((section (".data"))) = 0;

NvBootClocksClockId i2c_clock_ids[] = {
	NvBootClocksClockId_I2cpId,
	NvBootClocksClockId_I2c1Id,
	NvBootClocksClockId_I2c2Id,
	NvBootClocksClockId_I2c3Id
};

NvBootResetDeviceId i2c_reset_ids[] = {
	NvBootResetDeviceId_I2cpId,
	NvBootResetDeviceId_I2c1Id,
	NvBootResetDeviceId_I2c2Id,
	NvBootResetDeviceId_I2c3Id
};

NvU32 i2c_cnfg_addrs[] = {
	NV_ADDRESS_MAP_APB_DVC_I2C_CNFG_REG,
	NV_ADDRESS_MAP_APB_I2C_CNFG_REG,
	NV_ADDRESS_MAP_APB_I2C2_CNFG_REG,
	NV_ADDRESS_MAP_APB_I2C3_CNFG_REG
};

NvU32 i2c_tx_fifo_addrs[] = {
	NV_ADDRESS_MAP_APB_DVC_I2C_TX_FIFO_REG,
	NV_ADDRESS_MAP_APB_I2C_TX_FIFO_REG,
	NV_ADDRESS_MAP_APB_I2C2_TX_FIFO_REG,
	NV_ADDRESS_MAP_APB_I2C3_TX_FIFO_REG
};

NvU32 i2c_rx_fifo_addrs[] = {
	NV_ADDRESS_MAP_APB_DVC_I2C_RX_FIFO_REG,
	NV_ADDRESS_MAP_APB_I2C_RX_FIFO_REG,
	NV_ADDRESS_MAP_APB_I2C2_RX_FIFO_REG,
	NV_ADDRESS_MAP_APB_I2C3_RX_FIFO_REG
};

NvU32 i2c_fifo_control_addrs[] = {
	NV_ADDRESS_MAP_APB_DVC_I2C_FIFO_CONTROL_REG,
	NV_ADDRESS_MAP_APB_I2C_FIFO_CONTROL_REG,
	NV_ADDRESS_MAP_APB_I2C2_FIFO_CONTROL_REG,
	NV_ADDRESS_MAP_APB_I2C3_FIFO_CONTROL_REG
};

NvU32 i2c_fifo_status_addrs[] = {
	NV_ADDRESS_MAP_APB_DVC_I2C_FIFO_STATUS_REG,
	NV_ADDRESS_MAP_APB_I2C_FIFO_STATUS_REG,
	NV_ADDRESS_MAP_APB_I2C2_FIFO_STATUS_REG,
	NV_ADDRESS_MAP_APB_I2C3_FIFO_STATUS_REG
};

NvU32 i2c_packet_status_addrs[] = {
	NV_ADDRESS_MAP_APB_DVC_I2C_PACKET_STATUS_REG,
	NV_ADDRESS_MAP_APB_I2C_PACKET_STATUS_REG,
	NV_ADDRESS_MAP_APB_I2C2_PACKET_STATUS_REG,
	NV_ADDRESS_MAP_APB_I2C3_PACKET_STATUS_REG
};

/* customize pin_mux_table based on the pin_mux configuration */
/* These constants are defined in tegra2_harmony.h or tegra2_seaboard.h */
int pin_mux_table[4] = {
	CONFIG_I2CP_PIN_MUX,	/* for I2CP */
	CONFIG_I2C1_PIN_MUX,	/* for I2C1 */
	CONFIG_I2C2_PIN_MUX,	/* for I2C2 */
	CONFIG_I2C3_PIN_MUX	/* for I2C3 */
};

/*
 * controller:
 *	0 - i2cp (power i2c),
 *	1 - i2c,
 *	2 - i2c2,
 *	3 - i2c3
 */
static void i2c_init_controller(NvU32 controller)
{
	NvU32 reg;

	NvBootClocksClockId clock_id;
	NvBootResetDeviceId reset_id;

	clock_id = i2c_clock_ids[controller];
	reset_id = i2c_reset_ids[controller];

	// Reset I2C controller.
	NvBootResetSetEnable(reset_id, NV_TRUE);
	NvBootClocksSetEnable(clock_id, NV_TRUE);
	NvBootResetSetEnable(reset_id, NV_FALSE);

	// Configure I2C controller.
	if (controller == 0) {	/* only for DVC I2C */
		NVB_READ32(NV_ADDRESS_MAP_APB_DVC_CTRL_REG3, reg);
		reg = NV_FLD_SET_DRF_DEF(DVC, CTRL_REG3, I2C_HW_SW_PROG,
					 SW, reg);
		NVB_WRITE32(NV_ADDRESS_MAP_APB_DVC_CTRL_REG3, reg);
	}
}

void i2c_pin_mux_select(int controller, int pin_mux)
{
	NvU32	reg;

	switch (controller) {
	case 0:	/* I2CP */
		/* there is only one selection, pin_mux is ignored */
		NVB_READ32(NV_ADDRESS_MAP_PP_PIN_MUX_CTL_C, reg);
		reg = NV_FLD_SET_DRF_DEF(APB_MISC_PP, PIN_MUX_CTL_C,
					 I2CP_SEL, I2C, reg);
		NVB_WRITE32(NV_ADDRESS_MAP_PP_PIN_MUX_CTL_C, reg);
		break;
	case 1: /* I2C1 */
		/* support pin_mux of 1 for now */
		/* which is RM pin group */
		NVB_READ32(NV_ADDRESS_MAP_PP_PIN_MUX_CTL_A, reg);
		reg = NV_FLD_SET_DRF_DEF(APB_MISC_PP, PIN_MUX_CTL_A,
					 RM_SEL, I2C, reg);
		NVB_WRITE32(NV_ADDRESS_MAP_PP_PIN_MUX_CTL_A, reg);
		break;
	case 2: /* I2C2 */
		switch (pin_mux) {
		case 1:	/* DDC pin group */
			NVB_READ32(NV_ADDRESS_MAP_PP_PIN_MUX_CTL_C, reg);
			reg = NV_FLD_SET_DRF_DEF(APB_MISC_PP, PIN_MUX_CTL_C,
						 DDC_SEL, I2C2, reg);
			NVB_WRITE32(NV_ADDRESS_MAP_PP_PIN_MUX_CTL_C, reg);
			/* PTA to HDMI */
			NVB_READ32(NV_ADDRESS_MAP_PP_PIN_MUX_CTL_G, reg);
			reg = NV_FLD_SET_DRF_DEF(APB_MISC_PP, PIN_MUX_CTL_G,
						 PTA_SEL, HDMI, reg);
			NVB_WRITE32(NV_ADDRESS_MAP_PP_PIN_MUX_CTL_G, reg);
			break;
		case 2:	/* PTA pin group */
			NVB_READ32(NV_ADDRESS_MAP_PP_PIN_MUX_CTL_G, reg);
			reg = NV_FLD_SET_DRF_DEF(APB_MISC_PP, PIN_MUX_CTL_G,
						 PTA_SEL, I2C2, reg);
			NVB_WRITE32(NV_ADDRESS_MAP_PP_PIN_MUX_CTL_G, reg);
			/* set DDC_SEL to RSVD1 */
			NVB_READ32(NV_ADDRESS_MAP_PP_PIN_MUX_CTL_C, reg);
			reg = NV_FLD_SET_DRF_DEF(APB_MISC_PP, PIN_MUX_CTL_C,
						 DDC_SEL, RSVD1, reg);
			NVB_WRITE32(NV_ADDRESS_MAP_PP_PIN_MUX_CTL_C, reg);
			break;
		default:
			break;
		}
		break;
	case 3: /* I2C3 */
		/* support pin_mux of 1 for now */
		/* which is DTF pin group */
		NVB_READ32(NV_ADDRESS_MAP_PP_PIN_MUX_CTL_G, reg);
		reg = NV_FLD_SET_DRF_DEF(APB_MISC_PP, PIN_MUX_CTL_G,
					 DTF_SEL, I2C3, reg);
		NVB_WRITE32(NV_ADDRESS_MAP_PP_PIN_MUX_CTL_G, reg);
		break;
	default:
		break;
	}
}

/**
 * state : 0 - NORMAL
 *	   1 - TRISTATE
 */
void i2c_pin_mux_tristate(int controller, int pin_mux, int tristate)
{
	NvU32	reg;

	switch (controller) {
	case 0:	/* I2CP */
		/* there is only one selection, pin_mux is ignored */
		NVB_READ32(NV_ADDRESS_MAP_PP_TRISTATE_REG_A, reg);
		reg = NV_FLD_SET_DRF_NUM(APB_MISC_PP, TRISTATE_REG_A,
					 Z_I2CP, tristate, reg);
		NVB_WRITE32(NV_ADDRESS_MAP_PP_TRISTATE_REG_A, reg);
		break;
	case 1:	/* I2C1 */
		/* support pin_mux of 1 for now */
		/* which is RM pin group */
		NVB_READ32(NV_ADDRESS_MAP_PP_TRISTATE_REG_A, reg);
		reg = NV_FLD_SET_DRF_NUM(APB_MISC_PP, TRISTATE_REG_A,
					 Z_RM, tristate, reg);
		NVB_WRITE32(NV_ADDRESS_MAP_PP_TRISTATE_REG_A, reg);
		break;
	case 2:	/* I2C2 */
		switch (pin_mux) {
		case 1:	/* DDC pin group */
			NVB_READ32(NV_ADDRESS_MAP_PP_TRISTATE_REG_B, reg);
			reg = NV_FLD_SET_DRF_NUM(APB_MISC_PP, TRISTATE_REG_B,
						 Z_DDC, tristate, reg);
			NVB_WRITE32(NV_ADDRESS_MAP_PP_TRISTATE_REG_B, reg);
			break;
		case 2:	/* PTA pin group */
			NVB_READ32(NV_ADDRESS_MAP_PP_TRISTATE_REG_A, reg);
			reg = NV_FLD_SET_DRF_NUM(APB_MISC_PP, TRISTATE_REG_A,
						 Z_PTA, tristate, reg);
			NVB_WRITE32(NV_ADDRESS_MAP_PP_TRISTATE_REG_A, reg);
			break;
		default:
			break;
		}
		break;
	case 3:	/* I2C3 */
		/* support pin_mux of 1 for now */
		/* which is DTF pin group */
		NVB_READ32(NV_ADDRESS_MAP_PP_TRISTATE_REG_D, reg);
		reg = NV_FLD_SET_DRF_NUM(APB_MISC_PP, TRISTATE_REG_D,
					 Z_DTF, tristate, reg);
		NVB_WRITE32(NV_ADDRESS_MAP_PP_TRISTATE_REG_D, reg);
		break;
	default:
		break;
	}
}

static void set_packet_mode(NvU32 controller)
{
	NvU32 config;
	NvU32 i2c_cnfg_reg;

	i2c_cnfg_reg = i2c_cnfg_addrs[controller];
	config = NV_DRF_DEF(I2C, I2C_CNFG, NEW_MASTER_FSM, ENABLE);
	config = NV_FLD_SET_DRF_DEF(I2C, I2C_CNFG, PACKET_MODE_EN, GO, config);
	NVB_WRITE32(i2c_cnfg_reg, config);
}

static void get_packet_headers(
	NvU32 controller,
	I2c_Transaction_Info *transaction_info,
	NvU32 packet_id,
	NvU32 *pkt_header1_ptr,
	NvU32 *pkt_header2_ptr,
	NvU32 *pkt_header3_ptr)
{
	NvU32 pkt_header1;
	NvU32 pkt_header2;
	NvU32 pkt_header3;

	// prepare Generic header1
	// Header size = 0 Protocol  = I2C,pktType = 0
	pkt_header1 = NV_DRF_DEF(I2C, IO_PACKET_HEADER, HDRSZ, ONE) |
			NV_DRF_DEF(I2C, IO_PACKET_HEADER, PROTOCOL, I2C);

	// Set pkt id
	pkt_header1 = NV_FLD_SET_DRF_NUM(I2C, IO_PACKET_HEADER, PKTID,
					 packet_id, pkt_header1);

	// Controller id is according to the instance of the i2c/dvc
	pkt_header1 = NV_FLD_SET_DRF_NUM(I2C, IO_PACKET_HEADER,
				CONTROLLER_ID, controller, pkt_header1);

	pkt_header2 = NV_FLD_SET_DRF_NUM(I2C, IO_PACKET_HEADER, PAYLOADSIZE,
				(transaction_info->num_bytes - 1), 0);

	// prepare IO specific header
	// Configure the slave address
	pkt_header3 = transaction_info->address;
	// Enable Read if it is required
	if (!(transaction_info->flags & I2C_IS_WRITE))
		pkt_header3 = NV_FLD_SET_DRF_DEF(I2C, IO_PACKET_HEADER, READ,
					READ, pkt_header3);

	*pkt_header1_ptr = pkt_header1;
	*pkt_header2_ptr = pkt_header2;
	*pkt_header3_ptr = pkt_header3;
}

/*
 * reset_tx_fifo():
 *	returns	 0: reset OK.
 *		-1: error resetting FIFO.
 */
static int reset_tx_fifo(NvU32 controller)
{
	NvU32 i2c_fifo_control_reg;
	NvU32 fifo_cntl;

	i2c_fifo_control_reg = i2c_fifo_control_addrs[controller];
	NVB_READ32(i2c_fifo_control_reg, fifo_cntl);
	fifo_cntl = NV_FLD_SET_DRF_DEF(I2C, FIFO_CONTROL, TX_FIFO_FLUSH, 
						SET, fifo_cntl);
	NVB_WRITE32(i2c_fifo_control_reg, fifo_cntl);
	NvBootUtilWaitUS(10);
	NVB_READ32(i2c_fifo_control_reg, fifo_cntl);
	if (fifo_cntl & NV_DRF_DEF(I2C, FIFO_CONTROL, TX_FIFO_FLUSH, SET))
		/* cannot reset the FIFO, return -1 (error */
		return -1;
	return 0;
}

/*
 * reset_rx_fifo():
 *	returns	 0: reset OK.
 *		-1: error resetting FIFO.
 */
static int reset_rx_fifo(NvU32 controller)
{
	NvU32 i2c_fifo_control_reg;
	NvU32 fifo_cntl;

	i2c_fifo_control_reg = i2c_fifo_control_addrs[controller];
	NVB_READ32(i2c_fifo_control_reg, fifo_cntl);
	fifo_cntl = NV_FLD_SET_DRF_DEF(I2C, FIFO_CONTROL, RX_FIFO_FLUSH, 
					SET, fifo_cntl);
	NVB_WRITE32(i2c_fifo_control_reg, fifo_cntl);
	NvBootUtilWaitUS(10);
	NVB_READ32(i2c_fifo_control_reg, fifo_cntl);
	if (fifo_cntl & NV_DRF_DEF(I2C, FIFO_CONTROL, RX_FIFO_FLUSH, SET))
		/* cannot reset the FIFO, return -1 (error) */
		return -1;
	return 0;
}

static int wait_for_tx_fifo_empty(NvU32 controller, int timeout_us)
{
	NvU32 i2c_fifo_status_reg;
	NvU32 fifo_status;
	NvU32 fifo_empty_count;

	i2c_fifo_status_reg = i2c_fifo_status_addrs[controller];
	while (timeout_us >= 0) {
		NVB_READ32(i2c_fifo_status_reg, fifo_status);
		fifo_empty_count = NV_DRF_VAL(I2C, FIFO_STATUS,
					TX_FIFO_EMPTY_CNT, fifo_status);
		if (fifo_empty_count == I2C_FIFO_DEPTH)
			return 1;
		NvBootUtilWaitUS(10);
		timeout_us -= 10;
	};
	return 0;
}

static int wait_for_rx_fifo_notempty(NvU32 controller, int timeout_us)
{
	NvU32 i2c_fifo_status_reg;
	NvU32 fifo_status;
	NvU32 fifo_full_count;

	i2c_fifo_status_reg = i2c_fifo_status_addrs[controller];
	while (timeout_us >= 0) {
		NVB_READ32(i2c_fifo_status_reg, fifo_status);
		fifo_full_count = NV_DRF_VAL(I2C, FIFO_STATUS,
					RX_FIFO_FULL_CNT, fifo_status);
		if (fifo_full_count)
			return 1;
		NvBootUtilWaitUS(10);
		timeout_us -= 10;
	};
	return 0;
}

static int wait_for_transfer_complete(NvU32 controller, int timeout_us)
{
	NvU32 i2c_packet_status_reg;
	NvU32 packet_status;

	i2c_packet_status_reg = i2c_packet_status_addrs[controller];
	while (timeout_us >= 0) {
		NVB_READ32(i2c_packet_status_reg, packet_status);
		if (packet_status & NV_DRF_DEF(I2C, PACKET_TRANSFER_STATUS,
						TRANSFER_COMPLETE, SET))
			return 1;
		NvBootUtilWaitUS(10);
		timeout_us -= 10;
	};
	return 0;
}

NvBootError tegra2_i2c_send_pkt_transaction(
	NvU32 controller,
	I2c_Transaction_Info *transaction_info,
	NvU32 clock_khz)
{
	NvU32 divisor;
	NvU32 reg;
	NvU32 osc_freq;
	NvU32 osc_freq_khz;
	NvU32 osc_freq_table[] = {13000, 19200, 12000, 26000};
	NvBootClocksClockId clock_id;
	NvBootResetDeviceId reset_id;
	NvU32 i2c_cnfg_reg;
	NvU32 i2c_tx_fifo_reg;
	int pin_mux;
	NvU32 packet_header1;
	NvU32 packet_header2;
	NvU32 packet_header3;
	NvU32 words_to_send;
	NvU32 *dptr;
	NvBootError error = NvBootError_Success;

	clock_id = i2c_clock_ids[controller];
	reset_id = i2c_reset_ids[controller];
	i2c_cnfg_reg = i2c_cnfg_addrs[controller];
	i2c_tx_fifo_reg = i2c_tx_fifo_addrs[controller];

	// Initialize.
	i2c_init_controller(controller);

	pin_mux = pin_mux_table[controller];
	i2c_pin_mux_select(controller, pin_mux);
	i2c_pin_mux_tristate(controller, pin_mux, 0); /* 0 means NORMAL */

	// Setup divisor.
	NVB_READ32(NV_ADDRESS_MAP_APB_CLR_RST_OSC_CTRL_REG, reg);
	osc_freq = NV_DRF_VAL(CLK_RST_CONTROLLER, OSC_CTRL, OSC_FREQ, reg);
	osc_freq_khz = osc_freq_table[osc_freq];
	divisor = (osc_freq_khz >> 3) / clock_khz;

	NvBootClocksConfigureClock(clock_id,
		divisor - 1,
		CLK_RST_CONTROLLER_CLK_SOURCE_DVC_I2C_0_DVC_I2C_CLK_SRC_CLK_M);

	/* set packet mode to config register */
	set_packet_mode(controller);

	get_packet_headers(controller, transaction_info, 1, &packet_header1, 
				&packet_header2, &packet_header3);

	if (reset_tx_fifo(controller)) {
		error = NvBootError_HwTimeOut;
		goto exit;
	}		

	// Words to write 
	words_to_send = BYTES_TO_WORD(transaction_info->num_bytes);

	//Write Generic Header1 & 2
	NVB_WRITE32(i2c_tx_fifo_reg, packet_header1);
	debug("pkt header 1 sent (0x%x)\n", packet_header1);
	NVB_WRITE32(i2c_tx_fifo_reg, packet_header2);
	debug("pkt header 2 sent (0x%x)\n", packet_header2);

	// Write I2C specific header
	NVB_WRITE32(i2c_tx_fifo_reg, packet_header3);
	debug("pkt header 3 sent (0x%x)\n", packet_header3);

	dptr = (NvU32 *)transaction_info->buf;
	while (words_to_send) {
		NVB_WRITE32(i2c_tx_fifo_reg, *dptr);
		debug("pkt data sent (0x%x)\n", *dptr);
		if (!wait_for_tx_fifo_empty(controller, I2C_TIMEOUT_USEC)) {
			error = NvBootError_HwTimeOut;
			goto exit;
		}
		words_to_send--;
		dptr++;
	}
	if (!wait_for_transfer_complete(controller, I2C_TIMEOUT_USEC)) {
		error = NvBootError_HwTimeOut;
		goto exit;
	}

exit:
	i2c_pin_mux_tristate(controller, pin_mux, 1); /* TRISTATE */
	NvBootClocksSetEnable(clock_id, NV_FALSE);
	return error;
}

NvBootError tegra2_i2c_receive_pkt_transaction(
	NvU32 controller,
	I2c_Transaction_Info *transaction_info,
	NvU32 clock_khz)
{
	NvU32 divisor;
	NvU32 reg;
	NvU32 osc_freq;
	NvU32 osc_freq_khz;
	NvU32 osc_freq_table[] = {13000, 19200, 12000, 26000};
	NvBootClocksClockId clock_id;
	NvBootResetDeviceId reset_id;
	NvU32 i2c_cnfg_reg;
	NvU32 i2c_tx_fifo_reg;
	NvU32 i2c_rx_fifo_reg;
	int pin_mux;
	NvU32 packet_header1;
	NvU32 packet_header2;
	NvU32 packet_header3;
	NvU32 words_to_receive;
	NvU32 *dptr;
	NvU32 local;
	uchar last_bytes;
	NvBootError error = NvBootError_Success;

	clock_id = i2c_clock_ids[controller];
	reset_id = i2c_reset_ids[controller];
	i2c_cnfg_reg = i2c_cnfg_addrs[controller];
	i2c_tx_fifo_reg = i2c_tx_fifo_addrs[controller];
	i2c_rx_fifo_reg = i2c_rx_fifo_addrs[controller];

	// Initialize.
	i2c_init_controller(controller);

	pin_mux = pin_mux_table[controller];
	i2c_pin_mux_select(controller, pin_mux);
	i2c_pin_mux_tristate(controller, pin_mux, 0); /* 0 means NORMAL */

	// Setup divisor.
	NVB_READ32(NV_ADDRESS_MAP_APB_CLR_RST_OSC_CTRL_REG, reg);
	osc_freq = NV_DRF_VAL(CLK_RST_CONTROLLER, OSC_CTRL, OSC_FREQ, reg);
	osc_freq_khz = osc_freq_table[osc_freq];
	divisor = (osc_freq_khz >> 3) / clock_khz;

	NvBootClocksConfigureClock(clock_id,
		divisor - 1,
		CLK_RST_CONTROLLER_CLK_SOURCE_DVC_I2C_0_DVC_I2C_CLK_SRC_CLK_M);

	/* set packet mode to config register */
	set_packet_mode(controller);

	get_packet_headers(controller, transaction_info, 1, &packet_header1, 
				&packet_header2, &packet_header3);

	if (reset_tx_fifo(controller)) {
		error = NvBootError_HwTimeOut;
		goto exit;
	}		
	if (reset_rx_fifo(controller)) {
		error = NvBootError_HwTimeOut;
		goto exit;
	}		

	// words to receive
	words_to_receive = BYTES_TO_WORD(transaction_info->num_bytes);
	last_bytes = (transaction_info->num_bytes % 4);

	//Write Generic Header1 & 2
	NVB_WRITE32(i2c_tx_fifo_reg, packet_header1);
	debug("pkt header 1 sent (0x%x)\n", packet_header1);
	NVB_WRITE32(i2c_tx_fifo_reg, packet_header2);
	debug("pkt header 2 sent (0x%x)\n", packet_header2);
	// Write I2C specific header
	NVB_WRITE32(i2c_tx_fifo_reg, packet_header3);
	debug("pkt header 3 sent (0x%x)\n", packet_header3);

	dptr = (NvU32 *)transaction_info->buf;
	while (words_to_receive) {
		if (!wait_for_rx_fifo_notempty(controller, I2C_TIMEOUT_USEC)) {
			error = NvBootError_HwTimeOut;
			goto exit;
		}
		/* for the last word, we read into our local buffer,
		 * in case that caller did not provide enough buffer.
		 */
		if ((words_to_receive == 1) && last_bytes) {
			NVB_READ32(i2c_rx_fifo_reg, local);
			memcpy( (uchar *)dptr, (char *)&local, last_bytes);
		} else {
			NVB_READ32(i2c_rx_fifo_reg, *dptr);
		}
		debug("pkt data received (0x%x)\n", *dptr);
		words_to_receive--;
		dptr++;
	}
	if (!wait_for_transfer_complete(controller, I2C_TIMEOUT_USEC)) {
		error = NvBootError_HwTimeOut;
		goto exit;
	}
exit:
	i2c_pin_mux_tristate(controller, pin_mux, 1); /* TRISTATE */
	NvBootClocksSetEnable(clock_id, NV_FALSE);
	return error;
}

NvBool tegra2_i2c_write_data(
	NvU32 addr,
	NvU8  *data,
	NvU32 len)
{
	NvBootError status = NvBootError_Success;
	I2c_Transaction_Info transaction_info = {0};

	transaction_info.address = addr;
	transaction_info.buf = data;
	transaction_info.flags = I2C_IS_WRITE;
	transaction_info.num_bytes = len;

	status = tegra2_i2c_send_pkt_transaction(i2c_bus_num,
						&transaction_info,
						I2CSPEED);
	if (status == NvBootError_Success) {
		return NV_TRUE;
	} else {
		switch (status) {
		case NvBootError_HwTimeOut:
			debug(("tegra2_i2c_write_data: Timeout\n")); 
			break;
		case NvBootError_SlaveNotFound:
		default:
			debug(("tegra2_i2c_write_data: NotFound\n"));
			break; 
		}
	}
	return NV_FALSE;
}

NvBool tegra2_i2c_read_data(
	NvU32 addr,
	NvU8  *data,
	NvU32 len)
{
	NvBootError status = NvBootError_Success;
	I2c_Transaction_Info transaction_info = {0};

	transaction_info.address = addr | 1;
	transaction_info.buf = data;
	transaction_info.num_bytes = len;

	status = tegra2_i2c_receive_pkt_transaction(i2c_bus_num,
						&transaction_info,
						I2CSPEED);
	if (status == NvBootError_Success) {
		return NV_TRUE;
	} else {
		switch (status) {
		case NvBootError_HwTimeOut:
			debug(("tegra2_i2c_read_data: Timeout\n")); 
			break;
		case NvBootError_SlaveNotFound:
		default:
			debug(("tegra2_i2c_read_data: NotFound\n"));
			break; 
		}
	}
	return NV_FALSE;
}

void i2c_init(int speed, int slaveaddr)
{
	debug("i2c_init(speed=%u, slaveaddr=0x%x)\n", speed, slaveaddr);
}

/* i2c write version without the register address */
int i2c_write_data(uchar chip, uchar *buffer, int len)
{	int rc;

	debug("i2c_write_data: chip=0x%x, len=0x%x\n", chip, len);
	debug("write_data: ");
	/* use rc for counter */
	for (rc=0; rc<len; ++rc) {
		debug(" 0x%02x", buffer[rc]);
	}
	debug("\n");

	rc = tegra2_i2c_write_data(chip*2, buffer, len);
	if (rc == NV_FALSE)
		return 1;
	return 0;
}

/* i2c write version without the register address */
int i2c_read_data(uchar chip, uchar *buffer, int len)
{	int rc;

	debug("inside i2c_read_data():\n");
	rc = tegra2_i2c_read_data(chip*2, buffer, len);
	if (rc == NV_FALSE)
		return 1;
	debug("i2c_read_data: ");
	/* reuse rc for counter*/
	for (rc=0; rc<len; ++rc) {
		debug(" 0x%02x", buffer[rc]);
	}
	debug("\n");
	return 0;
}

/*-----------------------------------------------------------------------
 * Probe to see if a chip is present.  Also good for checking for the
 * completion of EEPROM writes since the chip stops responding until
 * the write completes (typically 10mSec).
 */
int i2c_probe(uchar chip)
{	int rc;
	int reg;

	debug("i2c_probe: addr=0x%x\n", chip);
	reg = 0;
	rc = i2c_write_data(chip, (uchar *)&reg, 1);
	if (rc) {
		debug("Error probing 0x%x.\n", chip);
		return 1;
	}
	return 0;
}

/*-----------------------------------------------------------------------
 * Read bytes
 */
int i2c_read(uchar chip, uint addr, int alen, uchar *buffer, int len)
{	int rc;
	uchar *ptr = buffer;

	debug("i2c_read: chip=0x%x, addr=0x%x, len=0x%x\n",
				chip, addr, len);
	while (len) {
		rc = i2c_write_data(chip, (uchar *)&addr, 1);
		if (rc) {
			debug("i2c_read: error sending (0x%x)\n",
				addr);
			return 1;
		}
		rc = i2c_read_data(chip, ptr, 1);
		if (rc) {
			debug("i2c_read: error reading (0x%x)\n",
				addr);
			return 1;
		}
		++addr;
		++ptr;
		--len;
	}
	return 0;
}

/*-----------------------------------------------------------------------
 * Write bytes
 */
int i2c_write(uchar chip, uint addr, int alen, uchar *buffer, int len)
{	int rc;
	uchar local_buffer[260];
	uchar *ptr = buffer;

	debug("i2c_write: chip=0x%x, addr=0x%x, len=0x%x\n",
				chip, addr, len);

	while (len) {
		local_buffer[0] = addr & 0xFF;
		local_buffer[1] = *ptr;
		rc = i2c_write_data(chip, local_buffer, 2);
		if (rc) {
			debug("i2c_write: error sending (0x%x)\n",
						addr);
			return 1;
		}
		++addr;
		++ptr;
		--len;
	}
	return 0;
}

#if defined(CONFIG_I2C_MULTI_BUS)
/*
 * Functions for multiple I2C bus handling
 */
unsigned int i2c_get_bus_num(void)
{
	return i2c_bus_num;
}

int i2c_set_bus_num(unsigned int bus)
{
	if (bus >= CONFIG_SYS_MAX_I2C_BUS)
		return -1;
	i2c_bus_num = bus;

#ifdef DO_TPM_TEST
	/* TPM is on I2C2 bus only, and it is on Seaboard only */
	if (bus == 2) {
		printf("\ncalling do_i2c_tpmwd() ...\n");
		do_i2c_tpmwd();
		NvBootUtilWaitUS(1000000);
		printf("\ncalling do_i2c_tpmrd() ...\n");
		do_i2c_tpmrd();
		NvBootUtilWaitUS(1000000);
		printf("\ncalling do_i2c_tpmrs() ...\n");
		do_i2c_tpmrs();
		NvBootUtilWaitUS(1000000);
	}
#endif
	return 0;
}
#endif

/* This is included as an example to test TPM access */
#ifdef DO_TPM_TEST

u_char i2c_tpm_write_data[] = {
	0x02, 0x04, 0x00, 0x12, 0x14,
	0x01, 0x0b, 0x00, 0x00, 0x00, 0x0c,
	0x00, 0xc1, 0x00, 0x00, 0x00, 0x0c, 0x00, 0x00, 0x00, 0x99, 0x00, 0x01,
	0x53
};

u_char i2c_tpm_read_data[256];

int do_i2c_tpmwd ( void )
{
	u_char	chip;
	u_char	data[8];
	int	len, write_size;
	u_char	*ptr;
	int	rc;
	int	i;

	chip = 0x1a;
	data[0] = 0xa5;
	rc = i2c_write_data(chip, data, 1);
	if (rc) {
		printf("Error sending to 0xA5.\n");
		return 1;
	}
	len = sizeof(i2c_tpm_write_data);
	ptr = &i2c_tpm_write_data[0];
	printf("Data to send: ");
	for (i=0; i<len; ++i)
		printf(" 0x%02x", ptr[i]);
	printf("\n");
	while (len) {
		if (len >= 32)
			write_size = 32;
		else
			write_size = len;
		rc = i2c_write_data(chip, ptr, write_size);
		if (rc) {
			printf("Error sending data (remaining len=%u)\n", len);
			return 1;
		}
		len -= write_size;
		ptr += write_size;
	}
	return 0;
}

/* expect to receive: 
 *   02 04 00 10 16 01 0b 00 00 00 0a 00 c4 00 00 00 0a 00 00 00 00 ce
 * after do_i2c_tpmwd().
 */
int do_i2c_tpmrd ( void )
{
	u_char	chip;
	u_char	data[8];
	int	len;
	u_char	*ptr;
	int	rc;
	int	i;

	chip = 0x1a;
	data[0] = 0xa4;
	rc = i2c_write_data(chip, data, 1);
	if (rc) {
		printf("Error sending to 0xA4.\n");
		return 1;
	}

	/* receive response from TPM */
	ptr = i2c_tpm_read_data;
	len = 22;
	rc = i2c_read_data(chip, ptr, len);
	if (rc) {
		printf("Error reading data\n");
		return 1;
	}
	printf("Data read: ");
	for (i=0; i<len; ++i) {
		printf(" %02x", ptr[i]);
	}
	printf("\n");
	return 0;
}

int do_i2c_tpmrs ( void )
{
	u_char	chip;
	u_char	data[8];
	int	len;
	u_char	*ptr;
	int	rc;
	int	i;

	chip = 0x1a;
	data[0] = 0xa7;
	rc = i2c_write_data(chip, data, 1);
	if (rc) {
		printf("Error sending to 0xA7.\n");
		return 1;
	}

	/* receive response from TPM */
	ptr = i2c_tpm_read_data;
	len = 4;
	rc = i2c_read_data(chip, ptr, len);
	if (rc) {
		printf("Error reading data\n");
		return 1;
	}
	printf("Data read: ");
	for (i=0; i<len; ++i) {
		printf(" %02x", ptr[i]);
	}
	printf("\n");
	return 0;
}
#endif
