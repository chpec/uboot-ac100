/*
 * Copyright (c) 2011 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#define CONFIG_TEGRA2_PINMUX_DISPLAY_HARMONY                    \
        PINMUX(LCSN,  DISPLAYA,      PULL_UP,   TRISTATE)       \
        PINMUX(LD0,   DISPLAYA,      PULL_DOWN, NORMAL)         \
        PINMUX(LD1,   DISPLAYA,      PULL_DOWN, NORMAL)         \
        PINMUX(LD10,  DISPLAYA,      PULL_DOWN, NORMAL)         \
        PINMUX(LD11,  DISPLAYA,      PULL_DOWN, NORMAL)         \
        PINMUX(LD12,  DISPLAYA,      PULL_DOWN, NORMAL)         \
        PINMUX(LD13,  DISPLAYA,      PULL_DOWN, NORMAL)         \
        PINMUX(LD14,  DISPLAYA,      PULL_DOWN, NORMAL)         \
        PINMUX(LD15,  DISPLAYA,      PULL_DOWN, NORMAL)         \
        PINMUX(LD16,  DISPLAYA,      PULL_DOWN, NORMAL)         \
        PINMUX(LD17,  DISPLAYA,      PULL_DOWN, NORMAL)         \
        PINMUX(LD2,   DISPLAYA,      PULL_DOWN, NORMAL)         \
        PINMUX(LD3,   DISPLAYA,      PULL_DOWN, NORMAL)         \
        PINMUX(LD4,   DISPLAYA,      PULL_DOWN, NORMAL)         \
        PINMUX(LD5,   DISPLAYA,      PULL_DOWN, NORMAL)         \
        PINMUX(LD6,   DISPLAYA,      PULL_DOWN, NORMAL)         \
        PINMUX(LD7,   DISPLAYA,      PULL_DOWN, NORMAL)         \
        PINMUX(LD8,   DISPLAYA,      PULL_DOWN, NORMAL)         \
        PINMUX(LD9,   DISPLAYA,      PULL_DOWN, NORMAL)         \
        PINMUX(LDI,   DISPLAYA,      PULL_DOWN, NORMAL)         \
        PINMUX(LHP0,  DISPLAYA,      PULL_DOWN, NORMAL)         \
        PINMUX(LHP1,  DISPLAYA,      PULL_DOWN, NORMAL)         \
        PINMUX(LHP2,  DISPLAYA,      PULL_DOWN, NORMAL)         \
        PINMUX(LHS,   DISPLAYA,      PULL_UP,   NORMAL)         \
        PINMUX(LM0,   DISPLAYA,      PULL_UP,   NORMAL)         \
        PINMUX(LM1,   DISPLAYA,      PULL_UP,   TRISTATE)       \
        PINMUX(LPP,   DISPLAYA,      PULL_DOWN, NORMAL)         \
        PINMUX(LPW0,  DISPLAYA,      PULL_UP,   NORMAL)         \
        PINMUX(LPW1,  DISPLAYA,      PULL_UP,   TRISTATE)       \
        PINMUX(LPW2,  DISPLAYA,      PULL_UP,   NORMAL)         \
        PINMUX(LSC0,  DISPLAYA,      PULL_UP,   NORMAL)         \
        PINMUX(LSC1,  DISPLAYA,      PULL_UP,   TRISTATE)       \
        PINMUX(LSCK,  DISPLAYA,      PULL_UP,   TRISTATE)       \
        PINMUX(LSDA,  DISPLAYA,      PULL_UP,   TRISTATE)       \
        PINMUX(LSDI,  DISPLAYA,      PULL_UP,   TRISTATE)       \
        PINMUX(LSPI,  DISPLAYA,      PULL_UP,   NORMAL)         \
        PINMUX(LVP0,  DISPLAYA,      PULL_UP,   TRISTATE)       \
        PINMUX(LVP1,  DISPLAYA,      PULL_DOWN, NORMAL)         \
        PINMUX(LVS,   DISPLAYA,      PULL_UP,   NORMAL)         \
        PINMUX(SDC,   PWM,           PULL_UP,   NORMAL)

#define CONFIG_TEGRA2_PINMUX                    \
        CONFIG_TEGRA2_PINMUX_DISPLAY_HARMONY
