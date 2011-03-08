/*
 * Copyright (c) 2011 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#define PINMUX(group, mux, pupd, tri)                   \
        {TEGRA_PINGROUP_##group, TEGRA_MUX_##mux,       \
         TEGRA_PUPD_##pupd, TEGRA_TRI_##tri},

#define CONFIG_TEGRA2_PINMUX_DISPLAY_CROS_1                     \
        PINMUX(GPU,   PWM,           NORMAL,    NORMAL)         \
        PINMUX(LD0,   DISPLAYA,      NORMAL,    NORMAL)         \
        PINMUX(LD1,   DISPLAYA,      NORMAL,    NORMAL)         \
        PINMUX(LD10,  DISPLAYA,      NORMAL,    NORMAL)         \
        PINMUX(LD11,  DISPLAYA,      NORMAL,    NORMAL)         \
        PINMUX(LD12,  DISPLAYA,      NORMAL,    NORMAL)         \
        PINMUX(LD13,  DISPLAYA,      NORMAL,    NORMAL)         \
        PINMUX(LD14,  DISPLAYA,      NORMAL,    NORMAL)         \
        PINMUX(LD15,  DISPLAYA,      NORMAL,    NORMAL)         \
        PINMUX(LD16,  DISPLAYA,      NORMAL,    NORMAL)         \
        PINMUX(LD17,  DISPLAYA,      NORMAL,    NORMAL)         \
        PINMUX(LD2,   DISPLAYA,      NORMAL,    NORMAL)         \
        PINMUX(LD3,   DISPLAYA,      NORMAL,    NORMAL)         \
        PINMUX(LD4,   DISPLAYA,      NORMAL,    NORMAL)         \
        PINMUX(LD5,   DISPLAYA,      NORMAL,    NORMAL)         \
        PINMUX(LD6,   DISPLAYA,      NORMAL,    NORMAL)         \
        PINMUX(LD7,   DISPLAYA,      NORMAL,    NORMAL)         \
        PINMUX(LD8,   DISPLAYA,      NORMAL,    NORMAL)         \
        PINMUX(LD9,   DISPLAYA,      NORMAL,    NORMAL)         \
        PINMUX(LDI,   DISPLAYA,      NORMAL,    NORMAL)         \
        PINMUX(LHP0,  DISPLAYA,      NORMAL,    NORMAL)         \
        PINMUX(LHP1,  DISPLAYA,      NORMAL,    NORMAL)         \
        PINMUX(LHP2,  DISPLAYA,      NORMAL,    NORMAL)         \
        PINMUX(LHS,   DISPLAYA,      NORMAL,    NORMAL)         \
        PINMUX(LM0,   RSVD4,         NORMAL,    NORMAL)         \
        PINMUX(LPP,   DISPLAYA,      NORMAL,    NORMAL)         \
        PINMUX(LPW0,  RSVD4,         NORMAL,    NORMAL)         \
        PINMUX(LPW1,  RSVD4,         NORMAL,    TRISTATE)       \
        PINMUX(LPW2,  RSVD4,         NORMAL,    NORMAL)         \
        PINMUX(LSC0,  DISPLAYA,      NORMAL,    NORMAL)         \
        PINMUX(LSPI,  DISPLAYA,      NORMAL,    NORMAL)         \
        PINMUX(LVP1,  DISPLAYA,      NORMAL,    NORMAL)         \
        PINMUX(LVS,   DISPLAYA,      NORMAL,    NORMAL)         \
        PINMUX(SLXD,  SPDIF,         NORMAL,    NORMAL)
