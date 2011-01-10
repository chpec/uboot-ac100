/*
 * Copyright 2010, Google Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 * * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Alternatively, this software may be distributed under the terms of the
 * GNU General Public License ("GPL") version 2 as published by the Free
 * Software Foundation.
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

#define CONFIG_DISPLAY_CPUINFO		1
#define CONFIG_DISPLAY_BOARDINFO	1

#define CONFIG_SYS_LONGHELP		1
#define CONFIG_SYS_PROMPT		"CrOS> "

#define CONFIG_CMDLINE_EDITING		1
#define CONFIG_COMMAND_HISTORY		1
#define CONFIG_AUTOCOMPLETE		1

#define CONFIG_SYS_CBSIZE		512
#define CONFIG_SYS_PBSIZE		(CONFIG_SYS_CBSIZE + \
					 sizeof(CONFIG_SYS_PROMPT) + \
					 64)
#define CONFIG_SYS_MAXARGS		64
#define CONFIG_SYS_BARGSIZE		(CONFIG_SYS_CBSIZE)

#endif //__configs_chromeos_common_h__
