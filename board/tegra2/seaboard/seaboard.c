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
#include <nand.h>
#include <asm/io.h>
#include <asm/arch/sys_proto.h>
#include <asm/mach-types.h>
#include <asm/arch/nvcommon.h>
#include <asm/arch/nv_hardware_access.h>
#include <asm/arch/nv_drf.h>
#include <asm/arch/tegra2.h>
#include "../common/board.h"
#include "seaboard.h"
#include <spi.h>

void board_spi_init(void)
{
	spi_init();
}

/***************************************************************************
 * Routines for SD/EMMC board specific configuration.
 ***************************************************************************/

/***************************************************************************
 * Routines for UART board specific configuration.
 ***************************************************************************/
void NvBlUartInitD(void)
{
    NvBlUartClockInit(
	CLK_RST_CONTROLLER_RST_DEVICES_U_0,
	NV_FLD_MASK(CLK_RST_CONTROLLER, RST_DEVICES_U, SWR_UARTD_RST),
	NV_DRF_DEF(CLK_RST_CONTROLLER, RST_DEVICES_U, SWR_UARTD_RST, ENABLE),
	NV_DRF_DEF(CLK_RST_CONTROLLER, RST_DEVICES_U, SWR_UARTD_RST, DISABLE),

	CLK_RST_CONTROLLER_CLK_OUT_ENB_U_0,
	NV_FLD_MASK(CLK_RST_CONTROLLER, CLK_OUT_ENB_U, CLK_ENB_UARTD),
	NV_DRF_DEF(CLK_RST_CONTROLLER, CLK_OUT_ENB_U, CLK_ENB_UARTD, ENABLE),

	CLK_RST_CONTROLLER_CLK_SOURCE_UARTD_0,
	NV_DRF_DEF(CLK_RST_CONTROLLER,
		   CLK_SOURCE_UARTD,
		   UARTD_CLK_SRC,
		   PLLP_OUT0));

    /* Enable UARTD - uses config2 */
    CONFIG(A, B, GMC, UARTD);

    NvBlUartInitBase((NvU8 *)NV_ADDRESS_MAP_APB_UARTD_BASE);
}

void NvBlUartInit(void)
{
#if (CONFIG_TEGRA2_ENABLE_UARTD)
    NvBlUartInitD();
#endif
}
