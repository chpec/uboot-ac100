/*
 * Copyright (c) 2009-2010, Code Aurora Forum. All rights reserved.
 *
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
 *
 * (C) Copyright 2002
 * David Mueller, ELSOFT AG, <d.mueller@elsoft.ch>
 *
 * (C) Copyright 2003
 * Texas Instruments, <www.ti.com>
 * Kshitij Gupta <Kshitij@ti.com>
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

#include <common.h>
#include <asm/arch/gpio.h>
#include <asm/arch/memtypes.h>

#ifdef CONFIG_CMD_USB
#include <usb.h>
#endif

#ifdef CONFIG_GENERIC_MMC
#include <mmc.h>
#endif

#ifdef CONFIG_QSD_SDCC
#include <qsd_sdcc.h>
#include <asm/arch/adm.h>

static struct mmc mmc_1;
static struct mmc mmc_2;
static sdcc_params_t sdcc_1;
static sdcc_params_t sdcc_3;

#define SDCC_1_VOLTAGE_SUPPORTED 0x00FF8080; /* 2.3-2.4 and  2.7-3.6 */
#define SDCC_3_VOLTAGE_SUPPORTED 0x00FF8000; /* 2.7-3.6 */
#endif /* CONFIG_MSM_SDCC */

extern int timer_init(void);

#define SUCCESS      0
#define ERROR       -1

/*
 * Optional boot progress function.
 */
#ifdef CONFIG_SHOW_BOOT_PROGRESS
void show_boot_progress(int progress)
{
	printf("Boot reached stage %d\n", progress);
}
#endif

static void board_ebi2_init(void);

/*
 * Miscellaneous platform dependent initialisations
 */
int board_init (void)
{
	DECLARE_GLOBAL_DATA_PTR;

	/* arch number of Qualcomm MSM */
	gd->bd->bi_arch_number = LINUX_MACH_TYPE;

	/* address of boot parameters */
	gd->bd->bi_boot_params = CFG_QC_BOOT_PARAM_ADDR;

	gd->flags = 0;

	/* Initialize GPT timer support */
	timer_init();

	/* Initialize EBI2 interface */
	board_ebi2_init();

#ifdef CONFIG_SILENT_CONSOLE
	gd->flags |= GD_FLG_SILENT;
#endif

#ifdef CONFIG_MSM8X60_LCDC
	gd->fb_base = LCDC_FB_ADDR;
#endif

	return SUCCESS;
}

#ifdef CONFIG_SYS_CONSOLE_OVERWRITE_ROUTINE
int overwrite_console(void)
{

#ifdef CONFIG_STDERR
	setenv("stderr", CONFIG_STDERR);
#endif

#ifdef CONFIG_STDOUT
	setenv("stdout", CONFIG_STDOUT);
#endif

#ifdef CONFIG_STDIN
	setenv("stdin", CONFIG_STDIN);
#endif

	return SUCCESS;
}
#endif /* CONFIG_SYS_CONSOLE_OVERWRITE_ROUTINE */

int misc_init_r (void)
{
	/* don't checksum loaded image */
	setenv("verify", "n");
	/* don't tftp images automatically for dhcp,bootp,rarp commands */
	setenv("autoload", "no");

	return SUCCESS;
}

/******************************
 Routine: cleanup_platform_before_linux
 Description: Calls various functions to deinitialize that were initialized.
******************************/
int cleanup_platform_before_linux(void)
{
	/* Defined in start.S, global to track warm booting */
	extern ulong _warmboot;

	/* Just about to boot the kernel, next power collapse should warm boot
	* on wakeup.
	*/
	_warmboot = CONFIG_WARMBOOT_TRUE;

	return SUCCESS;
}

