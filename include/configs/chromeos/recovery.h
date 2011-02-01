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
#define CONFIG_CMD_MEMORY
#define CONFIG_CMD_READ
#define CONFIG_CMD_MISC
#define CONFIG_CMD_SAVEENV
#define CONFIG_CMD_SETGETDCR
#define CONFIG_CMD_XIMG

#define CONFIG_MMC
#define CONFIG_EFI_PARTITION
#define CONFIG_DOS_PARTITION

#define CONFIG_CMD_READ
#define CONFIG_CMD_MMC
#define CONFIG_CMD_EXT2
#define CONFIG_CMD_FAT

/*
 * The following make using the console nicer.
 */
#define CONFIG_DISPLAY_CPUINFO          1
#define CONFIG_DISPLAY_BOARDINFO        1

#define CONFIG_SYS_LONGHELP             1
#define CONFIG_CMDLINE_EDITING          1
#define CONFIG_COMMAND_HISTORY          1
#define CONFIG_AUTOCOMPLETE             1

#endif //__configs_chromeos_recovery_h__
