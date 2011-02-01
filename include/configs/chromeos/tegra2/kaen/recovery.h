/*
 * Copyright (c) 2011 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef __configs_chromeos_tegra2_kaen_recovery_h__
#define __configs_chromeos_tegra2_kaen_recovery_h__

#include <configs/chromeos/tegra2/recovery.h>
#include <configs/chromeos/tegra2/kaen/common.h>

#define CONFIG_BOOTCOMMAND \
	"run usb0_boot ; " \
	"run usb1_boot ; " \
	"run mmc1_boot ; " \
	"run mmc0_boot"

#define CONFIG_SYS_MEMTEST_START       0x0000
#define CONFIG_SYS_MEMTEST_END         0x1000

#include <configs/chromeos/tegra2/kaen/parts/usb.h>

#endif /*__configs_chromeos_tegra2_kaen_recovery_h__*/
