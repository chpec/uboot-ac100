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
#define TEGRA2_SYSMEM           "mem=384M@0M nvmem=128M@384M mem=512M@512M"
#define V_PROMPT		"Tegra2 (SeaBoard) # "
#define TEGRA_NFSPORT_DEFAULT	"usb0"

#define CONFIG_SERIAL_MULTI	1
#define CONFIG_TEGRA2_HARMONY_ENABLE_KEYBOARD_UART	1
#define CONFIG_TEGRA2_HARMONY_ENABLE_DEBUG_UART		1

/* UARTD: keyboard satellite board uart, default */
#define CONFIG_SYS_NS16550_COM1		NV_ADDRESS_MAP_APB_UARTD_BASE

/* UARTA: debug board uart */
#define CONFIG_SYS_NS16550_COM2		NV_ADDRESS_MAP_APB_UARTA_BASE

/* These config switches are for GPIO support */
#define CONFIG_TEGRA2_GPIO		1
#define CONFIG_CMD_TEGRA2_GPIO_INFO	1

#define LINUX_MACH_TYPE                 MACH_TYPE_TEGRA_SEABOARD
#define CONFIG_SYS_BOARD_ODMDATA        0x30d08011

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
/*
 * To put the env in SPI-Flash on Seaboard, define the following,
 *  and undef or delete the CONFIG_ENV_IS_NOWHERE (if present).
 * Note that you MUST change the ENV_OFFSET to be beyond the BCT/BL area!
 */
#if	0
#define CONFIG_ENV_IS_IN_SPI_FLASH
#define CONFIG_ENV_SECT_SIZE	0x1000	/* 4K sectors */
#define CONFIG_ENV_OFFSET		0x0		/* DANGER! for test, use offset != 0 */
#endif

#endif /* __CONFIG_H */
