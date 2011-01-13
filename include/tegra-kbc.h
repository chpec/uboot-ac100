/*
 * Copyright (c) 2011 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef __include_tegra_kbc_h__
#define __include_tegra_kbc_h__

#include <common.h>

#define KEY_FN          222
#define KEY_SHIFT       223

typedef struct {
        u32     reg;
        u32     value;
        u32     mask;
} tegra_register_update;

typedef struct {
        tegra_register_update   tri;
        tegra_register_update   func;
        tegra_register_update   pupd;
} tegra_pinmux_config;

typedef struct {
    tegra_pinmux_config *       pinmux;
    int                         pinmux_length;
    int *                       plain_keycode;
    int *                       shift_keycode;
    int *                       function_keycode;
} tegra_keyboard_config;

#endif /* __include_tegra_kbc_h__ */
