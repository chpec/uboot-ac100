/*
 * Copyright (c) 2011 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef __configs_chromeos_tegra2_seaboard_common_h__
#define __configs_chromeos_tegra2_seaboard_common_h__

#include <configs/chromeos/tegra2/common.h>

#define LINUX_MACH_TYPE			MACH_TYPE_SEABOARD
#define CONFIG_SYS_BOARD_ODMDATA	0x300d8011  /* lp1, 1GB */

#define CONFIG_SERIAL_MULTI		1
#define CONFIG_TEGRA2_ENABLE_UARTD	1
#define CONFIG_TEGRA2_KEYBOARD		1
#define CONFIG_KEYBOARD			1

#define CONFIG_CONSOLE_MUX		1
#define CONFIG_SYS_CONSOLE_IS_IN_ENV	1

#define CONFIG_EXTRA_ENV_SETTINGS_BOARD \
	"stdin=serial,tegra-kbc\0" \
	"stdout=serial,lcd\0" \
	"stderr=serial,lcd\0"

#define CONFIG_PLATFORM_EXTRAS_BOARD \
	"mem=384M@0M nvmem=128M@384M mem=512M@512M"

#include <configs/chromeos/tegra2/seaboard/parts/i2c.h>
#include <configs/chromeos/tegra2/seaboard/parts/keyboard.h>
#include <configs/chromeos/tegra2/seaboard/parts/lcd.h>
#include <configs/chromeos/tegra2/seaboard/parts/mmc.h>
#include <configs/chromeos/tegra2/seaboard/parts/pinmux.h>
#include <configs/chromeos/tegra2/seaboard/parts/spi.h>
#include <configs/chromeos/tegra2/seaboard/parts/uart.h>
#include <configs/chromeos/tegra2/seaboard/parts/tpm.h>

#define CONFIG_CHROMEOS_HWID	"ARM SEABOARD TEST 1176"

#endif /*__configs_chromeos_tegra2_seaboard_common_h__*/
