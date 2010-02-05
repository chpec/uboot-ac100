/*
 * Copyright (c) 2009, Code Aurora Forum. All rights reserved.
 *
 * (C) Copyright 2002-2005
 * Wolfgang Denk, DENX Software Engineering, <wd@denx.de>
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
 * Gary Jennejohn <gj@denx.de>
 *
 * Configuation settings for the QUALCOMM QSD8x50 SURF board.
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

#ifndef __CONFIG_H
#define __CONFIG_H

#include <asm/arch/QSD8x50_reg.h>
#include <asm-armv7Scorpion/armv7Scorpion.h>
#include <config.h>


#define IO_READ32(addr)        (*((volatile unsigned int *) (addr)))
#define IO_WRITE32(addr, val)  (*((volatile unsigned int *) (addr)) = ((unsigned int) (val)))
#define IO_READ16(addr)        (*((volatile unsigned short *) (addr)))
#define IO_WRITE16(addr, val)  (*((volatile unsigned short *) (addr)) = ((unsigned short) (val)))
#define IO_READ8(addr)         (*((volatile char *) (addr)))
#define IO_WRITE8(addr, val)   (*((volatile unsigned char *) (addr)) = ((unsigned char) (val)))

/*
 * High Level Configuration Options
 * (easy to change)
 */

#define CONFIG_DCACHE

#define CONFIG_STACK_BASE

#ifndef MACH_QSD8X50_SURF
	#define MACH_QSD8X50_SURF 1008000
#endif

#define LINUX_MACH_TYPE	MACH_QSD8X50_SURF

#define CONFIG_SYS_HZ                  (32768)           /* GPT Timer frequency */
#define CONFIG_TCXO_HZ		19200000 /* TCX0 frequency */

#undef  CONFIG_SHOW_BOOT_PROGRESS

#define CONFIG_CMDLINE_TAG	   	  /* enable passing of ATAGs  */
#define CONFIG_SETUP_MEMORY_TAGS
#define CONFIG_INITRD_TAG

/*
 * Enable to pass framebuffer info to kernel
 * but after implementing calc_fbsize() in lcdc.c
 */
#define CONFIG_VIDEOFLB_ATAG_NOT_SUPPORTED

#define ATAG_CORE_FLAGS 	0x00000001
#define ATAG_PAGE_SIZE	  	0x00001000
#define ATAG_CORE_RDEV		0x000000FF

#define CONFIG_MISC_INIT_R	1	/* call misc_init_r during start up */

/* Warm boot related constants */
#define CONFIG_WARMBOOT_TRUE			0xBAC4F00D
#define CONFIG_WARMBOOT_FALSE   		0x15FA15E2

/*
 * Address where ATAGs are stored and where bootwedge used to be.
 * 2 words at ATAGs are overwritten by power collapse routine
 * in the kernel to cause a jump back to the
 * power collapse exit routine in the kernel.
 * These are restored back to their original values
 * after successful power collapse .
 */
#define CONFIG_WARMBOOT_POWER_COLLAPSE_EXIT_ADDRESS 	PHYS_SDRAM_1

/*
 * Size of malloc() pool
 */
#define CONFIG_SYS_MALLOC_LEN	(CONFIG_ENV_SIZE + 128*1024)

/* size in bytes reserved for initial data */
#define CONFIG_SYS_GBL_DATA_SIZE	128

/*
 * Stack space needed =
 * Stack + Global Data + 3 words abort stack
 * This is checked by linker script to define stack section
 */
#define CONFIG_SYS_MAX_STACK_SPACE (CONFIG_STACKSIZE + \
		                    CONFIG_SYS_GBL_DATA_SIZE + 12)

#define HAPPY_LED_BASE_BANK1          (EBI2CS7_BASE + 0x0282)
#define HAPPY_LED_BASE_BANK2          (EBI2CS7_BASE + 0x0284)

/*
 * MMC/SD card
 */
#define CONFIG_MMC
#define CONFIG_DOS_PARTITION

#define PROC_COMM_MPP_FOR_USB_VBUS PM_MPP_16
#undef  USE_PROC_COMM_USB_PHY_RESET /* proc_comm cmd to reset phy not working
                                       rt now, but eventually it will*/

/*
 * Monitor functions
 * Defaults + some additional
 */
#include <config_cmd_default.h>

