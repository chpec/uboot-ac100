/*
 * Copyright (c) 2011 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef __configs_chromeos_tegra2_stub_h__
#define __configs_chromeos_tegra2_stub_h__

#include <configs/chromeos/stub.h>
#include <configs/chromeos/tegra2/common.h>

#define CONFIG_EXTRA_ENV_SETTINGS_ARCH CONFIG_EXTRA_ENV_SETTINGS_ARCH_COMMON

/*
 * Boot stub is supposed to be loaded to fixed location, and so does not have
 * to able to relocate itself
 */
#define CONFIG_SKIP_RELOCATE_UBOOT

#endif //__configs_chromeos_tegra2_stub_h__
