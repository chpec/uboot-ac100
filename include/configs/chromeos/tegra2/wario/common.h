/*
 * Copyright (c) 2011 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef __configs_chromeos_tegra2_wario_common_h__
#define __configs_chromeos_tegra2_wario_common_h__

#include <configs/chromeos/tegra2/common.h>

#define LINUX_MACH_TYPE			MACH_TYPE_WARIO
#define CONFIG_SYS_BOARD_ODMDATA	0x300d8011  /* lp1, 1GB */

#include <configs/chromeos/tegra2/wario/parts/i2c.h>
#include <configs/chromeos/tegra2/wario/parts/spi.h>
#include <configs/chromeos/tegra2/wario/parts/mmc.h>

#endif /*__configs_chromeos_tegra2_wario_common_h__*/
