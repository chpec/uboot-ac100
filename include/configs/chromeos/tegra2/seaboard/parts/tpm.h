/*
 * Copyright (c) 2011 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/* this macro enable tlcl stub to call real hardware functions */
#define CONFIG_HARDWARE_TPM

/* config for infineon prototype i2c tpm chip */
#define CONFIG_INFINEON_TPM_I2C
#define CONFIG_INFINEON_TPM_I2C_BUS       (2)
#define CONFIG_INFINEON_TPM_CHIP_ADDRESS  (0x1A)

