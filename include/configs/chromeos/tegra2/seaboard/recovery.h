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

#include <configs/chromeos/tegra2/recovery.h>
#include <configs/chromeos/tegra2/seaboard/common.h>

#define CONFIG_SERIAL_MULTI		1
#define CONFIG_TEGRA2_ENABLE_UARTD	1
#define CONFIG_TEGRA2_KEYBOARD		1
#define CONFIG_KEYBOARD			1

#define CONFIG_CONSOLE_MUX		1
#define CONFIG_SYS_CONSOLE_IS_IN_ENV	1

#include <configs/chromeos/tegra2/seaboard/parts/uart.h>

#ifdef CONFIG_TEGRA2_LP0
#define CONFIG_EXTRA_ENV_SETTINGS	\
	CONFIG_TEGRA_ENV_SETTINGS	\
	"platform_extras=mem=384M@0M nvmem=128M@384M mem=512M@512M "\
	"lp0_vec=" QUOTE(TEGRA_LP0_SIZE) "@" QUOTE(TEGRA_LP0_DEFAULT_ADDR) "\0" \
	"stdin=serial,tegra-kbc\0" \
	"stdout=serial,lcd\0" \
	"stderr=serial,lcd\0"
#else
#define CONFIG_EXTRA_ENV_SETTINGS	\
	CONFIG_TEGRA_ENV_SETTINGS	\
	"platform_extras=mem=384M@0M nvmem=128M@384M mem=512M@512M\0" \
	"stdin=serial,tegra-kbc\0" \
	"stdout=serial,lcd\0" \
	"stderr=serial,lcd\0"
#endif

#define CONFIG_BOOTCOMMAND		\
	"run usb0boot ; " \
	"run usb1boot ; " \
	"run mmc1boot ; " \
	"run mmc0boot"

#define CONFIG_SYS_MEMTEST_START       0x0000
#define CONFIG_SYS_MEMTEST_END         0x1000

#include <configs/chromeos/tegra2/seaboard/parts/usb.h>
#include <configs/chromeos/tegra2/seaboard/parts/lcd.h>

#endif //__configs_chromeos_tegra2_seaboard_recovery_h__
