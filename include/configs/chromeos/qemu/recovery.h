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

#ifndef __CONFIGS_CHROMEOS_QEMU_RECOVERY_H__
#define __CONFIGS_CHROMEOS_QEMU_RECOVERY_H__

#include <configs/chromeos/recovery.h>

/* HACK: undef variables that will be defined by versatile.h */
#undef CONFIG_ENV_SIZE
#undef CONFIG_SYS_MALLOC_LEN
#undef CONFIG_BAUDRATE
#undef CONFIG_SYS_BAUDRATE_TABLE
#undef CONFIG_SYS_LONGHELP
#undef CONFIG_SYS_PROMPT
#undef CONFIG_BOOTARGS
#undef CONFIG_SYS_CBSIZE
#undef CONFIG_SYS_PBSIZE
#undef CONFIG_SYS_MAXARGS
#undef CONFIG_SYS_BARGSIZE

#include <configs/versatile.h>

/* HACK: restore variables */
#undef CONFIG_SYS_PROMPT
#define CONFIG_SYS_PROMPT "CrOS> "

/* HACK: prevent compile error */
#define CONFIG_GENERIC_MMC
#define CONFIG_GENERIC_MMC_MULTI_BLOCK_READ
#undef CONFIG_DISPLAY_CPUINFO
#undef CONFIG_DISPLAY_BOARDINFO

#endif /* __CONFIGS_CHROMEOS_QEMU_RECOVERY_H__ */
