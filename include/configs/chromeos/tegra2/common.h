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

#include <configs/chromeos/tegra2/parts/uart.h>
#include <configs/chromeos/tegra2/parts/mmc.h>

#define CONFIG_SYS_NO_FLASH

#include <configs/chromeos/tegra2/parts/gpio.h>
#include <configs/chromeos/tegra2/parts/nand.h>
#include <configs/chromeos/tegra2/parts/i2c.h>

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
	"scriptboot=fatload ${devtype} ${devnum}:c ${scriptaddr} ${script_img};" \
		"source ${scriptaddr};" \
		"read ${devtype} ${devnum}:${kernelpart} ${scriptaddr} 0 10;" \
		"source ${scriptaddr};\0" \
	"mmc0boot=setenv devnum 0;" \
		"run mmcboot;\0" \
	"mmc1boot=setenv devnum 1;" \
		"run mmcboot;\0" \
	"mmcboot=mmc init ${devnum};" \
		"setenv devtype mmc;" \
	        "setenv devname mmcblk${devnum}p;" \
		"run scriptboot;\0" \
	"usb0boot=usb start 0;" \
		"run usbboot;\0" \
	"usb1boot=usb start 1;" \
		"run usbboot;\0" \
	"usbboot=setenv devtype usb;" \
		"setenv devnum 0;" \
		"setenv devname sda;" \
		"run scriptboot;\0"

#undef CONFIG_BOOTCOMMAND
#define CONFIG_BOOTCOMMAND		"run usb0boot ; " \
					"run usb1boot ; " \
					"run mmc1boot ; " \
					"run mmc0boot"

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
