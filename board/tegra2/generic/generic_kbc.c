/*
 * Copyright (c) 2011 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <common.h>
#include <tegra-kbc.h>

static tegra_pinmux_config keyboard_pinmux[] = {
    {{0x14, 0, 22}, {0x88, 0, 10}, {0xA4, 2,  8}},
    {{0x14, 0, 21}, {0x88, 0, 12}, {0xA4, 2, 10}},
    {{0x18, 0, 26}, {0x88, 0, 14}, {0xA4, 2, 12}},
    {{0x20, 0, 10}, {0x98, 0, 26}, {0xA4, 2, 14}},
    {{0x14, 0, 26}, {0x80, 0, 28}, {0xb0, 2,  2}},
    {{0x14, 0, 27}, {0x80, 0, 26}, {0xb0, 2,  0}},
};

static int plain_keycode[] = { CONFIG_TEGRA2_KBC_PLAIN_KEYCODES };
static int shift_keycode[] = { CONFIG_TEGRA2_KBC_SHIFT_KEYCODES };
static int function_keycode[] = { CONFIG_TEGRA2_KBC_FUNCTION_KEYCODES };

tegra_keyboard_config board_keyboard_config = {
    .pinmux           = keyboard_pinmux,
    .pinmux_length    = (sizeof(keyboard_pinmux) / sizeof(keyboard_pinmux[0])),
    .plain_keycode    = plain_keycode,
    .shift_keycode    = shift_keycode,
    .function_keycode = function_keycode
};
