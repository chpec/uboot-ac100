/*
 * Copyright (c) 2011 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef __configs_chromeos_tegra2_flasher_h__
#define __configs_chromeos_tegra2_flasher_h__

#include <configs/chromeos/flasher.h>
#include <configs/chromeos/tegra2/common.h>

#define SCRIPT_ADDRESS   0xe48000 // TEXT_BASE + 0x40000

#define CONFIG_EXTRA_ENV_SETTINGS_ARCH  CONFIG_EXTRA_ENV_SETTINGS_ARCH_COMMON
#define CONFIG_BOOTCOMMAND              "source " QUOTE(SCRIPT_ADDRESS)

#endif //__configs_chromeos_tegra2_flasher_h__
