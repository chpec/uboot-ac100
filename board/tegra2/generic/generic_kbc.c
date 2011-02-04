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

static int plain_keycode[] = {
       0,    0,    0,    0,    0,    0,    0,    0,
       0, 0x1b, 0x09,  '`',  'a',  'z',  '1',  'q',
       0,    0,    0,    0,  'd',  'c',  '3',  'e',
       0,    0,    0,    0,    0,    0,    0,    0,
     'b',  'g',  't',  '5',  'f',  'v',  '4',  'r',
       0,    0,    0,    0,  's',  'x',  '2',  'w',
       0,    0,  ']',    0,  'k',  ',',  '8',  'i',
       0,    0,    0,    0,    0,    0,    0,    0,
     'n',  'h',  'y',  '6',  'j',  'm',  '7',  'u',
       0,    0,    0,    0,    0, KEY_SHIFT, 0, KEY_SHIFT,
     '=', '\'',  '[',  '-',  ';',  '/',  '0',  'p',
       0,    0,    0,    0,  'l',  '.',  '9',  'o',
       0,    0,    0,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,
       0, 0x08,    0, '\\', '\r',  ' ',    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0
};

static int shift_keycode[] = {
       0,    0,    0,    0,    0,    0,    0,    0,
       0, 0x1b, 0x09,  '~',  'A',  'Z',  '!',  'Q',
       0,    0,    0,    0,  'D',  'C',  '#',  'E',
       0,    0,    0,    0,    0,    0,    0,    0,
     'B',  'G',  'T',  '%',  'F',  'V',  '$',  'R',
       0,    0,    0,    0,  'S',  'X',  '@',  'W',
       0,    0,  '}',    0,  'K',  '<',  '*',  'I',
       0,    0,    0,    0,    0,    0,    0,    0,
     'N',  'H',  'Y',  '^',  'J',  'M',  '&',  'U',
       0,    0,    0,    0,    0, KEY_SHIFT, 0, KEY_SHIFT,
     '+',  '"',  '{',  '_',  ':',  '?',  ')',  'P',
       0,    0,    0,    0,  'L',  '>',  '(',  'O',
       0,    0,    0,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,
       0, 0x08,    0,  '|', '\r',  ' ',    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0
};

static int function_keycode[] = {
       0,    0,    0,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,
};

tegra_keyboard_config board_keyboard_config = {
    .pinmux           = keyboard_pinmux,
    .pinmux_length    = (sizeof(keyboard_pinmux) / sizeof(keyboard_pinmux[0])),
    .plain_keycode    = plain_keycode,
    .shift_keycode    = shift_keycode,
    .function_keycode = function_keycode
};
