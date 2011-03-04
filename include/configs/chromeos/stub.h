/*
 * Copyright (c) 2011 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef __configs_chromeos_stub_h__
#define __configs_chromeos_stub_h__

#include <configs/chromeos/common.h>

#define CONFIG_ENV_IS_NOWHERE

#define CONFIG_CMD_CROS_BOOTSTUB

#define CONFIG_BOOTCOMMAND "cros_bootstub"

#endif //__configs_chromeos_stub_h__
