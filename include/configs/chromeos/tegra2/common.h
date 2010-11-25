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

#ifndef __configs_chromeos_tegra2_common_h__
#define __configs_chromeos_tegra2_common_h__

#include <configs/chromeos/common.h>
#include <asm/arch/tegra2.h>

#undef DEBUG
/*
 * High Level Configuration Options
 */
#define CONFIG_ARMCORTEXA9		1
#define CONFIG_TEGRA2			1
#define CONFIG_MACH_TEGRA_GENERIC	1
#define CONFIG_SKIP_RELOCATE_UBOOT
#define CONFIG_L2_OFF			1

#define CONFIG_ENABLE_CORTEXA9		1
#undef  TEGRA2_TRACE

#undef CONFIG_USE_IRQ
#define CONFIG_ARCH_CPU_INIT
#define CONFIG_MISC_INIT_R
#define CONFIG_SKIP_LOWLEVEL_INIT

#define CONFIG_SYS_64BIT_STRTOUL                1
#define CONFIG_SYS_64BIT_VSPRINTF               1

/*
 * PllX Configuration
 */
#define CONFIG_SYS_CPU_OSC_FREQUENCY    1000000        /* Set CPU clock to 1GHz */

/*
 * NS16550 Configuration
 */
#define CONFIG_SYS_NS16550
#define CONFIG_SYS_NS16550_SERIAL
#define CONFIG_SYS_NS16550_REG_SIZE	(-4)
#define CONFIG_SYS_NS16550_CLK		216000000	/* 216MHz (pllp_out0) */

/*
 * select serial console configuration
 */
#define CONFIG_CONS_INDEX		1

#define CONFIG_TEGRA2_MMC		1
#define TEGRA2_MMC_DEFAULT_DEVICE	"0"

#define CONFIG_SYS_NO_FLASH

/*-----------------------------------------------------------------------
 * GPIO Config
 */
#define CONFIG_TEGRA2_GPIO		1
#define CONFIG_CMD_TEGRA2_GPIO_INFO	1

/*-----------------------------------------------------------------------
 * USB Config
 */
#define CONFIG_CMD_USB

#define CONFIG_USB_EHCI
#define CONFIG_USB_EHCI_TEGRA
#define USB_EHCI_TEGRA_BASE_ADDR	0xC5008000	/* USB3 base address */
#define CONFIG_USB_EHCI_DATA_ALIGN	4

/*
 * This parameter affects a TXFILLTUNING field that controls how much data is
 * sent to the latency fifo before it is sent to the wire. Without this
 * parameter, the default (2) causes occasional Data Buffer Errors in OUT
 * packets depending on the buffer address and size.
 */
#define CONFIG_USB_EHCI_TXFIFO_THRESH	10

#define CONFIG_EHCI_IS_TDI

#define CONFIG_USB_STORAGE

/*-----------------------------------------------------------------------
 * Board NAND Info.
 */
#define CONFIG_CMD_NAND

#define CONFIG_NAND_TEGRA2
#define CONFIG_SYS_NAND_ADDR		NAND_BASE
#define CONFIG_SYS_NAND_BASE		NAND_BASE
#define CONFIG_SYS_MAX_NAND_DEVICE	1

/*-------------------------
 *  I2C configuration
 */
#define CONFIG_TEGRA2_I2C

#ifdef CONFIG_TEGRA2_I2C
#define CONFIG_CMD_I2C
#define CONFIG_I2C_MULTI_BUS		1
#define CONFIG_SYS_MAX_I2C_BUS		4
#define CONFIG_SYS_I2C_SPEED		100000
#endif

/* Enable Warmboot code and lp0_vec */
#define CONFIG_TEGRA2_LP0		1

#ifdef CONFIG_TEGRA2_LP0
/* make sure the following 2 defines have the same values
 * (_STR is the string of TEGRA_LP0_DEFAULT_ADDR)
 */
#define TEGRA_LP0_DEFAULT_ADDR		0x1C406000
#define TEGRA_LP0_DEFAULT_ADDR_STR	"0x1C406000"

/* make sure the following 2 defines have the same values
 * (_STR is the string of TEGRA_LP0_SIZE)
 */
#define TEGRA_LP0_SIZE			0x2000
#define TEGRA_LP0_SIZE_STR		"0x2000"
#endif

#define CONFIG_LOADADDR			0x40C000
#define CONFIG_BOOTDELAY		2	/* 2s to break to prompt */
#define CONFIG_TEGRA_ENV_SETTINGS	\
	"scriptaddr=0x408000\0" \
	"script_img=/u-boot/boot.scr.uimg\0" \
	"scriptboot=fatload ${devtype} 0:c ${scriptaddr} ${script_img};" \
		"source ${scriptaddr};" \
		"read ${devtype} 0:${kernelpart} ${scriptaddr} 0 10;" \
		"source ${scriptaddr};\0" \
	"mmcboot=mmc init " TEGRA2_MMC_DEFAULT_DEVICE ";" \
		"setenv devtype mmc;" \
		"setenv devname mmcblk" TEGRA2_MMC_DEFAULT_DEVICE "p;" \
		"run scriptboot;\0" \
	"usbboot=usb start;" \
		"setenv devtype usb;" \
		"setenv devname sda;" \
		"run scriptboot;\0"

#undef CONFIG_BOOTCOMMAND
#define CONFIG_BOOTCOMMAND		"run usbboot ; run mmcboot"

#define CONFIG_SYS_LOAD_ADDR		0xA00800

#define CONFIG_SYS_HZ			1000

/*-----------------------------------------------------------------------
 * Stack sizes
 *
 * The stack sizes are set up in start.S using the settings below
 */
#define CONFIG_STACKBASE	(SZ_32M + SZ_8M)
#define CONFIG_STACKSIZE	SZ_128K

#ifdef CONFIG_USE_IRQ
#define CONFIG_STACKSIZE_IRQ	SZ_4K
#define CONFIG_STACKSIZE_FIQ	SZ_4K
#endif

/*-----------------------------------------------------------------------
 * Physical Memory Map
 */
#define CONFIG_NR_DRAM_BANKS	1
#define PHYS_SDRAM_1		TEGRA2_SDRC_CS0
#define PHYS_SDRAM_1_SIZE	SZ_512M

#endif //__configs_chromeos_tegra2_common_h__
