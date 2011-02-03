/*
 * Copyright (c) 2011 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef __configs_chromeos_developer_h__
#define __configs_chromeos_developer_h__

#include <configs/chromeos/common.h>

/*
 * Recovery, normal and stub configs define CONFIG_ENV_IS_NOWHERE, but this
 * developer config requires a saved environment. The developer header file
 * in include/config/chromeos/<arch>/developer.h should define where the
 * environment is, for example CONFIG_ENV_IS_IN_SPI_FLASH for SPI flash.
 * If a saved environment is not available for a particular architecture or
 * board, you can define CONFIG_ENV_IS_NOWHERE in that header file instead.
 */
#define CONFIG_CMD_SAVEENV

#endif //__configs_chromeos_developer_h__
