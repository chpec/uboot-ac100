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

/***************************************************************************
 * Routines for SD/EMMC board specific configuration.
 ***************************************************************************/

/***************************************************************************
 * Routines for UART board specific configuration.
 ***************************************************************************/
void NvBlUartClockInitA(void)
{
    NvU32 Reg;

    // Avoid running this function more than once.
    static int initialized = 0;
    if (initialized)
        return;
    initialized = 1;

    // 1. Assert Reset to UART A
    NV_CLK_RST_READ(RST_DEVICES_L, Reg);
    Reg = NV_FLD_SET_DRF_DEF(CLK_RST_CONTROLLER, RST_DEVICES_L,
                             SWR_UARTA_RST, ENABLE, Reg);
    NV_CLK_RST_WRITE(RST_DEVICES_L, Reg);

    // 2. Enable clk to UART A
    NV_CLK_RST_READ(CLK_OUT_ENB_L, Reg);
    Reg = NV_FLD_SET_DRF_DEF(CLK_RST_CONTROLLER, CLK_OUT_ENB_L,
                             CLK_ENB_UARTA, ENABLE, Reg);
    NV_CLK_RST_WRITE(CLK_OUT_ENB_L, Reg);


    // Override pllp setup for 216MHz operation.
    Reg = NV_DRF_DEF(CLK_RST_CONTROLLER, PLLP_BASE, PLLP_BYPASS, ENABLE)
          | NV_DRF_DEF(CLK_RST_CONTROLLER, PLLP_BASE, PLLP_ENABLE, DISABLE)
          | NV_DRF_DEF(CLK_RST_CONTROLLER, PLLP_BASE, PLLP_REF_DIS, REF_ENABLE)
          | NV_DRF_DEF(CLK_RST_CONTROLLER, PLLP_BASE, PLLP_BASE_OVRRIDE, ENABLE)
          | NV_DRF_NUM(CLK_RST_CONTROLLER, PLLP_BASE, PLLP_LOCK, 0x0)
          | NV_DRF_NUM(CLK_RST_CONTROLLER, PLLP_BASE, PLLP_DIVP, 0x1)
          | NV_DRF_NUM(CLK_RST_CONTROLLER, PLLP_BASE, PLLP_DIVN,
                       NVRM_PLLP_FIXED_FREQ_KHZ/500)
          | NV_DRF_NUM(CLK_RST_CONTROLLER, PLLP_BASE, PLLP_DIVM, 0x0C);
    NV_CLK_RST_WRITE(PLLP_BASE, Reg);

    Reg = NV_FLD_SET_DRF_DEF(CLK_RST_CONTROLLER, PLLP_BASE,
                             PLLP_ENABLE, ENABLE, Reg);
    NV_CLK_RST_WRITE(PLLP_BASE, Reg);

    Reg = NV_FLD_SET_DRF_DEF(CLK_RST_CONTROLLER, PLLP_BASE,
                             PLLP_BYPASS, DISABLE, Reg);
    NV_CLK_RST_WRITE(PLLP_BASE, Reg);

    // Enable pllp_out0 to UARTA.
    Reg = NV_DRF_DEF(CLK_RST_CONTROLLER, CLK_SOURCE_UARTA,
                     UARTA_CLK_SRC, PLLP_OUT0);
    NV_CLK_RST_WRITE(CLK_SOURCE_UARTA, Reg);


    // wait for 2us
    NvBlAvpStallUs(2);

    // De-assert reset to UART A
    NV_CLK_RST_READ(RST_DEVICES_L, Reg);
    Reg = NV_FLD_SET_DRF_DEF(CLK_RST_CONTROLLER, RST_DEVICES_L,
                             SWR_UARTA_RST, DISABLE, Reg);
    NV_CLK_RST_WRITE(RST_DEVICES_L, Reg);

}

