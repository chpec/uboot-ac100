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

#include <asm/arch/gpio.h>

#include "gp-util.h"

static void clk_init(void)
{
	tegra_clk_common_init();
	tegra_clk_init_from_table(tegra2_gp_clk_init_table);
}

static void pinmux_init(void)
{
	tegra_pinmux_config_table(tegra2_gp_pinmux, tegra2_gp_pinmux_tab_len);
}

static void panel_init(void)
{
	int i;

	for (i = 0; i < tegra2_gp_gpio_offset_tab_len; i++) {
		tg2_gpio_direction_output_ex(
			tegra2_gp_gpio_init_table[i].offset, 1);
		if (tegra2_gp_gpio_init_table[i].set == true)
			tg2_gpio_set_value_ex(
				tegra2_gp_gpio_init_table[i].offset, 1);
	}
}

void gpinit(void)
{
	clk_init();
	pinmux_init();
	poweron_3d();
	panel_init();
	tegra_dc_register(tegra2_gp_panel_resources,
			tegra2_gp_panel_modes,
			&tegra2_gp_fb_data);
	tegra_pwm_enable();
	tegra_dc_probe();
}
