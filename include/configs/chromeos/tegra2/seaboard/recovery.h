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

#ifndef __configs_chromeos_tegra2_seaboard_recovery_h__
#define __configs_chromeos_tegra2_seaboard_recovery_h__

#include <configs/chromeos/recovery.h>
#include <configs/chromeos/tegra2/common.h>

#define CONFIG_SERIAL_MULTI		1
#define CONFIG_TEGRA2_ENABLE_UARTD	1
#define CONFIG_TEGRA2_KEYBOARD		1
#define CONFIG_KEYBOARD			1

#define CONFIG_CONSOLE_MUX		1
#define CONFIG_SYS_CONSOLE_IS_IN_ENV	1

/* UARTD: keyboard satellite board uart, default */
#define CONFIG_SYS_NS16550_COM1		NV_ADDRESS_MAP_APB_UARTD_BASE

/* UARTA: debug board uart */
#define CONFIG_SYS_NS16550_COM2		NV_ADDRESS_MAP_APB_UARTA_BASE

#define CONFIG_EXTRA_ENV_SETTINGS	\
	CONFIG_TEGRA_ENV_SETTINGS	\
	"platform_extras=mem=384M@0M nvmem=128M@384M mem=512M@512M\0" \
	"stdin=serial,tegra-kbc\0" \
	"stdout=serial\0" \
	"stderr=serial\0" \

#define LINUX_MACH_TYPE			MACH_TYPE_TEGRA_SEABOARD
#define CONFIG_SYS_BOARD_ODMDATA	0x300d8011  //lp1, 1GB

#define CONFIG_I2CP_PIN_MUX		1
#define CONFIG_I2C1_PIN_MUX		1
#define CONFIG_I2C2_PIN_MUX		2
#define CONFIG_I2C3_PIN_MUX		1

#define CONFIG_SYS_MEMTEST_START       0x0000
#define CONFIG_SYS_MEMTEST_END         0x1000

/*
 * SPI Settings
 */
#define CONFIG_TEGRA2_SPI
#define CONFIG_SPI_FLASH
#define CONFIG_SPI_FLASH_WINBOND
#define CONFIG_SPI_FLASH_SLOW_READ
#define CONFIG_SF_DEFAULT_MODE	SPI_MODE_0
#define CONFIG_CMD_SPI
#define CONFIG_CMD_SF

/*
 * To put the env in SPI-Flash on Seaboard, define the following,
 *  and undef or delete the CONFIG_ENV_IS_NOWHERE (if present).
 * Note that you MUST change the ENV_OFFSET to be beyond the BCT/BL area!
 */
#if	0
#define CONFIG_ENV_IS_IN_SPI_FLASH
#define CONFIG_ENV_SECT_SIZE	0x1000	/* 4K sectors */
#define CONFIG_ENV_OFFSET	0x0	/* DANGER! for test, use offset != 0 */
#endif

/* 
 * These config switches are for SD/MMC controller support 
 */
/* Define this if device is always on EMMC4 controller and no Card Detect pin */
#define CONFIG_TEGRA2_EMMC4_ALWAYS_INSERTED     1

#define MMC_DEV_INSTANCES 2 
#define NvEmmcx_0       NvEmmc4
#define NvEmmcx_1       NvEmmc3
#define NvEmmcx_2       0
#define NvEmmcx_3       0

#endif //__configs_chromeos_tegra2_seaboard_recovery_h__
