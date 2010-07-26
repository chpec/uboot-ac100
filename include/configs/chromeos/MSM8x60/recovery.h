/*
 * Copyright (c) 2010, Code Aurora Forum. All rights reserved.
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

#ifndef __CONFIGS_CHROMEOS_8660_RECOVERY_H
#define __CONFIGS_CHROMEOS_8660_RECOVERY_H

#define CONFIG_CMDLINE_TAG
#define CONFIG_SETUP_MEMORY_TAGS

/*
 * Configure the LCD for recovery mode.
 * LCDC framebuffer is at MM HEAP1 in SMI
 * Note: Update as memory map changes
 */
//#define CONFIG_LCD
//#define CONFIG_SYS_WHITE_ON_BLACK
//#define CONFIG_MSM8X60_LCDC

#define LCD_BPP		LCD_COLOR24

#ifdef CONFIG_MSM8X60_LCDC
#   define LCDC_FB_ADDR		0x02b00000
#   define LCDC_FB_SIZE		0x00400000

/* 1024x600x24 @ 60 Hz */
#   define LCDC_vl_col	    1024
#   define LCDC_vl_row		600
#   define LCDC_vl_sync_width	1024
#   define LCDC_vl_sync_height	600
#   define LCDC_vl_hbp		80
#   define LCDC_vl_hfp		20
#   define LCDC_vl_vbp		22
#   define LCDC_vl_vfp		1
#   define LCDC_vl_hsync_width	40
#   define LCDC_vl_vsync_width	3

#   define LCD_MD_VAL_MHZ	0x1CF969FF //74.17 MHZ
#   define LCD_NS_VAL_MHZ	0x86F81B49 //74.17 MHZ
#   define LCD_CLK_PCOM_MHZ	74250000
#endif

/*
 * MMC/SD card
 */
#define CONFIG_MMC
#define CONFIG_EFI_PARTITION
//#define CONFIG_DOS_PARTITION

#include <configs/chromeos/MSM8x60/common.h>

/*
 * What monitor functions should be included
 */
#define CONFIG_CMD_AUTOSCRIPT	/* Autoscript Support		*/
#define CONFIG_CMD_BDI		/* bdinfo			*/
#define CONFIG_CMD_BOOTD	/* bootd			*/
#define CONFIG_CMD_CONSOLE	/* coninfo			*/
#define CONFIG_CMD_ECHO		/* echo arguments		*/
#define CONFIG_CMD_SAVEENV	/* saveenv			*/
#define CONFIG_CMD_FPGA		/* FPGA configuration Support	*/
#define CONFIG_CMD_IMI		/* iminfo			*/
#define CONFIG_CMD_ITEST	/* Integer (and string) test	*/
#define CONFIG_CMD_LOADB	/* loadb			*/
#define CONFIG_CMD_LOADS	/* loads			*/
#define CONFIG_CMD_MEMORY	/* md mm nm mw cp cmp crc base loop mtest */
#define CONFIG_CMD_MISC		/* Misc functions like sleep etc*/
#define CONFIG_CMD_RUN		/* run command in env variable	*/
#define CONFIG_CMD_SETGETDCR	/* DCR support on 4xx		*/
#define CONFIG_CMD_XIMG		/* Load part of Multi Image	*/
#define	CONFIG_CMD_DHCP
#define	CONFIG_CMD_PING
#define CONFIG_CMD_MMC
#define CONFIG_CMD_EXT2


#define CONFIG_CMD_RAMCONFIG
#define CONFIG_CMD_GETTIME
#define CONFIG_CMD_NPART

#define CONFIG_DYNAMIC_INODE_SIZE

#endif //__CONFIGS_CHROMEOS_8660_RECOVERY_H
