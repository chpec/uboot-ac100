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
#include <asm/io.h>
#include <asm/arch/sys_proto.h>
#include <asm/mach-types.h>
#include <asm/arch/tegra2.h>
#include "harmony.h"

/*
 * Routine: board_init
 * Description: Early hardware init.
 */
int board_init(void)
{
	DECLARE_GLOBAL_DATA_PTR;

#if CONFIG_TEGRA2_HARMONY_ENABLE_DEBUG_UART
	/* Enable UARTA - Output on the debug board UART */
	*((volatile unsigned long *)(NV_ADDRESS_MAP_APB_MISC_BASE + APB_MISC_PP_PIN_MUX_CTL_C_0)) &= ~(0xF << 16);
	*((volatile unsigned long *)(NV_ADDRESS_MAP_APB_MISC_BASE + APB_MISC_PP_TRISTATE_REG_A_0)) &= ~(0x3 << 19);

	/* Select pllp_out0 (216MHz) as the clock source for UARTA */
	*((volatile unsigned long *)(NV_ADDRESS_MAP_CLK_RST_BASE + CLK_RST_CONTROLLER_CLK_SOURCE_UARTA_0)) &= ~(0x3 << 30);
	
	/* Reset UARTA */
	*((volatile unsigned long *)(NV_ADDRESS_MAP_CLK_RST_BASE + CLK_RST_CONTROLLER_RST_DEV_L_SET_0)) |= (1 << 6);
	udelay(10);
	*((volatile unsigned long *)(NV_ADDRESS_MAP_CLK_RST_BASE + CLK_RST_CONTROLLER_CLK_OUT_ENB_L_0)) |= (1 << 6);
	udelay(10);
	*((volatile unsigned long *)(NV_ADDRESS_MAP_CLK_RST_BASE + CLK_RST_CONTROLLER_RST_DEV_L_CLR_0)) |= (1 << 6);
	udelay(10);
#endif

#if CONFIG_TEGRA2_HARMONY_ENABLE_KEYBOARD_UART
	/* Enable UARTD pinmux - Output on the keyboard satellite board UART */

	/* Reset:
	 * 	PIN_MUX_CTL_B, GMC_SEL select UARTD (assigned to pin group gmc on Harmony)
	 */
	*((volatile unsigned long *)(NV_ADDRESS_MAP_APB_MISC_BASE + APB_MISC_PP_PIN_MUX_CTL_B_0)) &= ~(0x3 << 2);

	/* Select pllp_out0 (216MHz) as the clock source for UARTD */
	*((volatile unsigned long *)(NV_ADDRESS_MAP_CLK_RST_BASE + CLK_RST_CONTROLLER_CLK_SOURCE_UARTD_0)) &= ~(0x3 << 30);


	/* Reset UARTD */
	*((volatile unsigned long *)(NV_ADDRESS_MAP_CLK_RST_BASE + CLK_RST_CONTROLLER_RST_DEV_U_SET_0)) |= (1 << 1);
	udelay(10);
	*((volatile unsigned long *)(NV_ADDRESS_MAP_CLK_RST_BASE + CLK_RST_CONTROLLER_CLK_ENB_U_SET_0)) |= (1 << 1);
	udelay(10);
	*((volatile unsigned long *)(NV_ADDRESS_MAP_CLK_RST_BASE + CLK_RST_CONTROLLER_RST_DEV_U_CLR_0)) |= (1 << 1);
	udelay(10);
#endif

	/* boot param addr */
	gd->bd->bi_boot_params = (NV_ADDRESS_MAP_SDRAM_BASE + 0x100);
	/* board id for Linux */
	gd->bd->bi_arch_number = MACH_TYPE_TEGRA_GENERIC;
	
	return 0;
}

/*
 * Routine: misc_init_r
 * Description: Configure board specific parts
 */
int misc_init_r(void)
{
#ifdef CONFIG_CMDLINE
	parse_fastboot_cmd();
#endif
	return 0;
}

/*
 * Routine: set_muxconf_regs
 * Description: Setting up the configuration Mux registers specific to the
 *		hardware. Many pins need to be moved from protect to primary
 *		mode.
 */
void set_muxconf_regs(void)
{
}
