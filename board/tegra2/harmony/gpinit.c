/*
 *  (C) Copyright 2010
 *  NVIDIA Corporation <www.nvidia.com>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>

#include "../common/lcd/gpinit/gp-dc_reg.h"
#include "../common/lcd/gpinit/gpinit.h"
#include "../common/lcd/gpinit/gp-pinmux.h"
#include "../common/lcd/gpinit/gp-util.h"

#define TEGRA_GPIO_BACKLIGHT		TEGRA_GPIO_PB5
#define TEGRA_GPIO_LVDS_SHUTDOWN	TEGRA_GPIO_PB2
#define TEGRA_GPIO_BACKLIGHT_PWM	TEGRA_GPIO_PB4
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
	{ "host1x",	"pll_p",	166000000,	true},
	{ "disp1",	"pll_p",	216000000,	true},
	{ "2d",		"pll_m",	266400000,	true},
	{ "3d",		"pll_m",	266400000,	true},
	{ "pwm",	"clk_32k",	32768,		true},
	{ NULL,		NULL,		0,		0},
};

struct tegra_pingroup_config tegra2_gp_pinmux[] = {
	{TEGRA_PINGROUP_LCSN,  TEGRA_MUX_DISPLAYA,      TEGRA_PUPD_PULL_UP,   TEGRA_TRI_TRISTATE},
	{TEGRA_PINGROUP_LD0,   TEGRA_MUX_DISPLAYA,      TEGRA_PUPD_PULL_DOWN, TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_LD1,   TEGRA_MUX_DISPLAYA,      TEGRA_PUPD_PULL_DOWN, TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_LD10,  TEGRA_MUX_DISPLAYA,      TEGRA_PUPD_PULL_DOWN, TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_LD11,  TEGRA_MUX_DISPLAYA,      TEGRA_PUPD_PULL_DOWN, TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_LD12,  TEGRA_MUX_DISPLAYA,      TEGRA_PUPD_PULL_DOWN, TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_LD13,  TEGRA_MUX_DISPLAYA,      TEGRA_PUPD_PULL_DOWN, TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_LD14,  TEGRA_MUX_DISPLAYA,      TEGRA_PUPD_PULL_DOWN, TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_LD15,  TEGRA_MUX_DISPLAYA,      TEGRA_PUPD_PULL_DOWN, TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_LD16,  TEGRA_MUX_DISPLAYA,      TEGRA_PUPD_PULL_DOWN, TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_LD17,  TEGRA_MUX_DISPLAYA,      TEGRA_PUPD_PULL_DOWN, TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_LD2,   TEGRA_MUX_DISPLAYA,      TEGRA_PUPD_PULL_DOWN, TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_LD3,   TEGRA_MUX_DISPLAYA,      TEGRA_PUPD_PULL_DOWN, TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_LD4,   TEGRA_MUX_DISPLAYA,      TEGRA_PUPD_PULL_DOWN, TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_LD5,   TEGRA_MUX_DISPLAYA,      TEGRA_PUPD_PULL_DOWN, TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_LD6,   TEGRA_MUX_DISPLAYA,      TEGRA_PUPD_PULL_DOWN, TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_LD7,   TEGRA_MUX_DISPLAYA,      TEGRA_PUPD_PULL_DOWN, TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_LD8,   TEGRA_MUX_DISPLAYA,      TEGRA_PUPD_PULL_DOWN, TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_LD9,   TEGRA_MUX_DISPLAYA,      TEGRA_PUPD_PULL_DOWN, TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_LDI,   TEGRA_MUX_DISPLAYA,      TEGRA_PUPD_PULL_DOWN, TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_LHP0,  TEGRA_MUX_DISPLAYA,      TEGRA_PUPD_PULL_DOWN, TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_LHP1,  TEGRA_MUX_DISPLAYA,      TEGRA_PUPD_PULL_DOWN, TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_LHP2,  TEGRA_MUX_DISPLAYA,      TEGRA_PUPD_PULL_DOWN, TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_LHS,   TEGRA_MUX_DISPLAYA,      TEGRA_PUPD_PULL_UP,   TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_LM0,   TEGRA_MUX_DISPLAYA,      TEGRA_PUPD_PULL_UP,   TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_LM1,   TEGRA_MUX_DISPLAYA,      TEGRA_PUPD_PULL_UP,   TEGRA_TRI_TRISTATE},
	{TEGRA_PINGROUP_LPP,   TEGRA_MUX_DISPLAYA,      TEGRA_PUPD_PULL_DOWN, TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_LPW0,  TEGRA_MUX_DISPLAYA,      TEGRA_PUPD_PULL_UP,   TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_LPW1,  TEGRA_MUX_DISPLAYA,      TEGRA_PUPD_PULL_UP,   TEGRA_TRI_TRISTATE},
	{TEGRA_PINGROUP_LPW2,  TEGRA_MUX_DISPLAYA,      TEGRA_PUPD_PULL_UP,   TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_LSC0,  TEGRA_MUX_DISPLAYA,      TEGRA_PUPD_PULL_UP,   TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_LSC1,  TEGRA_MUX_DISPLAYA,      TEGRA_PUPD_PULL_UP,   TEGRA_TRI_TRISTATE},
	{TEGRA_PINGROUP_LSCK,  TEGRA_MUX_DISPLAYA,      TEGRA_PUPD_PULL_UP,   TEGRA_TRI_TRISTATE},
	{TEGRA_PINGROUP_LSDA,  TEGRA_MUX_DISPLAYA,      TEGRA_PUPD_PULL_UP,   TEGRA_TRI_TRISTATE},
	{TEGRA_PINGROUP_LSDI,  TEGRA_MUX_DISPLAYA,      TEGRA_PUPD_PULL_UP,   TEGRA_TRI_TRISTATE},
	{TEGRA_PINGROUP_LSPI,  TEGRA_MUX_DISPLAYA,      TEGRA_PUPD_PULL_UP,   TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_LVP0,  TEGRA_MUX_DISPLAYA,      TEGRA_PUPD_PULL_UP,   TEGRA_TRI_TRISTATE},
	{TEGRA_PINGROUP_LVP1,  TEGRA_MUX_DISPLAYA,      TEGRA_PUPD_PULL_DOWN, TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_LVS,   TEGRA_MUX_DISPLAYA,      TEGRA_PUPD_PULL_UP,   TEGRA_TRI_NORMAL},
	{TEGRA_PINGROUP_SDC,   TEGRA_MUX_PWM,           TEGRA_PUPD_PULL_UP,   TEGRA_TRI_NORMAL},
};
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
		.start	= TEGRA_PWFM0_BASE,
		.end	= TEGRA_PWFM0_BASE + TEGRA_PWFM0_SIZE - 1,
	},
};

struct tegra_dc_mode tegra2_gp_panel_modes[] = {
	{
		.pclk = 79500000,
		.h_ref_to_sync = 4,
		.v_ref_to_sync = 2,
		.h_sync_width = 136,
		.v_sync_width = 4,
		.h_back_porch = 138,
		.v_back_porch = 21,
		.h_active = CONFIG_LCD_vl_col,
		.v_active = CONFIG_LCD_vl_row,
		.h_front_porch = 34,
		.v_front_porch = 4,
	},
};

struct tegra_fb_data tegra2_gp_fb_data = {
	.win            = 0,
	.xres           = CONFIG_LCD_vl_col,
	.yres           = CONFIG_LCD_vl_row,
	.bits_per_pixel = 16,
};
