/*
 * Copyright (c) 2011 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef __configs_chromeos_tegra2_developer_h__
#define __configs_chromeos_tegra2_developer_h__

#include <configs/chromeos/developer.h>
#include <configs/chromeos/tegra2/common.h>

#include <configs/chromeos/tegra2/parts/usb.h>

/*
 * We expect all Tegra2 machines to have SPI flash as defined by the
 * firmware_layout.h config file. The environment will go there.
 */
#define CONFIG_ENV_IS_IN_SPI_FLASH 1

#define CONFIG_ENV_SECT_SIZE    CONFIG_LENGTH_ENV
#define CONFIG_ENV_OFFSET       CONFIG_OFFSET_ENV

#define CONFIG_SYS_MEMTEST_START       0x0000
#define CONFIG_SYS_MEMTEST_END         0x1000

#endif //__configs_chromeos_tegra2_developer_h__
