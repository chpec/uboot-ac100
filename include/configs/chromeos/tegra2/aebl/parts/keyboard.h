/*
 * Copyright (c) 2011 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#define CONFIG_TEGRA2_KBC_PLAIN_KEYCODES                        \
        0,    0,  'w',  's',  'a',  'z',    0,    KEY_FN,       \
        0,    0,    0,    0,    0,    0,    0,    0,            \
        0,    0,    0,    0,    0,    0,    0,    0,            \
      '5',  '4',  'r',  'e',  'f',  'd',  'x',    0,            \
      '7',  '6',  't',  'h',  'g',  'v',  'c',  ' ',            \
      '9',  '8',  'u',  'y',  'j',  'n',  'b', '\\',            \
      '-',  '0',  'o',  'i',  'l',  'k',  ',',  'm',            \
        0,  '=',  ']', '\r',    0,    0,    0,    0,            \
        0,    0,    0,    0, KEY_SHIFT, KEY_SHIFT,    0,    0,  \
        0,    0,    0,    0,    0,    0,    0,    0,            \
        0,    0,    0,    0,    0,    0,    0,    0,            \
      '[',  'p', '\'',  ';',  '/',  '.',    0,    0,            \
        0,    0, 0x08,  '3',  '2',    0,    0,    0,            \
        0, 0x7F,    0,    0,    0,    0,    0,    0,            \
        0,    0,    0,  'q',    0,    0,  '1',    0,            \
     0x1B,  '`',    0, 0x09,    0,    0,    0,    0

#define CONFIG_TEGRA2_KBC_SHIFT_KEYCODES                        \
        0,    0,  'W',  'S',  'A',  'Z',    0,    0,            \
        0,    0,    0,    0,    0,    0,    0,    0,            \
        0,    0,    0,    0,    0,    0,    0,    0,            \
      '%',  '$',  'R',  'E',  'F',  'D',  'X',    0,            \
      '&',  '^',  'T',  'H',  'G',  'V',  'C',  ' ',            \
      '(',  '*',  'U',  'Y',  'J',  'N',  'B',  '|',            \
      '_',  ')',  'O',  'I',  'L',  'K',  ',',  'M',            \
        0,  '+',  '}', '\r',    0,    0,    0,    0,            \
        0,    0,    0,    0,    0,    0,    0,    0,            \
        0,    0,    0,    0,    0,    0,    0,    0,            \
        0,    0,    0,    0,    0,    0,    0,    0,            \
      '{',  'P',  '"',  ':',  '?',  '>',    0,    0,            \
        0,    0, 0x08,  '#',  '@',    0,    0,    0,            \
        0, 0x7F,    0,    0,    0,    0,    0,    0,            \
        0,    0,    0,  'Q',    0,    0,  '!',    0,            \
     0x1B,  '~',    0, 0x09,    0,    0,    0,    0

#define CONFIG_TEGRA2_KBC_FUNCTION_KEYCODES                     \
        0,    0,    0,    0,    0,    0,    0,    0,            \
        0,    0,    0,    0,    0,    0,    0,    0,            \
        0,    0,    0,    0,    0,    0,    0,    0,            \
        0,    0,    0,    0,    0,    0,    0,    0,            \
      '7',    0,    0,    0,    0,    0,    0,    0,            \
      '9',  '8',  '4',    0,  '1',    0,    0,    0,            \
        0,  '/',  '6',  '5',  '3',  '2',    0,  '0',            \
        0,    0,    0,    0,    0,    0,    0,    0,            \
        0,    0,    0,    0,    0,    0,    0,    0,            \
        0,    0,    0,    0,    0,    0,    0,    0,            \
        0,    0,    0,    0,    0,    0,    0,    0,            \
        0, '\'',    0,  '-',  '+',  '.',    0,    0,            \
        0,    0,    0,    0,    0,    0,    0,    0,            \
        0,    0,    0,    0,    0,    0,    0,    0,            \
        0,    0,    0,    0,    0,    0,    0,    0,            \
        0,    0,    0,    0,  '?',    0,    0,    0