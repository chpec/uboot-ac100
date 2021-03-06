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

#ifndef __CONFIG_H
#define __CONFIG_H
#include <asm/sizes.h>
#include "tegra2-common.h"

/*
 * High Level Configuration Options
 */
#define TEGRA2_SYSMEM			"mem=384M@0M nvmem=128M@384M mem=512M@512M"
#define V_PROMPT			"Tegra2 (harmony) # "
#define TEGRA_NFSPORT_DEFAULT		"usb0"

#define CONFIG_SERIAL_MULTI		1
#define CONFIG_TEGRA2_ENABLE_UARTD	1
#define CONFIG_TEGRA2_ENABLE_UARTA	0
#define CONFIG_STD_DEVICES_SETTINGS

/* UARTD: keyboard satellite board uart, default */
#define CONFIG_SYS_NS16550_COM1		NV_ADDRESS_MAP_APB_UARTD_BASE

/* UARTA: debug board uart */
#define CONFIG_SYS_NS16550_COM2		NV_ADDRESS_MAP_APB_UARTA_BASE

/* These config switches are for GPIO support */
#define CONFIG_TEGRA2_GPIO		1
#define CONFIG_CMD_TEGRA2_GPIO_INFO	1

#define LINUX_MACH_TYPE			MACH_TYPE_HARMONY
#define CONFIG_SYS_BOARD_ODMDATA	0x300d8011

#define CONFIG_I2CP_PIN_MUX		1
#define CONFIG_I2C1_PIN_MUX		1
#define CONFIG_I2C2_PIN_MUX		1
#define CONFIG_I2C3_PIN_MUX		1

#define MMC_DEV_INSTANCES 2
#define NvEmmcx_0	NvEmmc4
#define NvEmmcx_1	NvEmmc2
#define NvEmmcx_2	0
#define NvEmmcx_3	0

/* To set base address of USB controller */
#define NvUSBx_0	USB_EHCI_TEGRA_BASE_ADDR_USB3
#define NvUSBx_1	USB_EHCI_TEGRA_BASE_ADDR_USB1
#define NvUSBx_2	0
#define NvUSBx_3	0

/* LCD Settings */
#ifdef CONFIG_LCD
#define CONFIG_LCD_vl_col	1024
#define CONFIG_LCD_vl_row	600
#endif

#define CONFIG_TEGRA2_PINMUX_DISPLAY_HARMONY			\
        PINMUX(LCSN,  DISPLAYA,      PULL_UP,   TRISTATE)       \
        PINMUX(LD0,   DISPLAYA,      PULL_DOWN, NORMAL)         \
        PINMUX(LD1,   DISPLAYA,      PULL_DOWN, NORMAL)         \
        PINMUX(LD10,  DISPLAYA,      PULL_DOWN, NORMAL)         \
        PINMUX(LD11,  DISPLAYA,      PULL_DOWN, NORMAL)         \
        PINMUX(LD12,  DISPLAYA,      PULL_DOWN, NORMAL)         \
        PINMUX(LD13,  DISPLAYA,      PULL_DOWN, NORMAL)         \
        PINMUX(LD14,  DISPLAYA,      PULL_DOWN, NORMAL)         \
        PINMUX(LD15,  DISPLAYA,      PULL_DOWN, NORMAL)         \
        PINMUX(LD16,  DISPLAYA,      PULL_DOWN, NORMAL)         \
        PINMUX(LD17,  DISPLAYA,      PULL_DOWN, NORMAL)         \
        PINMUX(LD2,   DISPLAYA,      PULL_DOWN, NORMAL)         \
        PINMUX(LD3,   DISPLAYA,      PULL_DOWN, NORMAL)         \
        PINMUX(LD4,   DISPLAYA,      PULL_DOWN, NORMAL)         \
        PINMUX(LD5,   DISPLAYA,      PULL_DOWN, NORMAL)         \
        PINMUX(LD6,   DISPLAYA,      PULL_DOWN, NORMAL)         \
        PINMUX(LD7,   DISPLAYA,      PULL_DOWN, NORMAL)         \
        PINMUX(LD8,   DISPLAYA,      PULL_DOWN, NORMAL)         \
        PINMUX(LD9,   DISPLAYA,      PULL_DOWN, NORMAL)         \
        PINMUX(LDI,   DISPLAYA,      PULL_DOWN, NORMAL)         \
        PINMUX(LHP0,  DISPLAYA,      PULL_DOWN, NORMAL)         \
        PINMUX(LHP1,  DISPLAYA,      PULL_DOWN, NORMAL)         \
        PINMUX(LHP2,  DISPLAYA,      PULL_DOWN, NORMAL)         \
        PINMUX(LHS,   DISPLAYA,      PULL_UP,   NORMAL)         \
        PINMUX(LM0,   DISPLAYA,      PULL_UP,   NORMAL)         \
        PINMUX(LM1,   DISPLAYA,      PULL_UP,   TRISTATE)       \
        PINMUX(LPP,   DISPLAYA,      PULL_DOWN, NORMAL)         \
        PINMUX(LPW0,  DISPLAYA,      PULL_UP,   NORMAL)         \
        PINMUX(LPW1,  DISPLAYA,      PULL_UP,   TRISTATE)       \
        PINMUX(LPW2,  DISPLAYA,      PULL_UP,   NORMAL)         \
        PINMUX(LSC0,  DISPLAYA,      PULL_UP,   NORMAL)         \
        PINMUX(LSC1,  DISPLAYA,      PULL_UP,   TRISTATE)       \
        PINMUX(LSCK,  DISPLAYA,      PULL_UP,   TRISTATE)       \
        PINMUX(LSDA,  DISPLAYA,      PULL_UP,   TRISTATE)       \
        PINMUX(LSDI,  DISPLAYA,      PULL_UP,   TRISTATE)       \
        PINMUX(LSPI,  DISPLAYA,      PULL_UP,   NORMAL)         \
        PINMUX(LVP0,  DISPLAYA,      PULL_UP,   TRISTATE)       \
        PINMUX(LVP1,  DISPLAYA,      PULL_DOWN, NORMAL)         \
        PINMUX(LVS,   DISPLAYA,      PULL_UP,   NORMAL)         \
        PINMUX(SDC,   PWM,           PULL_UP,   NORMAL)

#define CONFIG_TEGRA2_PINMUX                    \
        CONFIG_TEGRA2_PINMUX_DISPLAY_HARMONY

#endif /* __CONFIG_H */
