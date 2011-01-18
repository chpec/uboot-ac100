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

#ifndef __board_tegra2_common_lcd_gpinit_gp_util_h__
#define __board_tegra2_common_lcd_gpinit_gp_util_h__

#include <asm/types.h>

#include "gp-dc_reg.h"
#include "gpinit.h"
#include "gp-pinmux.h"

extern struct tegra_gpio_init_table	tegra2_gp_gpio_init_table[];
extern unsigned int			tegra2_gp_gpio_offset_tab_len;
extern struct tegra_clk_init_table	tegra2_gp_clk_init_table[];
extern struct tegra_pingroup_config	tegra2_gp_pinmux[];
extern unsigned int			tegra2_gp_pinmux_tab_len;
extern struct resource			tegra2_gp_panel_resources[];
extern struct tegra_dc_mode		tegra2_gp_panel_modes[];
extern struct tegra_fb_data		tegra2_gp_fb_data;

#endif /*__board_tegra2_common_lcd_gpinit_gp_util_h__*/
