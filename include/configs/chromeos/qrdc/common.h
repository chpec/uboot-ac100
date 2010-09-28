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
 * Configuation settings for the QRDC board, based on the Qualcomm
 * MSM8x60 surf board.
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

#ifndef __CONFIGS_CHROMEOS_QRDC_COMMON_H
#define __CONFIGS_CHROMEOS_QRDC_COMMON_H

#define CPU_IS_MSM8x60

#include <asm/arch/MSM8x60_reg.h>
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

#define CONFIG_STACK_BASE


/*
 * Undefine Machine ID if defined in common board files. 
*  Redefine the Machine ID for QRDC Board
 */
#ifdef LINUX_MACH_TYPE
#undef LINUX_MACH_TYPE
#endif /* LINUX_MACH_TYPE */

#define LINUX_MACH_TYPE	(3060)

#define CONFIG_SYS_HZ	(32768)  /* GPT Timer frequency */
#define CONFIG_TCXO_HZ	19200000 /* TCX0 frequency */

#undef  CONFIG_SHOW_BOOT_PROGRESS

#define CONFIG_CMDLINE_TAG	   	  /* enable passing of ATAGs  */
#define CONFIG_SETUP_MEMORY_TAGS
//#define CONFIG_INITRD_TAG

/*
 * Enable to pass framebuffer info to kernel
 * but after implementing calc_fbsize() in lcdc.c
 */
#define CONFIG_VIDEOFLB_ATAG_NOT_SUPPORTED

#define ATAG_CORE_FLAGS 	0x00000004
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

#define CONFIG_BOOTDELAY	0

#define CONFIG_BOOTARGS "console=tty0 root=/dev/mmcblk1p3 rootwait init=/sbin/init pmem_kernel_ebi1_size=32M"

#define CONFIG_BOOTCOMMAND                                              \
        "if mmcinfo 1; then "                                           \
           "if ext2load mmc 1:3 0x40200000 boot/boot_script.uimg; then "\
              "source 0x40200000; "                                     \
           "elif ext2load mmc 1:3 0x40207fc0 boot/vmlinux.uimg; then "  \
                "bootm 0x40207fc0; "                                    \
           "fi; "                                                       \
        "elif mmcinfo 0; then "                                         \
           "if ext2load mmc 0:3 0x40200000 boot/boot_script.uimg; then "\
              "source 0x40200000; "                                     \
           "elif ext2load mmc 0:3 0x40207fc0 boot/vmlinux.uimg; then "  \
              "bootm 0x40207fc0; "                                      \
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
 * SDRAM: Memory available for kernel: 510MB.
 */
#define CONFIG_NR_DRAM_BANKS	1
#define PHYS_SDRAM_1		0x40200000
#define PHYS_SDRAM_1_SIZE	0x1FE00000 /* 510MB */

#define CONFIG_ARM_DCC
#define CONFIG_ARM_DCC_MULTI
#define CONFIG_CPU_V7
#define CONFIG_SERIAL_CONSOLE
//#define CONFIG_DCC_CONSOLE

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
 * U-Boot code, data, stack, etc. reside in external RAM.
 */
#define UBOOT_SDRAM_BASE         0x40100000      /* external ram */
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

/* Ethernet support */
#define CONFIG_CMD_NET
#define CONFIG_NET_MULTI
#define CONFIG_CMD_DHCP
#define CONFIG_CMD_PING

#define CONFIG_SMC911X
#define CONFIG_SMC911X_16_BIT
#define CONFIG_SMC911X_BASE 0x1B800000

#endif /* __CONFIGS_CHROMEOS_QRDC_COMMON_H */
