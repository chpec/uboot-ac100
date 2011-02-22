/*
 * Copyright (c) 2011 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef __configs_chromeos_tegra2_seaboard_recovery_h__
#define __configs_chromeos_tegra2_seaboard_recovery_h__

/*
 * Order of headers (tegra2/seaboard/common.h before
 * tegra2/{developer,normal,recovery,stub}.h) must be preserved so that
 * seaboard/firmware_layout.h can override top-level firmware layout header.
 */
#include <configs/chromeos/tegra2/seaboard/common.h>
#include <configs/chromeos/tegra2/recovery.h>

#define CONFIG_BOOTCOMMAND \
	"run usb0_boot ; " \
	"run usb1_boot ; " \
	"run mmc1_boot ; " \
	"run mmc0_boot"

#include <configs/chromeos/tegra2/seaboard/parts/usb.h>

#endif /*__configs_chromeos_tegra2_seaboard_recovery_h__*/