#ifdef CONFIG_QSD_SDCC
int board_mmc_init(bd_t *bis)
{
	/* populate sdcc_1 struct with base addresses and other info */
	sdcc_1.instance             = 1;
	sdcc_1.base                 = SDC1_BASE;
	sdcc_1.ns_addr              = SDC1_NS_REG;
	sdcc_1.md_addr              = SDC1_MD_REG;
	sdcc_1.row_reset_mask       = ROW_RESET__SDC1___M;
	sdcc_1.glbl_clk_ena_mask    = GLBL_CLK_ENA__SDC1_H_CLK_ENA___M;
	sdcc_1.adm_crci_num         = ADM_CRCI_SDC1;

	/* GPIO config */
	sdcard_gpio_config(sdcc_1.instance);

	mmc_1.priv      = &sdcc_1;
	mmc_1.send_cmd  = sdcc_send_cmd;
	mmc_1.set_ios   = sdcc_set_ios;
	mmc_1.init      = sdcc_init;
	mmc_1.voltages  = SDCC_1_VOLTAGE_SUPPORTED;
	mmc_1.host_caps = MMC_MODE_4BIT |
			  MMC_MODE_8BIT |
			  MMC_MODE_HS |
			  MMC_MODE_HS_52MHz;

	mmc_1.f_min     = MCLK_400KHz;
	mmc_1.f_max     = MCLK_48MHz;
	sprintf(mmc_1.name, "Embedded_MMC");

	/* populate sdcc_3 struct with base addresses and other info */
	sdcc_3.instance             = 3;
	sdcc_3.base                 = SDC3_BASE;
	sdcc_3.ns_addr              = SDC3_NS_REG;
	sdcc_3.md_addr              = SDC3_MD_REG;
	sdcc_3.row_reset_mask       = ROW_RESET__SDC3___M;
	sdcc_3.glbl_clk_ena_mask    = GLBL_CLK_ENA__SDC3_H_CLK_ENA___M;
	sdcc_3.adm_crci_num         = ADM_CRCI_SDC3;

	sdcard_gpio_config(sdcc_3.instance);

	mmc_2.priv      = &sdcc_3;
	mmc_2.send_cmd  = sdcc_send_cmd;
	mmc_2.set_ios   = sdcc_set_ios;
	mmc_2.init      = sdcc_init;
	mmc_2.voltages  = SDCC_3_VOLTAGE_SUPPORTED;
	mmc_2.host_caps = MMC_MODE_4BIT |
			  MMC_MODE_HS |
			  MMC_MODE_HS_52MHz;

	mmc_2.f_min     = MCLK_400KHz;
	mmc_2.f_max     = MCLK_48MHz;
	sprintf(mmc_2.name, "External_Card");

	/* register our available mmc interfaces with mmc framework */
	mmc_register(&mmc_1);
	mmc_register(&mmc_2);

	return SUCCESS;
}

int board_sdcc_init(sdcc_params_t *sd)
{
	if(sd->instance == 1 || sd->instance == 3){
		return SUCCESS;
	}
	else{
		/* this board does not have an sd/mmc card on this interface. */
		return ERROR;
	}
}
#endif /* CONFIG_MSM_SDCC */

int board_eth_init(bd_t *bis)
{
	int rc = 0;
	rc = smc911x_initialize(0, CONFIG_SMC911X_BASE);
	return rc;
}

void board_ebi2_init(void)
{
	/* EBI2_CHIP_SELECT_CFG0 -- enable cs2 and cs5 */
	IO_WRITE32(0x1A100000, 0x00000B11);

	/* Configure GPIOs used by ebi2 interface */

	/* EBI2 CS2 */
	gpio_tlmm_config(40, 2, GPIO_OUTPUT, GPIO_PULL_UP,
			GPIO_16MA, GPIO_ENABLE);

	/* EBI2 address */
	gpio_tlmm_config(123, 1, GPIO_OUTPUT, GPIO_PULL_UP,
			GPIO_16MA, GPIO_ENABLE);
	gpio_tlmm_config(124, 1, GPIO_OUTPUT, GPIO_PULL_UP,
			GPIO_16MA, GPIO_ENABLE);
	gpio_tlmm_config(125, 1, GPIO_OUTPUT, GPIO_PULL_UP,
			GPIO_16MA, GPIO_ENABLE);
	gpio_tlmm_config(126, 1, GPIO_OUTPUT, GPIO_PULL_UP,
			GPIO_16MA, GPIO_ENABLE);
	gpio_tlmm_config(127, 1, GPIO_OUTPUT, GPIO_PULL_UP,
			GPIO_16MA, GPIO_ENABLE);
	gpio_tlmm_config(128, 1, GPIO_OUTPUT, GPIO_PULL_UP,
			GPIO_16MA, GPIO_ENABLE);
	gpio_tlmm_config(129, 1, GPIO_OUTPUT, GPIO_PULL_UP,
			GPIO_16MA, GPIO_ENABLE);
	gpio_tlmm_config(130, 1, GPIO_OUTPUT, GPIO_PULL_UP,
			GPIO_16MA, GPIO_ENABLE);

	/* EBI2 address-data */
	gpio_tlmm_config(135, 1, GPIO_OUTPUT, GPIO_PULL_UP,
			GPIO_16MA, GPIO_ENABLE);
	gpio_tlmm_config(136, 1, GPIO_OUTPUT, GPIO_PULL_UP,
			GPIO_16MA, GPIO_ENABLE);
	gpio_tlmm_config(137, 1, GPIO_OUTPUT, GPIO_PULL_UP,
			GPIO_16MA, GPIO_ENABLE);
	gpio_tlmm_config(138, 1, GPIO_OUTPUT, GPIO_PULL_UP,
			GPIO_16MA, GPIO_ENABLE);
	gpio_tlmm_config(139, 1, GPIO_OUTPUT, GPIO_PULL_UP,
			GPIO_16MA, GPIO_ENABLE);
	gpio_tlmm_config(140, 1, GPIO_OUTPUT, GPIO_PULL_UP,
			GPIO_16MA, GPIO_ENABLE);
	gpio_tlmm_config(141, 1, GPIO_OUTPUT, GPIO_PULL_UP,
			GPIO_16MA, GPIO_ENABLE);
	gpio_tlmm_config(142, 1, GPIO_OUTPUT, GPIO_PULL_UP,
			GPIO_16MA, GPIO_ENABLE);
	gpio_tlmm_config(143, 1, GPIO_OUTPUT, GPIO_PULL_UP,
			GPIO_16MA, GPIO_ENABLE);
	gpio_tlmm_config(144, 1, GPIO_OUTPUT, GPIO_PULL_UP,
			GPIO_16MA, GPIO_ENABLE);
	gpio_tlmm_config(145, 1, GPIO_OUTPUT, GPIO_PULL_UP,
			GPIO_16MA, GPIO_ENABLE);
	gpio_tlmm_config(146, 1, GPIO_OUTPUT, GPIO_PULL_UP,
			GPIO_16MA, GPIO_ENABLE);
	gpio_tlmm_config(147, 1, GPIO_OUTPUT, GPIO_PULL_UP,
			GPIO_16MA, GPIO_ENABLE);
	gpio_tlmm_config(148, 1, GPIO_OUTPUT, GPIO_PULL_UP,
			GPIO_16MA, GPIO_ENABLE);
	gpio_tlmm_config(149, 1, GPIO_OUTPUT, GPIO_PULL_UP,
			GPIO_16MA, GPIO_ENABLE);
	gpio_tlmm_config(150, 1, GPIO_OUTPUT, GPIO_PULL_UP,
			GPIO_16MA, GPIO_ENABLE);

	/* EBI2 OE */
	gpio_tlmm_config(151, 1, GPIO_OUTPUT, GPIO_PULL_UP,
			GPIO_16MA, GPIO_ENABLE);

	/* EBI2 WE */
	gpio_tlmm_config(157, 1, GPIO_OUTPUT, GPIO_PULL_UP,
			GPIO_16MA, GPIO_ENABLE);
}

