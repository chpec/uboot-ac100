/*
 * Copyright (c) 2011 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <common.h>

#define TEGRA_MISC_BASE 0x70000000
#define TEGRA_CLK_BASE 0x60006000

#define KBC_CLK_REG 0x328

#define readl(addr) (*(volatile unsigned int *)(addr))
#define writel(b, addr) ((*(volatile unsigned int *) (addr)) = (b))
#define misc_readl(addr) readl(TEGRA_MISC_BASE + addr)
#define misc_writel(b, addr) writel(b, TEGRA_MISC_BASE + addr)
#define clk_writel(b, addr) writel(b, TEGRA_CLK_BASE + addr)

static void pinmux_set_func(u32 mux_reg, u32 mux_val, u32 mux_bit)
{
	u32 reg;

	reg = misc_readl(mux_reg);
	reg &= ~(0x3 << mux_bit);
	reg |= mux_val << mux_bit;
	misc_writel(reg, mux_reg);
}

static void pinmux_set_tri(u32 tri_reg, u32 tri_val, u32 tri_bit)
{
	u32 reg;

	reg = misc_readl(tri_reg);
	reg &= ~(0x1 << tri_bit);
	if (tri_val)
		reg |= 1 << tri_bit;
	misc_writel(reg, tri_reg);
}

static void pinmux_set_pupd(u32 pupd_reg, u32 pupd_val, u32 pupd_bit)
{
	u32 reg;

	reg = misc_readl(pupd_reg);
	reg &= ~(0x3 << pupd_bit);
	reg |= pupd_val << pupd_bit;
	misc_writel(reg, pupd_reg);
}

void config_kbc_pinmux(void)
{
	pinmux_set_tri(0x14, 0, 22);
	pinmux_set_func(0x88, 0, 10);
	pinmux_set_pupd(0xA4, 2, 8);

	pinmux_set_tri(0x14, 0, 21);
	pinmux_set_func(0x88, 0, 12);
	pinmux_set_pupd(0xA4, 2, 10);

	pinmux_set_tri(0x18, 0, 26);
	pinmux_set_func(0x88, 0, 14);
	pinmux_set_pupd(0xA4, 2, 12);

	pinmux_set_tri(0x20, 0, 10);
	pinmux_set_func(0x98, 0, 26);
	pinmux_set_pupd(0xA4, 2, 14);

	pinmux_set_tri(0x14, 0, 26);
	pinmux_set_func(0x80, 0, 28);
	pinmux_set_pupd(0xB0, 2, 2);

	pinmux_set_tri(0x14, 0, 27);
	pinmux_set_func(0x80, 0, 26);
	pinmux_set_pupd(0xB0, 2, 0);

}

void config_kbc_clock(void)
{
	clk_writel(1 << 4, KBC_CLK_REG);
}
