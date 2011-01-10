/*
 * Copyright (c) 2011 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef __configs_chromeos_common_h__
#define __configs_chromeos_common_h__

#include <asm/sizes.h>
#include <config.h>

/*
 * QUOTE(m) will evaluate to a string version of the value of the macro m
 * passed in.  The extra level of indirection here is to first evaluate the
 * macro m before applying the quoting operator.
 */
#define QUOTE_(m) #m
#define QUOTE(m) QUOTE_(m)

#define CONFIG_CHROMEOS
#define CONFIG_CMD_CROS_ROFW

#define CONFIG_CMD_RUN
#define CONFIG_CMD_READ
#define CONFIG_CMD_SOURCE

/* friendly debug environment */
#ifdef VBOOT_DEBUG
#define CONFIG_CMD_CROS
#define CONFIG_SYS_HUSH_PARSER
#define CONFIG_SYS_PROMPT_HUSH_PS2	"> "
#endif

#define CONFIG_OF_LIBFDT		1
/*
 * The early kernel mapping on ARM currently only maps from the base of DRAM
 * to the end of the kernel image.  The kernel is loaded at DRAM base + 0x8000.
 * The early kernel pagetable uses DRAM base + 0x4000 to DRAM base + 0x8000,
 * so that leaves DRAM base to DRAM base + 0x4000 available.
 */
#define CONFIG_SYS_BOOTMAPSZ	        0x4000

#define CONFIG_CMDLINE_TAG		1

#define CONFIG_ENV_IS_NOWHERE		1
#define CONFIG_ENV_SIZE			SZ_128K
#define CONFIG_ENV_OVERWRITE		1

#define CONFIG_SYS_MALLOC_LEN		SZ_1M
#define CONFIG_SYS_GBL_DATA_SIZE	128

#define CONFIG_BAUDRATE			115200
#define CONFIG_SYS_BAUDRATE_TABLE	{4800, \
					 9600, \
					 19200, \
					 38400, \
					 57600, \
					 115200}

#define CONFIG_SYS_PROMPT		"CrOS> "

#define CONFIG_EXTRA_ENV_SETTINGS \
	CONFIG_EXTRA_ENV_SETTINGS_ARCH \
	CONFIG_EXTRA_ENV_SETTINGS_BOARD \
	"platform_extras=" \
	CONFIG_PLATFORM_EXTRAS_ARCH " " \
	CONFIG_PLATFORM_EXTRAS_BOARD "\0"

#define CONFIG_SYS_CBSIZE		512
#define CONFIG_SYS_PBSIZE		(CONFIG_SYS_CBSIZE + \
					 sizeof(CONFIG_SYS_PROMPT) + \
					 64)
#define CONFIG_SYS_MAXARGS		64
#define CONFIG_SYS_BARGSIZE		(CONFIG_SYS_CBSIZE)

#endif /* __configs_chromeos_common_h__ */