#undef  CONFIG_CMD_FLASH     // no flash support
#undef  CONFIG_CMD_IMLS
#undef  CONFIG_CMD_NET
#undef  CONFIG_CMD_NFS
#define CONFIG_CMD_MMC
#define CONFIG_CMD_FAT
#define CONFIG_CMD_EXT2
#define CONFIG_CMD_NAND
#define CONFIG_CMD_RAMCONFIG
#define CONFIG_CMD_GETTIME
#define CONFIG_DYNAMIC_INODE_SIZE

/*
 * Serial port Configuration
 */
#undef  CONFIG_SILENT_CONSOLE
#define CFG_QC_SERIAL
#define CONFIG_CONS_INDEX	 0
#define CONFIG_BAUDRATE	     115200
#define CONFIG_SYS_BAUDRATE_TABLE   { 9600, 19200, 38400, 57600, 115200 }
/*
 * ARM DCC Console
 */
#define CONFIG_ARM_DCC
#define CONFIG_ARM_DCC_MULTI
#define CONFIG_ARM_DCC_SCORPION /* Use ARM DCC on Scorpion side */

#define CONFIG_BOOTP_MASK	CONFIG_BOOTP_DEFAULT

#define CONFIG_BOOTDELAY	4
#define CONFIG_BOOTARGS "init=/sbin/init root=/dev/mmcblk0p1 r0 rootdelay=4 ip=dhcp"

/* boot cmd: Attempt to load the boot script from mmc card and execute that script.
 *           If script does not exists, attempt booting with vmlinux from mmc.
 */
#define CONFIG_BOOTCOMMAND "if mmcinit; then "                                                  \
                               "if ext2load mmc 0 0x2d000000 boot-script.uimg; then "           \
                                   "autoscr 0x2d000000; "                                       \
                                "else "                                                         \
                                   "echo boot-script.uimg not found; "                          \
                                   "if ext2load mmc 0 0x20007fc0 vmlinux.uimg; then "           \
                                       "bootm 0x20007fc0; "                                     \
                                    "else "                                                     \
                                       "echo vmlinux.uimg not found; "                          \
                                    "fi; "                                                      \
                                "fi; "                                                          \
                            "else "                                                             \
                                "echo Nothing to do...; "                                       \
                            "fi;"


/*
 * Miscellaneous configurable options
 */
#define CONFIG_SYS_NO_FLASH
#define CONFIG_SYS_LONGHELP	/* undef to save memory     */
#define CONFIG_SYS_HUSH_PARSER
#define CONFIG_SYS_PROMPT_HUSH_PS2      ">"
#define CONFIG_SYS_PROMPT	"QSD8x50_SURF-QC> "	/* Monitor Command Prompt   */
#define CONFIG_SYS_CBSIZE	256		/* Console I/O Buffer Size  */
/* Print Buffer Size */
#define CONFIG_SYS_PBSIZE	(CONFIG_SYS_CBSIZE+sizeof(CONFIG_SYS_PROMPT)+16)
#define CONFIG_SYS_MAXARGS	16		/* max number of command args   */
#define CONFIG_SYS_BARGSIZE	2048 /* Boot Argument Buffer Size    */

#define CONFIG_SYS_LOAD_ADDR	(PHYS_SDRAM_1 + 0x7FC0) 	/* default load address in EBI1 SDRAM */

/*-----------------------------------------------------------------------
 * Stack sizes
 *
 * The stack sizes are set up in start.S using the settings below
 */
#define CONFIG_STACKSIZE	(128*1024)	/* regular stack */
#ifdef CONFIG_USE_IRQ
#define CONFIG_STACKSIZE_IRQ	(4*1024)	/* IRQ stack */
#define CONFIG_STACKSIZE_FIQ	(4*1024)	/* FIQ stack */
#endif

#define CONFIG_NEW_MEM_MAP

#ifdef CONFIG_NEW_MEM_MAP
#   define CONFIG_NR_DRAM_BANKS    2	  /* SDRAM is split into 2 regions
                                           * to accomodate ADSP as a memory hole.
                                           */
#   define PHYS_SDRAM_1            0x20000000	    /* EBI1 start */
#   define PHYS_SDRAM_1_SIZE       0x0E000000	    /* 256 - 32(adsp)
                                                       = 224MB (0x0E000000)*/
#   define PHYS_SDRAM_2            0x30000000	    /* EBI1, AFTER ADSP */

#ifdef CONFIG_SDRAM_256MB
#   define PHYS_SDRAM_2_SIZE       0x00000000	    /* The other 256 MB */
#elif defined(CONFIG_SDRAM_512MB)
#   define PHYS_SDRAM_2_SIZE       0x10000000	    /* For 512MB on new memory map */
#elif defined(CONFIG_SDRAM_1024MB)
#   define PHYS_SDRAM_2_SIZE       0x30000000	    /* For 1024MB on new memory map */
#endif /*CONFIG_SDRAM_256MB*/