void NvBlUartClockInitD(void)
{
    NvU32 Reg;

    // 1. Assert Reset to UART D
    NV_CLK_RST_READ(RST_DEVICES_U, Reg);
    Reg = NV_FLD_SET_DRF_DEF(CLK_RST_CONTROLLER, RST_DEVICES_U,
                             SWR_UARTD_RST, ENABLE, Reg);
    NV_CLK_RST_WRITE(RST_DEVICES_U, Reg);

    // 2. Enable clk to UART D
    NV_CLK_RST_READ(CLK_OUT_ENB_U, Reg);
    Reg = NV_FLD_SET_DRF_DEF(CLK_RST_CONTROLLER, CLK_OUT_ENB_U,
                             CLK_ENB_UARTD, ENABLE, Reg);
    NV_CLK_RST_WRITE(CLK_OUT_ENB_U, Reg);

    // Override pllp setup for 216MHz operation.
    Reg = NV_DRF_DEF(CLK_RST_CONTROLLER, PLLP_BASE, PLLP_BYPASS, ENABLE)
          | NV_DRF_DEF(CLK_RST_CONTROLLER, PLLP_BASE, PLLP_ENABLE, DISABLE)
          | NV_DRF_DEF(CLK_RST_CONTROLLER, PLLP_BASE, PLLP_REF_DIS, REF_ENABLE)
          | NV_DRF_DEF(CLK_RST_CONTROLLER, PLLP_BASE, PLLP_BASE_OVRRIDE, ENABLE)
          | NV_DRF_NUM(CLK_RST_CONTROLLER, PLLP_BASE, PLLP_LOCK, 0x0)
          | NV_DRF_NUM(CLK_RST_CONTROLLER, PLLP_BASE, PLLP_DIVP, 0x1)
          | NV_DRF_NUM(CLK_RST_CONTROLLER, PLLP_BASE, PLLP_DIVN,
                       NVRM_PLLP_FIXED_FREQ_KHZ/500)
          | NV_DRF_NUM(CLK_RST_CONTROLLER, PLLP_BASE, PLLP_DIVM, 0x0C);
    NV_CLK_RST_WRITE(PLLP_BASE, Reg);

    Reg = NV_FLD_SET_DRF_DEF(CLK_RST_CONTROLLER, PLLP_BASE,
                             PLLP_ENABLE, ENABLE, Reg);
    NV_CLK_RST_WRITE(PLLP_BASE, Reg);

    Reg = NV_FLD_SET_DRF_DEF(CLK_RST_CONTROLLER, PLLP_BASE,
                             PLLP_BYPASS, DISABLE, Reg);
    NV_CLK_RST_WRITE(PLLP_BASE, Reg);

    // Enable pllp_out0 to UARTD.
    Reg = NV_DRF_DEF(CLK_RST_CONTROLLER, CLK_SOURCE_UARTD,
                     UARTD_CLK_SRC, PLLP_OUT0);
    NV_CLK_RST_WRITE(CLK_SOURCE_UARTD, Reg);

    // wait for 2us
    NvBlAvpStallUs(2);

    // De-assert reset to UART D
    NV_CLK_RST_READ(RST_DEVICES_U, Reg);
    Reg = NV_FLD_SET_DRF_DEF(CLK_RST_CONTROLLER, RST_DEVICES_U,
                             SWR_UARTD_RST, DISABLE, Reg);
    NV_CLK_RST_WRITE(RST_DEVICES_U, Reg);

}

void
NvBlUartInitA(void)
{
    NvU32 Reg;

    // Avoid running this function more than once.
    static int initialized = 0;
    if (initialized)
        return;
    initialized = 1;

    NvBlUartClockInitA();

    /* Enable UARTA - Harmony board uses config4 */
    CONFIG(A,C,IRRX,UARTA); CONFIG(A,C,IRTX,UARTA);

    // Prepare the divisor value.
    Reg = NVRM_PLLP_FIXED_FREQ_KHZ * 1000 / NV_DEFAULT_DEBUG_BAUD / 16;

    // Set up UART parameters.
    NV_UARTA_WRITE(LCR,        0x80);
    NV_UARTA_WRITE(THR_DLAB_0, Reg);
    NV_UARTA_WRITE(IER_DLAB_0, 0x00);
    NV_UARTA_WRITE(LCR,        0x00);
    NV_UARTA_WRITE(IIR_FCR,    0x37);
    NV_UARTA_WRITE(IER_DLAB_0, 0x00);
    NV_UARTA_WRITE(LCR,        0x03);  // 8N1
    NV_UARTA_WRITE(MCR,        0x02);
    NV_UARTA_WRITE(MSR,        0x00);
    NV_UARTA_WRITE(SPR,        0x00);
    NV_UARTA_WRITE(IRDA_CSR,   0x00);
    NV_UARTA_WRITE(ASR,        0x00);

    NV_UARTA_WRITE(IIR_FCR,    0x31);

    // Flush any old characters out of the RX FIFO.
    while (NvBlUartRxReadyA())
        (void)NvBlUartRxA();
}

void
NvBlUartInitD(void)
{
    NvU32 Reg;

    NvBlUartClockInitD();

    /* Enable UARTD - Harmony board uses config2 */
    CONFIG(A,B,GMC,UARTD);

    // Prepare the divisor value.
    Reg = NVRM_PLLP_FIXED_FREQ_KHZ * 1000 / NV_DEFAULT_DEBUG_BAUD / 16;

    // Set up UART parameters.
    NV_UARTD_WRITE(LCR,        0x80);
    NV_UARTD_WRITE(THR_DLAB_0, Reg);
    NV_UARTD_WRITE(IER_DLAB_0, 0x00);
    NV_UARTD_WRITE(LCR,        0x00);
    NV_UARTD_WRITE(IIR_FCR,    0x37);
    NV_UARTD_WRITE(IER_DLAB_0, 0x00);
    NV_UARTD_WRITE(LCR,        0x03);  // 8N1
    NV_UARTD_WRITE(MCR,        0x02);
    NV_UARTD_WRITE(MSR,        0x00);
    NV_UARTD_WRITE(SPR,        0x00);
    NV_UARTD_WRITE(IRDA_CSR,   0x00);
    NV_UARTD_WRITE(ASR,        0x00);

    NV_UARTD_WRITE(IIR_FCR,    0x31);

    // Flush any old characters out of the RX FIFO.
    while (NvBlUartRxReadyD())
        (void)NvBlUartRxD();
}

int drv_keyboard_init(void)
{
#ifdef CONFIG_TEGRA2_KEYBOARD
	tegra_kbc_init();
#endif
}
