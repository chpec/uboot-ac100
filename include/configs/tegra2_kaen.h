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
#define V_PROMPT			"Tegra2 (Kaen) # "
#define TEGRA_NFSPORT_DEFAULT		"usb0"

#define CONFIG_SERIAL_MULTI		1
#define CONFIG_TEGRA2_ENABLE_UARTB	1
#define CONFIG_TEGRA2_KEYBOARD		1
#define CONFIG_KEYBOARD			1

#define CONFIG_CONSOLE_MUX		1
#define CONFIG_SYS_CONSOLE_IS_IN_ENV	1
#define CONFIG_STD_DEVICES_SETTINGS	"stdin=serial,tegra-kbc\0" \
					"stdout=serial,lcd\0" \
					"stderr=serial,lcd\0"

/* UARTA: default */
#define CONFIG_SYS_NS16550_COM1		NV_ADDRESS_MAP_APB_UARTB_BASE

/* These config switches are for GPIO support */
#define CONFIG_TEGRA2_GPIO		1
#define CONFIG_CMD_TEGRA2_GPIO_INFO	1

#define LINUX_MACH_TYPE			MACH_TYPE_KAEN
#define CONFIG_SYS_BOARD_ODMDATA	0x300d8011 // lp1, 1GB,

#define CONFIG_I2CP_PIN_MUX		1
#define CONFIG_I2C1_PIN_MUX		1
#define CONFIG_I2C2_PIN_MUX		2
#define CONFIG_I2C3_PIN_MUX		1

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

#undef CONFIG_ENV_IS_NOWHERE
#define CONFIG_ENV_IS_IN_SPI_FLASH

/*
 * These are defined in firmware_layout_config.h for Chromium OS. It might be
 * convenient to use the same area for all Kaen U-Boot builds, but we don't
 * want to pull in this header file.
 */
#define CONFIG_OFFSET_ENV	0x003ff000
#define CONFIG_LENGTH_ENV	0x00001000

#define CONFIG_ENV_SECT_SIZE	CONFIG_LENGTH_ENV
#define CONFIG_ENV_OFFSET	CONFIG_OFFSET_ENV

/* Define this if device is always on EMMC4 controller and no Card Detect pin */
#define CONFIG_TEGRA2_EMMC4_ALWAYS_INSERTED	1

#define MMC_DEV_INSTANCES 2
#define NvEmmcx_0	NvEmmc4
#define NvEmmcx_1	NvEmmc3
#define NvEmmcx_2	0
#define NvEmmcx_3	0

/* To set base address of USB controller */
#define NvUSBx_0	USB_EHCI_TEGRA_BASE_ADDR_USB3
#define NvUSBx_1	USB_EHCI_TEGRA_BASE_ADDR_USB1
#define NvUSBx_2	0
#define NvUSBx_3	0

/* USB1 takes 3 ms to clear Port Reset bit after issuing a Port Reset.
 * The expected time is 2 ms. Please refer to page 28 of EHCI 1.0 specification.
 */
#define CONFIG_USB_RESET_CLEARED_MS	3 * 1000

/* LCD Settings */
#ifdef CONFIG_LCD
#define CONFIG_LCD_vl_col	1366
#define CONFIG_LCD_vl_row	768
#endif

/* Keyboard scan matrix configuration */
#define CONFIG_TEGRA2_KBC_PLAIN_KEYCODES                        \
        0,    0,    0,    0,    0,    0,    0,    0,            \
        0, 0x1b, 0x09,  '`',  'a',  'z',  '1',  'q',            \
        0,    0,    0,    0,  'd',  'c',  '3',  'e',            \
        0,    0,    0,    0,    0,    0,    0,    0,            \
      'b',  'g',  't',  '5',  'f',  'v',  '4',  'r',            \
        0,    0,    0,    0,  's',  'x',  '2',  'w',            \
        0,    0,  ']',    0,  'k',  ',',  '8',  'i',            \
        0,    0,    0,    0,    0,    0,    0,    0,            \
      'n',  'h',  'y',  '6',  'j',  'm',  '7',  'u',            \
        0,    0,    0,    0,    0, KEY_SHIFT, 0, KEY_SHIFT,     \
      '=', '\'',  '[',  '-',  ';',  '/',  '0',  'p',            \
        0,    0,    0,    0,  'l',  '.',  '9',  'o',            \
        0,    0,    0,    0,    0,    0,    0,    0,            \
        0,    0,    0,    0,    0,    0,    0,    0,            \
        0, 0x08,    0, '\\', '\r',  ' ',    0,    0,            \
        0,    0,    0,    0,    0,    0,    0,    0,

#define CONFIG_TEGRA2_KBC_SHIFT_KEYCODES                        \
        0,    0,    0,    0,    0,    0,    0,    0,            \
        0, 0x1b, 0x09,  '~',  'A',  'Z',  '!',  'Q',            \
        0,    0,    0,    0,  'D',  'C',  '#',  'E',            \
        0,    0,    0,    0,    0,    0,    0,    0,            \
      'B',  'G',  'T',  '%',  'F',  'V',  '$',  'R',            \
        0,    0,    0,    0,  'S',  'X',  '@',  'W',            \
        0,    0,  '}',    0,  'K',  '<',  '*',  'I',            \
        0,    0,    0,    0,    0,    0,    0,    0,            \
      'N',  'H',  'Y',  '^',  'J',  'M',  '&',  'U',            \
        0,    0,    0,    0,    0, KEY_SHIFT, 0, KEY_SHIFT,     \
      '+',  '"',  '{',  '_',  ':',  '?',  ')',  'P',            \
        0,    0,    0,    0,  'L',  '>',  '(',  'O',            \
        0,    0,    0,    0,    0,    0,    0,    0,            \
        0,    0,    0,    0,    0,    0,    0,    0,            \
        0, 0x08,    0,  '|', '\r',  ' ',    0,    0,            \
        0,    0,    0,    0,    0,    0,    0,    0,

#define CONFIG_TEGRA2_KBC_FUNCTION_KEYCODES                     \
        0,    0,    0,    0,    0,    0,    0,    0,            \
        0,    0,    0,    0,    0,    0,    0,    0,            \
        0,    0,    0,    0,    0,    0,    0,    0,            \
        0,    0,    0,    0,    0,    0,    0,    0,            \
        0,    0,    0,    0,    0,    0,    0,    0,            \
        0,    0,    0,    0,    0,    0,    0,    0,            \
        0,    0,    0,    0,    0,    0,    0,    0,            \
        0,    0,    0,    0,    0,    0,    0,    0,            \
        0,    0,    0,    0,    0,    0,    0,    0,            \
        0,    0,    0,    0,    0,    0,    0,    0,            \
        0,    0,    0,    0,    0,    0,    0,    0,            \
        0,    0,    0,    0,    0,    0,    0,    0,            \
        0,    0,    0,    0,    0,    0,    0,    0,            \
        0,    0,    0,    0,    0,    0,    0,    0,            \
        0,    0,    0,    0,    0,    0,    0,    0,            \
        0,    0,    0,    0,    0,    0,    0,    0,

#endif /* __CONFIG_H */
