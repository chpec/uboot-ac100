/*
 * Copyright (c) 2011 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef __configs_chromeos_tegra2_wario_recovery_h__
#define __configs_chromeos_tegra2_wario_recovery_h__

#include <configs/chromeos/tegra2/recovery.h>
#include <configs/chromeos/tegra2/wario/common.h>

#define CONFIG_SERIAL_MULTI		1
#define CONFIG_TEGRA2_ENABLE_UARTB	1
#define CONFIG_TEGRA2_KEYBOARD		1
#define CONFIG_KEYBOARD			1

#define CONFIG_CONSOLE_MUX		1
#define CONFIG_SYS_CONSOLE_IS_IN_ENV	1

#include <configs/chromeos/tegra2/wario/parts/uart.h>

#ifdef CONFIG_TEGRA2_LP0
#define CONFIG_EXTRA_ENV_SETTINGS_BOARD	\
	"platform_extras=mem=384M@0M nvmem=128M@384M mem=512M@512M "\
	"lp0_vec="QUOTE(TEGRA_LP0_SIZE)"@"QUOTE(TEGRA_LP0_DEFAULT_ADDR)"\0" \
	"stdin=serial,tegra-kbc\0" \
	"stdout=serial,lcd\0" \
	"stderr=serial,lcd\0"
#else
#define CONFIG_EXTRA_ENV_SETTINGS_BOARD	\
	"platform_extras=mem=384M@0M nvmem=128M@384M mem=512M@512M\0" \
	"stdin=serial,tegra-kbc\0" \
	"stdout=serial,lcd\0" \
	"stderr=serial,lcd\0"
#endif

#define CONFIG_BOOTCOMMAND \
	"run usb1_boot ; " \
	"run mmc1_boot ; " \
	"run mmc0_boot"

#define CONFIG_SYS_MEMTEST_START       0x0000
#define CONFIG_SYS_MEMTEST_END         0x1000

#include <configs/chromeos/tegra2/wario/parts/usb.h>
#include <configs/chromeos/tegra2/wario/parts/lcd.h>

#endif /*__configs_chromeos_tegra2_wario_recovery_h__*/
