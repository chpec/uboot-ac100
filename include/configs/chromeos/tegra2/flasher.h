/*
 * Copyright (c) 2011 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef __configs_chromeos_tegra2_flasher_h__
#define __configs_chromeos_tegra2_flasher_h__

#include <configs/chromeos/flasher.h>
#include <configs/chromeos/tegra2/common.h>

#define IMAGE_ADDRESS   0xe48000        // TEXT_BASE + 0x40000
#define IMAGE_LENGTH    0x400000

#define CONFIG_EXTRA_ENV_SETTINGS_ARCH \
    CONFIG_EXTRA_ENV_SETTINGS_ARCH_COMMON \
    "image_address=" QUOTE(IMAGE_ADDRESS) "\0" \
    "image_length=" QUOTE(IMAGE_LENGTH) "\0" \
    "flash_spi=crc32 ${image_address} ${image_length}; " \
    "sf probe 0; " \
    "echo Erasing SPI flash ...; " \
    "sf erase 0 ${image_length}; " \
    "echo Writing SPI flash ...; " \
    "sf write ${image_address} 0 ${image_length}; " \
    "echo Clearing buffer ...; " \
    "mw.b ${image_address} 0 ${image_length}; " \
    "echo Reading SPI flash ...; " \
    "sf read ${image_address} 0 ${image_length}; " \
    "crc32 ${image_address} ${image_length}; " \
    "echo If the 2 CRCs above are equal, flash was successful;\0"

#endif //__configs_chromeos_tegra2_flasher_h__
