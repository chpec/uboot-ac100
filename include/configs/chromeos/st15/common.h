/*
 * Copyright (c) 2009-2010, Code Aurora Forum. All rights reserved.
 *
 * (C) Copyright 2002-2005
 * Wolfgang Denk, DENX Software Engineering, <wd@denx.de>
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
 * Gary Jennejohn <gj@denx.de>
 *
 * Configuation settings for the st1q board, based on the Qualcomm
 * QSD8x50 surf board.
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

#ifndef __CONFIGS_CHROMEOS_ST15_COMMON_H
#define __CONFIGS_CHROMEOS_ST15_COMMON_H

#define CPU_IS_QSD8x50A

#include <asm/arch/QSD8x50A_reg.h>
#include <asm-armv7Scorpion/armv7Scorpion.h>
#include <config.h>

#define CONFIG_CHROMEOS_HWID	"ARM ST15 TEST 7712"

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

#define CONFIG_STACK_BASE

#define CONFIG_ARCH_CPU_INIT

/*
 * Machine ID for ST1.5
 */
#define LINUX_MACH_TYPE	2627

#define CONFIG_SYS_HZ	(32768)  /* GPT Timer frequency */
#define CONFIG_TCXO_HZ	19200000 /* TCX0 frequency */

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

/*
 * Serial port Configuration
 */
#undef  CONFIG_SILENT_CONSOLE
#define CFG_QC_SERIAL
#define CONFIG_CONS_INDEX		0
#define CONFIG_BAUDRATE			115200
#define CONFIG_SYS_BAUDRATE_TABLE	{ 9600, 19200, 38400, 57600, 115200 }

#define CONFIG_BOOTP_MASK		CONFIG_BOOTP_DEFAULT

#define CONFIG_CMD_SOURCE

#define CONFIG_BOOTDELAY		0

#define CONFIG_BOOTARGS			"quiet root=/dev/mmcblk0p3 rootwait noresume noswap ro loglevel=1 pmem_kernel_ebi1_size=96M pmem_adsp_size=34200K"
#define CONFIG_BOOTCOMMAND                                              \
        "if mmcinfo 0; then "                                           \
           "if ext2load mmc 0:3 0x00000000 boot/boot_script.uimg; then "\
              "source 0x00000000; "                                     \
           "elif ext2load mmc 0:3 0x00007fc0 boot/vmlinux.uimg; then "  \
                "bootm 0x00007fc0; "                                    \
           "fi; "                                                       \
        "elif mmcinfo 1; then "                                         \
           "if ext2load mmc 1:3 0x00000000 boot/boot_script.uimg; then "\
              "source 0x00000000; "                                     \
           "elif ext2load mmc 1:3 0x00007fc0 boot/vmlinux.uimg; then "  \
              "bootm 0x00007fc0; "                                      \
           "fi; "                                                       \
        "fi;"

/*
 * Miscellaneous configurable options
 */
#define CONFIG_SYS_NO_FLASH
#define CONFIG_SYS_LONGHELP
#define CONFIG_SYS_HUSH_PARSER
#define CONFIG_SYS_PROMPT_HUSH_PS2      "> "
#define CONFIG_SYS_PROMPT	"ChromeOS> "
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

/*
 * SDRAM is split into 2 regions to accommodate ADSP as a memory hole.
 * Memory available for kernel: 224 + 768 = 992MB.
 */
#define CONFIG_NR_DRAM_BANKS	2
#define PHYS_SDRAM_1		0x00000000	    /* EBI1 start */
#define PHYS_SDRAM_1_SIZE	0x0E000000	    /* 256 - 32(adsp) = 224MB (0x0E000000)*/
#define PHYS_SDRAM_2		0x10000000	    /* EBI1, AFTER ADSP */
#define PHYS_SDRAM_2_SIZE	0x30000000	    /* 768MB */

#define CONFIG_ARM_DCC
#define CONFIG_ARM_DCC_MULTI
#define CONFIG_CPU_V7

/* Console setup */
#if defined(CONFIG_SERIAL_CONSOLE)
 #define CONFIG_STDOUT "serial"
 #define CONFIG_STDERR "serial"
 #define CONFIG_STDIN  "serial"
#elif defined(CONFIG_DCC_CONSOLE)
 #define CONFIG_STDOUT "dcc"
 #define CONFIG_STDERR "dcc"
 #define CONFIG_STDIN  "dcc"
#else /* default */
 #define CONFIG_STDOUT "lcd"
 #define CONFIG_STDERR "lcd"
 #define CONFIG_STDIN  "dcc"
#endif

#define CONFIG_SYS_CONSOLE_IS_IN_ENV
#define CONFIG_SYS_CONSOLE_OVERWRITE_ROUTINE

/*-----------------------------------------------------------------------
 * Shared Memory Location -
 */
#define SMEM_START  0xE0100000
#define SMEM_SIZE   0x00100000

/*-----------------------------------------------------------------------
 * Physical Memory Map -
 * U-Boot code, data, stack, etc. reside in SMI SDRAM 0x00000000-0x000FFFFF.
 * There are similar parameters in the u-boot.lds linker script which also
 * need to be updated.
 */
#define UBOOT_SDRAM_BASE         0xE0000000      /* SMI */
#define UBOOT_SDRAM_SIZE         0x00100000

/* Memory Test */
#define CONFIG_SYS_MEMTEST_START       PHYS_SDRAM_1
#define CONFIG_SYS_MEMTEST_END         (PHYS_SDRAM_1 + PHYS_SDRAM_1_SIZE)

/* Environment */
#define CONFIG_ENV_IS_NOWHERE
#define CONFIG_ENV_SIZE         0x2000

/* Boot parameter address - offset of 0x100 from base of first sdram region */
#define CFG_QC_BOOT_PARAM_ADDR    (PHYS_SDRAM_1 + 0x100)

/*-----------------------------------------------------------------------
 * The qc_serial driver uses the register names below. Set UART_BASE
 * for the desired UART.
 */
#define UART_BASE     UART3_BASE

/* MMC interface */
#define CONFIG_GENERIC_MMC
#define CONFIG_GENERIC_MMC_MULTI_BLOCK_READ
#define CONFIG_QSD_SDCC


#ifndef CONFIG_GENERIC_MMC
/* This section regarding legacy mmc will be removed once the new
 * mmc framework has been verified/tested sufficiently. If there are
 * any major issues, you could go back to the legacy mmc by undefining
 * the generic mmc code.
 */

/*-----------------------------------------------------------------------
 * Choose the SD controller to use. SDC1, 2, 3, or 4.
 */
#define SDC_INSTANCE  1
#define USE_DM
#define USE_HIGH_SPEED_MODE
#define USE_4_BIT_BUS_MODE
#define CONFIG_SYS_MMC_BASE             0xF0000000    // not used, but defined to prevent compile error
#define PROC_COMM_VREG_SDC  PM_VREG_WLAN_ID
#endif

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
#define PROC_COMM_MPP_FOR_USB_VBUS PM_MPP_16
#undef  USE_PROC_COMM_USB_PHY_RESET /* proc_comm cmd to reset phy not working
                                       rt now, but eventually it will*/

#endif /* __CONFIGS_CHROMEOS_ST15_COMMON_H */