#else
#   define CONFIG_NR_DRAM_BANKS    1	  /* There is 1 bank of SDRAM */
#   define PHYS_SDRAM_1            0x16000000	    /* EBI1 */
#ifdef CONFIG_SDRAM_256MB
#   define PHYS_SDRAM_1_SIZE       0x0A000000	/* For 256 on old memory map*/
#elif defined(CONFIG_SDRAM_512MB
#   define PHYS_SDRAM_1_SIZE       0x1A000000	*/  /* For 512MB on old memory map*/
#endif /*CONFIG_SDRAM_256MB*/

#endif /*CONFIG_NEW_MEM_MAP*/

#define CONFIG_SERIAL_CONSOLE
#define CONFIG_SYS_WHITE_ON_BLACK       /*Console colors*/

#if defined(CONFIG_SERIAL_CONSOLE)
#   define CONFIG_STDOUT "serial"
#   define CONFIG_STDERR "serial"
#   define CONFIG_STDIN  "serial"
#else
#   define CONFIG_STDOUT "dcc"
#   define CONFIG_STDERR "dcc"
#   define CONFIG_STDIN  "dcc"
#endif

/* use env values of stdin, stdout, stderr*/
#define CONFIG_SYS_CONSOLE_IS_IN_ENV

/* Overwrite env values of stdin, stdout, stderr.
   Our routine overwrites with CONFIG_STD* values */
#define CONFIG_SYS_CONSOLE_OVERWRITE_ROUTINE

/*-----------------------------------------------------------------------
 * Shared Memory Location -
 */
#define SMEM_START  0x00100000
#define SMEM_SIZE   0x00100000

/*-----------------------------------------------------------------------
 * Physical Memory Map -
 * U-Boot code, data, stack, etc. reside in SMI SDRAM 0x00000000-0x000FFFFF.
 * There are similar parameters in the u-boot.lds linker script which also
 * need to be updated.
 */
#define UBOOT_SDRAM_BASE         0x00000000      /* SMI */
#define UBOOT_SDRAM_SIZE         0x00100000

/* Memory Test */
#define CONFIG_SYS_MEMTEST_START       PHYS_SDRAM_1
#define CONFIG_SYS_MEMTEST_END         (PHYS_SDRAM_1 + PHYS_SDRAM_1_SIZE)

/* Environment */
#define CONFIG_ENV_IS_NOWHERE
#define CONFIG_ENV_SIZE         0x2000

/* Boot parameter address */
#define CFG_QC_BOOT_PARAM_ADDR    PHYS_SDRAM_1

/*-----------------------------------------------------------------------
 * The qc_serial driver uses the register names below. Set UART_BASE
 * for the desired UART.
 */
#define UART_BASE     UART3_BASE


/*-----------------------------------------------------------------------
 * Choose the SD controller to use. SDC1, 2, 3, or 4.
 */
#define SDC_INSTANCE  1
#define USE_DM
#define USE_HIGH_SPEED_MODE
#define USE_4_BIT_BUS_MODE
#define CONFIG_SYS_MMC_BASE		0xF0000000    // not used, but defined to prevent compile error
#define PROC_COMM_VREG_SDC  PM_VREG_GP5_ID

/*-----------------------------------------------------------------------
 * NAND configuration
 */
#define CONFIG_USE_ACCELERATED_PAGE_READ

#define CONFIG_SYS_MAX_NAND_DEVICE  1
#define CONFIG_SYS_NAND_BASE        0xF0000000  // not used, but defined to prevent compile error

// NAND device specific register values for the NAND controller
// These values are for the Samsung MFG=0xEC DEV=0xAA device (x8, 1.65~1.95V, 2K page)
// NAND_DEVn_CFG0 and 1 registers. These parameters are are used for page r/w
#define CONFIG_QC_NAND_NAND_DEVn_CFG0_VAL    0xAAD400C0
#define CONFIG_QC_NAND_NAND_DEVn_CFG1_VAL    0x0004745C

// NAND_DEVn_CFG0 and 1 registers. These parameters are are used for READ ID command
#define CONFIG_QC_NAND_NAND_DEVn_CFG0_RD_ID_VAL    0xA2D40000;
#define CONFIG_QC_NAND_NAND_DEVn_CFG1_RD_ID_VAL    0x0005019C;

//Decide whether to use proc comm to communicate with modem
//This will eventually go away.
#define USE_PROC_COMM

#endif							/* __CONFIG_H */
