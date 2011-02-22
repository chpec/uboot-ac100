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

#ifndef __SEABOARD_FIRMWARE_LAYOUT_H__

/*
 * Introduce smaller firmware image layout for Seaboard for two reasons:
 *
 * 1. As a workaround before we move firmware storage from NAND to SPI Flash.
 *    Seaboard's NAND has about 1.5MB (after excluding BCT, partition table,
 *    and etc.) for firmware image.
 *
 * 2. If we are going to move firmware storage from NAND to SPI Flash on
 *    Seaboard, we will have to further shrink this layout because Seaboard's
 *    SPI Flash only has 4MBit (=512KB).
 */

/*
 * You must keep this name the same with the one in the top-level layout header
 * (include/configs/chromeos/firmware_layout.h) to mask it off.
 */
#ifdef __FIRMWARE_LAYOUT_H__
#error __FIRMWARE_LAYOUT_H__ has been defined
#endif
#define __FIRMWARE_LAYOUT_H__ /* mask off top-level layout header file */

#define CONFIG_FIRMWARE_SIZE	0x001d0000

#define CONFIG_OFFSET_BOOTSTUB	0x00000000
/* Workaround: u-boot requires about 0x90000 bytes RAM footprint when start-up.
 * We preserve enough space here so that the whole image can also be loaded into
 * RAM without being corrupted -- easier for debugging.
 */
#define CONFIG_LENGTH_BOOTSTUB	0x00090000

/*
 * Workaround: Temporarily put boot stub at first so that whole image can be
 * loaded into RAM as if it is an ordinary u-boot image.
 */
#define CONFIG_OFFSET_BOARD_CONFIG_DATA	0x00090000
#define CONFIG_LENGTH_BOARD_CONFIG_DATA	0x00010000

#define CONFIG_OFFSET_RECOVERY	0x000a0000
#define CONFIG_LENGTH_RECOVERY	0x00040000

#define CONFIG_OFFSET_FMAP	0x000e0000
#define CONFIG_LENGTH_FMAP	0x00000400

#define CONFIG_OFFSET_GBB	0x000e0400
#define CONFIG_LENGTH_GBB	0x00040000

#define CONFIG_OFFSET_ROVPD	0x00120400
#define CONFIG_LENGTH_ROVPD	0x00000400

#define CONFIG_OFFSET_FW_A_KEY	0x00120800
#define CONFIG_LENGTH_FW_A_KEY	0x00002000

#define CONFIG_OFFSET_FW_A_DATA	0x00122800
#define CONFIG_LENGTH_FW_A_DATA	0x00050000

#define CONFIG_OFFSET_FW_B_KEY	0x00172800
#define CONFIG_LENGTH_FW_B_KEY	0x00002000

#define CONFIG_OFFSET_FW_B_DATA	0x00174800
#define CONFIG_LENGTH_FW_B_DATA	0x00050000

#define CONFIG_OFFSET_RWVPD	0x001c4800
#define CONFIG_LENGTH_RWVPD	0x00000400

#define CONFIG_OFFSET_LOGVOL	0x001c4c00
#define CONFIG_LENGTH_LOGVOL	0x00000400

#define CONFIG_OFFSET_ENV	0x001c5000
#define CONFIG_LENGTH_ENV	0x00001000

#endif /* __SEABOARD_FIRMWARE_LAYOUT_H__ */
