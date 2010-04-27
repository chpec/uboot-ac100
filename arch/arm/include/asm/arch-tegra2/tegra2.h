/*
 * (C) Copyright 2010
 * NVIDIA Corporation <www.nvidia.com>
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

#ifndef _TEGRA2_H_
#define _TEGRA2_H_

#ifndef _MK_SHIFT_CONST
  #define _MK_SHIFT_CONST(_constant_) _constant_
#endif
#ifndef _MK_MASK_CONST
  #define _MK_MASK_CONST(_constant_) _constant_
#endif
#ifndef _MK_ENUM_CONST
  #define _MK_ENUM_CONST(_constant_) (_constant_ ## UL)
#endif
#ifndef _MK_ADDR_CONST
  #define _MK_ADDR_CONST(_constant_) _constant_
#endif

/* ap20/arclk_rst.h */
#define CLK_RST_CONTROLLER_RST_DEVICES_L_0           _MK_ADDR_CONST(0x4)
#define CLK_RST_CONTROLLER_CLK_SOURCE_UARTA_0        _MK_ADDR_CONST(0x178)
#define CLK_RST_CONTROLLER_RST_DEV_L_SET_0           _MK_ADDR_CONST(0x300)
#define CLK_RST_CONTROLLER_CLK_OUT_ENB_L_0           _MK_ADDR_CONST(0x10)
#define CLK_RST_CONTROLLER_RST_DEV_L_CLR_0           _MK_ADDR_CONST(0x304)
#define CLK_RST_CONTROLLER_CLK_SOURCE_UARTD_0        _MK_ADDR_CONST(0x1c0)
#define CLK_RST_CONTROLLER_RST_DEV_U_SET_0           _MK_ADDR_CONST(0x310)
#define CLK_RST_CONTROLLER_CLK_ENB_U_SET_0           _MK_ADDR_CONST(0x330)
#define CLK_RST_CONTROLLER_RST_DEV_U_CLR_0           _MK_ADDR_CONST(0x314)

/* ap20/arapb_misc.h */
#define APB_MISC_PP_PIN_MUX_CTL_C_0                  _MK_ADDR_CONST(0x88)
#define APB_MISC_PP_TRISTATE_REG_A_0                 _MK_ADDR_CONST(0x14)
#define APB_MISC_PP_PIN_MUX_CTL_B_0                  _MK_ADDR_CONST(0x84)

/* ap20/arapbpm.h */
#define APBDEV_PMC_SCRATCH20_0                       _MK_ADDR_CONST(0xa0)

#define NV_ADDRESS_MAP_SDRAM_BASE	0x00000000
#define NV_ADDRESS_MAP_ARM_PERIPH_BASE	0x50000000
#define NV_ADDRESS_MAP_PPSB_TMRUS_BASE	0x60005010
#define NV_ADDRESS_MAP_CLK_RST_BASE	0x60006000
#define NV_ADDRESS_MAP_APB_MISC_BASE	0x70000000
#define NV_ADDRESS_MAP_APB_UARTA_BASE	(NV_ADDRESS_MAP_APB_MISC_BASE + 0x6000)
#define NV_ADDRESS_MAP_APB_UARTB_BASE	(NV_ADDRESS_MAP_APB_MISC_BASE + 0x6040)
#define NV_ADDRESS_MAP_APB_UARTC_BASE	(NV_ADDRESS_MAP_APB_MISC_BASE + 0x6200)
#define NV_ADDRESS_MAP_APB_UARTD_BASE	(NV_ADDRESS_MAP_APB_MISC_BASE + 0x6300)
#define NV_ADDRESS_MAP_APB_UARTE_BASE	(NV_ADDRESS_MAP_APB_MISC_BASE + 0x6400)

#define LOW_LEVEL_SRAM_STACK		0x4000FFFC

#ifndef __ASSEMBLY__
typedef volatile struct timerus {
        unsigned int cntr_1us;
} timerus_t;

#ifdef CONFIG_CMDLINE
#define CMDLINE_MAX_LENGTH	1024
extern char cmdline_copy[];
void parse_fastboot_cmd(void);
#endif

#else  /* __ASSEMBLY__ */
#define PRM_RSTCTRL             0x7000E400
#endif

#define NAND_BASE               0x70008000
#define TEGRA2_SDRC_CS0         0x00000000

#define AP20_BOOT_INFO_BASE     0x40000000UL
#define AP20_PMC_BASE           0x7000e400UL

#endif
