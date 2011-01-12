/*
 * Copyright (c) 2011 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/* To set base address of USB controller */
#define NvUSBx_0        USB_EHCI_TEGRA_BASE_ADDR_USB3
#define NvUSBx_1        USB_EHCI_TEGRA_BASE_ADDR_USB1
#define NvUSBx_2        0
#define NvUSBx_3        0

/*
 * USB1 takes 3 ms to clear Port Reset bit after issuing a Port Reset.
 * The expected time is 2 ms. Please refer to page 28 of EHCI 1.0 specification.
 */
#define CONFIG_USB_RESET_CLEARED_MS     3 * 1000
