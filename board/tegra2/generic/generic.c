/*
 * Copyright (c) 2011 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
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
#include "generic.h"
#include <spi.h>

void board_spi_init(void)
{
#if defined(CONFIG_TEGRA2_SPI)
	spi_init();
#endif
}

/***************************************************************************
 * Routines for SD/EMMC board specific configuration.
 ***************************************************************************/

/***************************************************************************
 * Routines for UART board specific configuration.
 ***************************************************************************/

#if (CONFIG_TEGRA2_ENABLE_UARTA)
void NvBlUartInitA(void)
{
    NvBlUartClockInit(
	CLK_RST_CONTROLLER_RST_DEVICES_L_0,
	NV_FLD_MASK(CLK_RST_CONTROLLER, RST_DEVICES_L, SWR_UARTA_RST),
	NV_DRF_DEF(CLK_RST_CONTROLLER, RST_DEVICES_L, SWR_UARTA_RST, ENABLE),
	NV_DRF_DEF(CLK_RST_CONTROLLER, RST_DEVICES_L, SWR_UARTA_RST, DISABLE),

	CLK_RST_CONTROLLER_CLK_OUT_ENB_L_0,
	NV_FLD_MASK(CLK_RST_CONTROLLER, CLK_OUT_ENB_L, CLK_ENB_UARTA),
	NV_DRF_DEF(CLK_RST_CONTROLLER, CLK_OUT_ENB_L, CLK_ENB_UARTA, ENABLE),

	CLK_RST_CONTROLLER_CLK_SOURCE_UARTA_0,
	NV_DRF_DEF(CLK_RST_CONTROLLER,
		   CLK_SOURCE_UARTA,
		   UARTA_CLK_SRC,
		   PLLP_OUT0));

    /* Enable UARTA - Harmony board uses config4 */
    CONFIG(A, C, IRRX, UARTA);
    CONFIG(A, C, IRTX, UARTA);

    NvBlUartInitBase((NvU8 *)NV_ADDRESS_MAP_APB_UARTA_BASE);
}
#endif

#if (CONFIG_TEGRA2_ENABLE_UARTB)
void NvBlUartInitB(void)
{
    NvBlUartClockInit(
	CLK_RST_CONTROLLER_RST_DEVICES_L_0,
	NV_FLD_MASK(CLK_RST_CONTROLLER, RST_DEVICES_L, SWR_UARTB_RST),
	NV_DRF_DEF(CLK_RST_CONTROLLER, RST_DEVICES_L, SWR_UARTB_RST, ENABLE),
	NV_DRF_DEF(CLK_RST_CONTROLLER, RST_DEVICES_L, SWR_UARTB_RST, DISABLE),

	CLK_RST_CONTROLLER_CLK_OUT_ENB_L_0,
	NV_FLD_MASK(CLK_RST_CONTROLLER, CLK_OUT_ENB_L, CLK_ENB_UARTB),
	NV_DRF_DEF(CLK_RST_CONTROLLER, CLK_OUT_ENB_L, CLK_ENB_UARTB, ENABLE),

	CLK_RST_CONTROLLER_CLK_SOURCE_UARTB_0,
	NV_DRF_DEF(CLK_RST_CONTROLLER,
		   CLK_SOURCE_UARTB,
		   UARTB_CLK_SRC,
		   PLLP_OUT0));

    /* Enable UARTB */
    CONFIG(B, A, UAD, IRDA);

    NvBlUartInitBase((NvU8 *)NV_ADDRESS_MAP_APB_UARTB_BASE);
}
#endif

#if (CONFIG_TEGRA2_ENABLE_UARTD)
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
#endif

void NvBlUartInit(void)
{
#if (CONFIG_TEGRA2_ENABLE_UARTA)
    NvBlUartInitA();
#endif

#if (CONFIG_TEGRA2_ENABLE_UARTB)
    NvBlUartInitB();
#endif

#if (CONFIG_TEGRA2_ENABLE_UARTD)
    NvBlUartInitD();
#endif
}
