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

#ifndef __configs_chromeos_tegra2_harmony_recovery_h__
#define __configs_chromeos_tegra2_harmony_recovery_h__

#include <configs/chromeos/tegra2/recovery.h>
#include <configs/chromeos/tegra2/harmony/common.h>

#define CONFIG_SERIAL_MULTI		1
#define CONFIG_TEGRA2_ENABLE_UARTD	1
#define CONFIG_TEGRA2_ENALBE_UARTA	0

#define CONFIG_CONSOLE_MUX		1
#define CONFIG_SYS_CONSOLE_IS_IN_ENV	1

/* UARTD: keyboard satellite board uart, default */
#define CONFIG_SYS_NS16550_COM1		NV_ADDRESS_MAP_APB_UARTD_BASE

/* UARTA: debug board uart */
#define CONFIG_SYS_NS16550_COM2		NV_ADDRESS_MAP_APB_UARTA_BASE

#ifdef CONFIG_TEGRA2_LP0
#define CONFIG_EXTRA_ENV_SETTINGS	\
	CONFIG_TEGRA_ENV_SETTINGS	\
	"platform_extras=mem=384M@0M nvmem=128M@384M mem=512M@512M "\
	"lp0_vec=" QUOTE(TEGRA_LP0_SIZE) "@" QUOTE(TEGRA_LP0_DEFAULT_ADDR) "\0" \
	"stdin=serial\0" \
	"stdout=serial,lcd\0" \
	"stderr=serial,lcd\0"
#else
#define CONFIG_EXTRA_ENV_SETTINGS	\
	CONFIG_TEGRA_ENV_SETTINGS	\
	"platform_extras=mem=384M@0M nvmem=128M@384M mem=512M@512M\0" \
	"stdin=serial\0" \
	"stdout=serial,lcd\0" \
	"stderr=serial,lcd\0"
#endif

#define CONFIG_SYS_MEMTEST_START       0x0000
#define CONFIG_SYS_MEMTEST_END         0x1000

/* These config switches are for I2C support */
#define CONFIG_I2CP_PIN_MUX		1
#define CONFIG_I2C1_PIN_MUX		1
#define CONFIG_I2C2_PIN_MUX		1
#define CONFIG_I2C3_PIN_MUX		1

/* These config switches are for SD/MMC controller support */
#define MMC_DEV_INSTANCES 2
#define NvEmmcx_0       NvEmmc4
#define NvEmmcx_1       NvEmmc2
#define NvEmmcx_2       0
#define NvEmmcx_3       0

/* To set base address of USB controller */
#define USB_CONTROLLER_INSTANCES 2
#define NvUSBx_0	USB_EHCI_TEGRA_BASE_ADDR_USB3
#define NvUSBx_1	USB_EHCI_TEGRA_BASE_ADDR_USB1
#define NvUSBx_2	0
#define NvUSBx_3	0

/* LCD Settings */
#ifdef CONFIG_LCD
#define LCD_vl_col	1024
#define LCD_vl_row	600
#endif

#endif /*__configs_chromeos_tegra2_harmony_recovery_h__*/
