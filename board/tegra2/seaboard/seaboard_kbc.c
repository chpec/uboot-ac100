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
	   0,    0,  'w',  's',  'a',  'z',    0,    KEY_FN,
	   0,    0,    0,    0,    0,    0,    0,    0,
	   0,    0,    0,    0,    0,    0,    0,    0,
	 '5',  '4',  'r',  'e',  'f',  'd',  'x',    0,
	 '7',  '6',  't',  'h',  'g',  'v',  'c',  ' ',
	 '9',  '8',  'u',  'y',  'j',  'n',  'b', '\\',
	 '-',  '0',  'o',  'i',  'l',  'k',  ',',  'm',
	   0,  '=',  ']', '\r',    0,    0,    0,    0,
	   0,    0,    0,    0, KEY_SHIFT, KEY_SHIFT,    0,    0,
	   0,    0,    0,    0,    0,    0,    0,    0,
	   0,    0,    0,    0,    0,    0,    0,    0,
	 '[',  'p', '\'',  ';',  '/',  '.',    0,    0,
	   0,    0, 0x08,  '3',  '2',    0,    0,    0,
	   0, 0x7F,    0,    0,    0,    0,    0,    0,
	   0,    0,    0,  'q',    0,    0,  '1',    0,
	0x1B,  '`',    0, 0x09,    0,    0,    0,    0
};

static int shift_keycode[] = {
	   0,    0,  'W',  'S',  'A',  'Z',    0,    0,
	   0,    0,    0,    0,    0,    0,    0,    0,
	   0,    0,    0,    0,    0,    0,    0,    0,
	 '%',  '$',  'R',  'E',  'F',  'D',  'X',    0,
	 '&',  '^',  'T',  'H',  'G',  'V',  'C',  ' ',
	 '(',  '*',  'U',  'Y',  'J',  'N',  'B',  '|',
	 '_',  ')',  'O',  'I',  'L',  'K',  ',',  'M',
	   0,  '+',  '}', '\r',    0,    0,    0,    0,
	   0,    0,    0,    0,    0,    0,    0,    0,
	   0,    0,    0,    0,    0,    0,    0,    0,
	   0,    0,    0,    0,    0,    0,    0,    0,
	 '{',  'P',  '"',  ':',  '?',  '>',    0,    0,
	   0,    0, 0x08,  '#',  '@',    0,    0,    0,
	   0, 0x7F,    0,    0,    0,    0,    0,    0,
	   0,    0,    0,  'Q',    0,    0,  '!',    0,
	0x1B,  '~',    0, 0x09,    0,    0,    0,    0
};

static int function_keycode[] = {
	   0,    0,    0,    0,    0,    0,    0,    0,
	   0,    0,    0,    0,    0,    0,    0,    0,
	   0,    0,    0,    0,    0,    0,    0,    0,
	   0,    0,    0,    0,    0,    0,    0,    0,
	 '7',    0,    0,    0,    0,    0,    0,    0,
	 '9',  '8',  '4',    0,  '1',    0,    0,    0,
	   0,  '/',  '6',  '5',  '3',  '2',    0,  '0',
	   0,    0,    0,    0,    0,    0,    0,    0,
	   0,    0,    0,    0,    0,    0,    0,    0,
	   0,    0,    0,    0,    0,    0,    0,    0,
	   0,    0,    0,    0,    0,    0,    0,    0,
	   0, '\'',    0,  '-',  '+',  '.',    0,    0,
	   0,    0,    0,    0,    0,    0,    0,    0,
	   0,    0,    0,    0,    0,    0,    0,    0,
	   0,    0,    0,    0,    0,    0,    0,    0,
	   0,    0,    0,    0,  '?',    0,    0,    0
};

tegra_keyboard_config board_keyboard_config = {
    .pinmux           = keyboard_pinmux,
    .pinmux_length    = (sizeof(keyboard_pinmux) / sizeof(keyboard_pinmux[0])),
    .plain_keycode    = plain_keycode,
    .shift_keycode    = shift_keycode,
    .function_keycode = function_keycode,
};
