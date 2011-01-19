/*
 * Copyright 2011, Google Inc.
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

#ifndef __FIRMWARE_LAYOUT_H__
#define __FIRMWARE_LAYOUT_H__

#define CONFIG_FIRMWARE_SIZE	0x00400000 /* 4 MB */

/* The board configuration data must be the first */
#define CONFIG_OFFSET_BOARD_CONFIG_DATA	0x00000000
#define CONFIG_LENGTH_BOARD_CONFIG_DATA	0x00010000

#define CONFIG_OFFSET_BOOTSTUB	0x00010000
#define CONFIG_LENGTH_BOOTSTUB	0x00040000

#define CONFIG_OFFSET_RECOVERY	0x00050000
#define CONFIG_LENGTH_RECOVERY	0x00100000

#define CONFIG_OFFSET_FMAP	0x00150000
#define CONFIG_LENGTH_FMAP	0x00010000

#define CONFIG_OFFSET_GBB	0x00160000
#define CONFIG_LENGTH_GBB	0x00040000

#define CONFIG_OFFSET_ROVPD	0x001a0000
#define CONFIG_LENGTH_ROVPD	0x00020000

#define CONFIG_OFFSET_FW_A_KEY	0x00200000
#define CONFIG_LENGTH_FW_A_KEY	0x00010000

#define CONFIG_OFFSET_FW_A_DATA	0x00210000
#define CONFIG_LENGTH_FW_A_DATA	0x000d0000

#define CONFIG_OFFSET_FW_B_KEY	0x002e0000
#define CONFIG_LENGTH_FW_B_KEY	0x00010000

#define CONFIG_OFFSET_FW_B_DATA	0x002f0000
#define CONFIG_LENGTH_FW_B_DATA	0x000d0000

#define CONFIG_OFFSET_RWVPD	0x003c0000
#define CONFIG_LENGTH_RWVPD	0x00001000

#define CONFIG_OFFSET_LOGVOL	0x003d0000
#define CONFIG_LENGTH_LOGVOL	0x00030000

#endif /* __FIRMWARE_LAYOUT_H__ */
