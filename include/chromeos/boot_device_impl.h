/*
 * Copyright (c) 2011 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 * Alternatively, this software may be distributed under the terms of the
 * GNU General Public License ("GPL") version 2 as published by the Free
 * Software Foundation.
 */

#ifndef __CHROMEOS_BOOT_DEVICE_IMPL_H__
#define __CHROMEOS_BOOT_DEVICE_IMPL_H__

#include <part.h>

/* Set boot device.
 *
 * Set partition number in argument part (starting from 1).  Pass part=0 for
 * the entire device.
 * Return zero if succees and non-zero if error.
 */
int set_bootdev(char *ifname, int dev, int part);

/* Return block device descriptor that was set by calls to set_bootdev(). */
block_dev_desc_t *get_bootdev(void);

/* Return the offset (in blocks) of the partition or zero if part=0. */
ulong get_offset(void);

/* Return the size (in blocks) of the partition or entire device if part=0. */
ulong get_limit(void);

#endif /* __CHROMEOS_BOOT_DEVICE_IMPL_H__ */
