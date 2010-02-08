/*
 * Copyright (c) 2009, Code Aurora Forum. All rights reserved.
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
#endif
#include <asm/arch/memtypes.h>

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

    // Write to the SURF LEDs
    IO_WRITE16(HAPPY_LED_BASE_BANK2, 0x0001);

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

    // Write to the SURF LEDs
    IO_WRITE16(HAPPY_LED_BASE_BANK2, 0x0002);

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
    // Cleanup SD resources if they were used
    SDCn_deinit(SDC_INSTANCE);
#endif

    // Write to the SURF LEDs
    IO_WRITE16(HAPPY_LED_BASE_BANK2, 0x0004);

    /* Just about to boot the kernel, next power collapse should warm boot
     * on wakeup.
     */
    _warmboot = CONFIG_WARMBOOT_TRUE;

    /* Timer is 32bit, and running off 32Khz (on surf), will overflow after 131072 sec */
    printf("Time Elapsed since timer start:%lu s\n", get_timer(0) / CONFIG_SYS_HZ);
    return(0);
}

void board_lcd_enable(void)
{
}

void board_lcd_disble(void)
{
}
