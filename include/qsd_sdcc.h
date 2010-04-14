/*
Copyright (c) 2010, Code Aurora Forum. All rights reserved.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License version 2 and
only version 2 as published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
02110-1301, USA.
*/
#ifndef _QSD_SDCC_H_
#define _QSD_SDCC_H_

#define BLOCK_SIZE         512
#define SDCC_FIFO_SIZE     64

#define BUS_WIDTH_1     0
#define BUS_WIDTH_4     2
#define BUS_WIDTH_8     3

#define CMD17           17
#define CMD18           18
#define CMD24           24
#define CMD25           25
#define CMD53           53

/* HCLK_FREQ is only used to set timeout values.  This is not critical,
 * as higher HCLK_FREQ will simply create a larger timeout count.
 */
#define HCLK_FREQ         20e6
#define WR_DATA_TIMEOUT   (int)(HCLK_FREQ * 250e-3)
#define RD_DATA_TIMEOUT   (int)(HCLK_FREQ * 100e-3)

#ifdef USE_PROC_COMM
/* the desired duty cycle is 50%,
 * using proc_comm with 45Mhz possibly giving too low duty cycle,
 * breaking it.
 */
enum SD_MCLK_speed
{	MCLK_144KHz = 144000,
	MCLK_400KHz = 400000,
	MCLK_25MHz = 25000000,
	MCLK_48MHz = 49152000, /* true 48Mhz not supported, use next highest */
	MCLK_49MHz = 49152000,
	MCLK_50MHz = 50000000,
};
#else /*USE_PROC_COMM defined*/
enum SD_MCLK_speed
{
/* 	MCLK_144KHz, //not implemented w/o proc_comm */
	MCLK_400KHz,
	MCLK_25MHz,
	MCLK_48MHz,
/*	MCLK_49MHz, //not implemented w/o proc_comm */
	MCLK_50MHz
};
#endif /*USE_PROC_COMM */


#define SDCC_ERR_GENERIC        -1
#define SDCC_ERR_CRC_FAIL       -2
#define SDCC_ERR_DATA_WRITE     -3
#define SDCC_ERR_BLOCK_NUM      -4
#define SDCC_ERR_RX_OVERRUN     -5
#define SDCC_ERR_DATA_TIMEOUT   -6
#define SDCC_ERR_DATA_CRC_FAIL  -7
#define SDCC_ERR_DATA_ADM_ERR   -8
#define SDCC_ERR_DATA_START_BIT -9
#define SDCC_ERR_INVALID_STATUS -10
#define SDCC_ERR_TIMEOUT        -19 /* Must match with the time out error value defined in mmc.h of mmc framework. */
#define SDCC_ERR_NOT_IMPLEMENTED   -111

int  sdcc_init(struct mmc *mmc);
void sdcc_set_ios(struct mmc *mmc);
int  sdcc_send_cmd(struct mmc *mmc, struct mmc_cmd *cmd, struct mmc_data *data);

typedef struct sd_parms {
   uint32_t instance;                  /* which instance of the SD controller */
   uint32_t base;                      /* SD controller base address */
   uint32_t ns_addr;                   /* Clock controller NS reg address */
   uint32_t md_addr;                   /* Clock controller MD reg address */
   uint32_t ns_initial;                /* Clock controller NS reg initial value */
   uint32_t md_initial;                /* Clock controller MD reg initial value */
   uint32_t row_reset_mask;            /* Bit in the ROW reset register */
   uint32_t glbl_clk_ena_mask;         /* Bit in the global clock enable */
   uint32_t glbl_clk_ena_initial;      /* Initial value of the global clock enable bit */
   uint32_t adm_crci_num;              /* ADM CRCI number */
   uint32_t adm_ch8_rslt_conf_initial; /* Initial value of HI0_CH8_RSLT_CONF_SD3 */
} sdcc_params_t;

#endif
