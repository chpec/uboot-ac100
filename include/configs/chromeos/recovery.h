/*
 * Copyright (c) 2011 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef __configs_chromeos_recovery_h__
#define __configs_chromeos_recovery_h__

#include <configs/chromeos/common.h>

#define CONFIG_CMD_AUTOSCRIPT
#define CONFIG_CMD_BDI
#define CONFIG_CMD_BOOTD
#define CONFIG_CMD_CONSOLE
#define CONFIG_CMD_ECHO
#define CONFIG_CMD_FPGA
#define CONFIG_CMD_IMI
#define CONFIG_CMD_ITEST
#define CONFIG_CMD_LOADB
#define CONFIG_CMD_LOADS
#define CONFIG_CMD_READ
#define CONFIG_CMD_MISC
#define CONFIG_CMD_SETGETDCR
#define CONFIG_CMD_XIMG

#define CONFIG_MMC
#define CONFIG_EFI_PARTITION
#define CONFIG_DOS_PARTITION

#define CONFIG_CHROMEOS_BMPBLK
#define CONFIG_CMD_BMP
#define CONFIG_LCD_BMP_RLE8
#define CONFIG_LZMA

#define CONFIG_CMD_READ
#define CONFIG_CMD_MMC
#define CONFIG_CMD_EXT2
#define CONFIG_CMD_FAT

#define CONFIG_ENV_IS_NOWHERE

#endif //__configs_chromeos_recovery_h__