void sdcard_gpio_config(int instance)
{
	unsigned int addr = 0;
	unsigned int val  = 0;

	switch (instance) {
		case 1:
			gpio_tlmm_config(159, 1, GPIO_OUTPUT, GPIO_PULL_UP,
					GPIO_8MA, GPIO_ENABLE);
			gpio_tlmm_config(160, 1, GPIO_OUTPUT, GPIO_PULL_UP,
					GPIO_8MA, GPIO_ENABLE);
			gpio_tlmm_config(161, 1, GPIO_OUTPUT, GPIO_PULL_UP,
					GPIO_8MA, GPIO_ENABLE);
			gpio_tlmm_config(162, 1, GPIO_OUTPUT, GPIO_PULL_UP,
					GPIO_8MA, GPIO_ENABLE);
			gpio_tlmm_config(163, 1, GPIO_OUTPUT, GPIO_PULL_UP,
					GPIO_8MA, GPIO_ENABLE);
			gpio_tlmm_config(164, 1, GPIO_OUTPUT, GPIO_PULL_UP,
					GPIO_8MA, GPIO_ENABLE);
			gpio_tlmm_config(165, 1, GPIO_OUTPUT, GPIO_PULL_UP,
					GPIO_8MA, GPIO_ENABLE);
			gpio_tlmm_config(166, 1, GPIO_OUTPUT, GPIO_PULL_UP,
					GPIO_8MA, GPIO_ENABLE);
			gpio_tlmm_config(167, 1, GPIO_OUTPUT, GPIO_NO_PULL,
					GPIO_8MA, GPIO_ENABLE);
			gpio_tlmm_config(168, 1, GPIO_OUTPUT, GPIO_PULL_UP,
					GPIO_8MA, GPIO_ENABLE);
			break;
		case 3:
			/* Set up HDrive and Pull values for SDC3
			 * interface pins
			 */
			addr = TLMM_BASE_ADDR + SDC3_HDRV_PULL_CTL;
			val =	(SDC3_PULL_UP  << SDC3_CMD_PULL__S ) |
				(SDC3_PULL_UP  << SDC3_DATA_PULL__S) |
				(SDC3_HDRV_8MA << SDC3_CLK_HDRV__S ) |
				(SDC3_HDRV_8MA << SDC3_CMD_HDRV__S ) |
				(SDC3_HDRV_8MA << SDC3_DATA_HDRV__S);
			IO_WRITE32(addr,val);
			break;
		default:
			break;
	} /* End of switch statement */
}
