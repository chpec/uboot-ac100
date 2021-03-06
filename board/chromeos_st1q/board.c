/*
 * Copyright (c) 2009-2010 Code Aurora Forum. All rights reserved.
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
#include <linux/mtd/nand.h>
#ifdef CONFIG_CMD_USB
#include <usb.h>
#endif
#ifdef USE_PROC_COMM
#include <asm/arch/proc_comm.h>
#include <asm/arch/proc_comm_clients.h>
#endif
#include <asm/arch/memtypes.h>

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

#define SDCC_1_VOLTAGE_SUPPORTED 0x00FF8000;
#define SDCC_3_VOLTAGE_SUPPORTED 0x00FF8080;
#endif

extern int timer_init(void);
extern void scorpion_pll_init (void);
extern void SDCn_deinit(uint32_t instance);

/*
 * Optional boot progress function.
 */
#ifdef CONFIG_SHOW_BOOT_PROGRESS
void show_boot_progress(int progress)
{
    printf("Boot reached stage %d\n", progress);
}
#endif

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

    /* When released from reset, Scorpion gets its clock from the AXI bus clock.
     * Start up the Scorpion PLL and start using it instead.
     */
    scorpion_pll_init();

#ifdef CONFIG_SILENT_CONSOLE
    gd->flags |= GD_FLG_SILENT;
#endif

    /*
     * Before proceeding, wait until ARM9 has finished booting.
     */
#ifdef USE_PROC_COMM
    proc_comm_init();
    proc_comm_wait_for_modem_ready();
#else

    udelay(5000000);
#endif

#ifdef CONFIG_QSD8X50_LCDC
    gd->fb_base = LCDC_FB_ADDR;
#endif

    return 0;
}

#ifdef CONFIG_SYS_CONSOLE_OVERWRITE_ROUTINE
int overwrite_console(void)
{
#ifdef CONFIG_USB_KEYBOARD
   static int usb_init_called = 0;
#endif

#ifdef CONFIG_STDERR
   setenv("stderr", CONFIG_STDERR);
#endif

#ifdef CONFIG_STDOUT
    setenv("stdout", CONFIG_STDOUT);
#endif

#ifdef CONFIG_STDIN
    setenv("stdin", CONFIG_STDIN);
#endif

#if defined(CONFIG_USB_KEYBOARD) && defined(CONFIG_CMD_USB)
    if (strcmp(CONFIG_STDIN, "usbkbd") == 0)
    {
        if (usb_init_called == 0)
        {
            usb_init_called = 1;
            usb_stop();
            usb_init();
        }
    }
#endif /* CONFIG_USB_KEYBOARD */

    return 0;
}
#endif /* CONFIG_SYS_CONSOLE_OVERWRITE_ROUTINE */

int misc_init_r (void)
{
    //dont checksum loaded image
    setenv("verify", "n");
    //dont tftp images automatically for dhcp,bootp,rarp commands
    setenv("autoload", "no");
	return (0);
}

/******************************
 Routine: cleanup_platform_before_linux
 Description: Calls various functions to deinitialize that were initialized.
******************************/
int cleanup_platform_before_linux(void)
{
    /* Defined in start.S, global to track warm booting */
    extern ulong _warmboot;
#ifdef CONFIG_MMC
#ifndef CONFIG_GENERIC_MMC
    // Cleanup SD resources if they were used
    SDCn_deinit(SDC_INSTANCE);
#endif
#endif

    /* Just about to boot the kernel, next power collapse should warm boot
     * on wakeup.
     */
    _warmboot = CONFIG_WARMBOOT_TRUE;

    return(0);
}

void board_lcd_enable(void)
{
    const uint	GPIO_BANK1 = 0xa9100c00;
    const uint	GPIO_BANK2 = 0xa9000804;

    uint	value = IO_READ32(GPIO_BANK1);

    value |= (1 << 4) | (1 << 16);

    IO_WRITE32(GPIO_BANK1, value);

    value = IO_READ32(GPIO_BANK2);

    value |= (1 << 18);

    IO_WRITE32(GPIO_BANK2, value);
}

void board_lcd_disble(void)
{
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
	proc_comm_sdcard_gpio_config(1);

    mmc_1.priv      = &sdcc_1;
    mmc_1.send_cmd  = sdcc_send_cmd;
    mmc_1.set_ios   = sdcc_set_ios;
    mmc_1.init      = sdcc_init;
    mmc_1.voltages  = SDCC_1_VOLTAGE_SUPPORTED ;
    mmc_1.host_caps = MMC_MODE_4BIT | MMC_MODE_HS | MMC_MODE_HS_52MHz;
    mmc_1.f_min     = 400000;
    mmc_1.f_max     = MCLK_48MHz;
    sprintf(mmc_1.name, "External_Card");


    /* populate sdcc_3 struct with base addresses and other info */
    sdcc_3.instance             = 3;
    sdcc_3.base                 = SDC3_BASE;
    sdcc_3.ns_addr              = SDC3_NS_REG;
    sdcc_3.md_addr              = SDC3_MD_REG;
    sdcc_3.row_reset_mask       = ROW_RESET__SDC3___M;
    sdcc_3.glbl_clk_ena_mask    = GLBL_CLK_ENA__SDC3_H_CLK_ENA___M;
    sdcc_3.adm_crci_num         = ADM_CRCI_SDC3;

    /* GPIO config */
    proc_comm_sdcard_gpio_config(3);

    mmc_2.priv      = &sdcc_3;
    mmc_2.send_cmd  = sdcc_send_cmd;
    mmc_2.set_ios   = sdcc_set_ios;
    mmc_2.init      = sdcc_init;
    mmc_2.voltages  = SDCC_3_VOLTAGE_SUPPORTED;
    mmc_2.host_caps = MMC_MODE_4BIT | MMC_MODE_8BIT | MMC_MODE_HS | MMC_MODE_HS_52MHz;
    mmc_2.f_min     = 400000;
    mmc_2.f_max     = MCLK_48MHz;
    sprintf(mmc_2.name, "Embedded_MMC");


    /* register our available mmc interfaces with mmc framework */
    mmc_register(&mmc_1);
    mmc_register(&mmc_2);

    return 0;
}

int board_sdcc_init(sdcc_params_t *sd)
{
    if(sd->instance == 1)
    {
        /* Power cycle the card. */
        proc_comm_vreg_control(PM_VREG_GP6_ID, 2850, 0);
        udelay(1000);
        proc_comm_vreg_control(PM_VREG_GP6_ID, 2850, 1);
    }
    else if(sd->instance == 3)
    {
        /* Power cycle the card. */
        proc_comm_vreg_control(PM_VREG_GP6_ID, 2850, 0);
        proc_comm_vreg_control(PM_VREG_WLAN_ID, 2850, 0);
        udelay(1000);
        proc_comm_vreg_control(PM_VREG_GP6_ID, 2850, 1);
        proc_comm_vreg_control(PM_VREG_WLAN_ID, 2850, 1);
    }
    else
    {
        /* this board does not have an sd/mmc card on this interface. */
        return -1;
    }

    return 0;
}
#endif
