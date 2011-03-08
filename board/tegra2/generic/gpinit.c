/*
 * Copyright (c) 2011 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <common.h>

#include "../common/lcd/gpinit/gp-dc_reg.h"
#include "../common/lcd/gpinit/gpinit.h"
#include "../common/lcd/gpinit/gp-pinmux.h"
#include "../common/lcd/gpinit/gp-util.h"

#define TEGRA_GPIO_LIDSWITCH		TEGRA_GPIO_PC7
#define TEGRA_GPIO_POWERKEY		TEGRA_GPIO_PV2
#define TEGRA_GPIO_BACKLIGHT		TEGRA_GPIO_PD4
#define TEGRA_GPIO_LVDS_SHUTDOWN	TEGRA_GPIO_PB2
#define TEGRA_GPIO_BACKLIGHT_PWM	TEGRA_GPIO_PU5
#define TEGRA_GPIO_BACKLIGHT_VDD	TEGRA_GPIO_PW0
#define TEGRA_GPIO_EN_VDD_PNL		TEGRA_GPIO_PC6

struct tegra_gpio_init_table tegra2_gp_gpio_init_table[] = {
	{ TEGRA_GPIO_BACKLIGHT,		true},
	{ TEGRA_GPIO_LVDS_SHUTDOWN,	true},
	{ TEGRA_GPIO_BACKLIGHT_VDD,	false},
	{ TEGRA_GPIO_EN_VDD_PNL,	true},
};
unsigned int tegra2_gp_gpio_offset_tab_len = ARRAY_SIZE(tegra2_gp_gpio_init_table);

struct tegra_clk_init_table tegra2_gp_clk_init_table[] = {
	/* name		parent		rate		enabled */
	{ "3d",		"pll_m",	300000000,	true},
	{ "2d",		"pll_m",	300000000,	true},
	{ "host1x",	"pll_p",	144000000,	true},
	{ "disp1",	"pll_p",	216000000,	true},
	{ "pwm",	"clk_32k",	32768,		true},
	{ NULL,		NULL,		0,		0},
};

struct tegra_pingroup_config tegra2_gp_pinmux[] = { CONFIG_TEGRA2_PINMUX };

unsigned int tegra2_gp_pinmux_tab_len = ARRAY_SIZE(tegra2_gp_pinmux);

struct resource tegra2_gp_panel_resources[] = {
	{
		.name	= "irq",
		.start	= 0x69,
		.end	= 0x69,
	},
	{
		.name	= "regs",
		.start	= TEGRA_DISPLAY_BASE,
		.end	= TEGRA_DISPLAY_BASE + TEGRA_DISPLAY_SIZE - 1,
	},
	{
		.name	= "fbmem",
		.start	= LCD_FB_ADDR,
		.end	= LCD_FB_ADDR + 0x4000000 - 1, /* 64M */
	},
	{
		.name	= "pwm",
		.start	= TEGRA_PWFM2_BASE,
		.end	= TEGRA_PWFM2_BASE + TEGRA_PWFM2_SIZE - 1,
	},
};

struct tegra_dc_mode tegra2_gp_panel_modes[] = {
	{
		.pclk = 62200000,
		.h_ref_to_sync = 11,
		.v_ref_to_sync = 1,
		.h_sync_width = 58,
		.v_sync_width = 4,
		.h_back_porch = 58,
		.v_back_porch = 4,
		.h_active = CONFIG_LCD_vl_col,
		.v_active = CONFIG_LCD_vl_row,
		.h_front_porch = 58,
		.v_front_porch = 4,
	},
};

struct tegra_fb_data tegra2_gp_fb_data = {
	.win		= 0,
	.xres		= CONFIG_LCD_vl_col,
	.yres		= CONFIG_LCD_vl_row,
	.bits_per_pixel	= 16,
};
