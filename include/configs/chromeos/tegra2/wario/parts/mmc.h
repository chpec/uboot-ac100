/*
 * Copyright (c) 2011 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/* Define this if device is always on EMMC4 controller and no Card Detect pin */
#define CONFIG_TEGRA2_EMMC4_ALWAYS_INSERTED     1
#define CONFIG_TEGRA2_EMMC4_IS_HS		1	/* only HS parts used */

#define MMC_DEV_INSTANCES 2
#define NvEmmcx_0       NvEmmc4
#define NvEmmcx_1       NvEmmc3
#define NvEmmcx_2       0
#define NvEmmcx_3       0
