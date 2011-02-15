/*
 * Copyright (c) 2011 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef __configs_chromeos_flasher_h__
#define __configs_chromeos_flasher_h__

#include <configs/chromeos/common.h>

#define CONFIG_CMD_BDI
#define CONFIG_CMD_CONSOLE
#define CONFIG_CMD_ECHO
#define CONFIG_CMD_MEMORY

#define CONFIG_SYS_MEMTEST_START       0x0000
#define CONFIG_SYS_MEMTEST_END         0x1000

#define CONFIG_ENV_IS_NOWHERE

#endif //__configs_chromeos_flasher_h__
