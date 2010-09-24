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
#include <asm/arch/nv_drf.h>
#include <asm/arch/nvcommon.h>
#include <asm/arch/tegra2.h>
#include <asm/arch/nv_hardware_access.h>

#include "nvboot_pinmux_local.h"

const NvU32 g_Ap20Mux_Sflash[] =
{
    CONFIGEND(),
    CONFIG(B,C,GMD,SFLASH),CONFIG(A,B,GMC,SFLASH),CONFIGEND(),
    MODULEDONE(),
};

const NvU32 g_Ap20Mux_Sdio4[] =
{
    CONFIGEND(),

    // config 1 - x4 or x8 on one set of pins
    CONFIG(A,A,ATC,SDIO4),CONFIG(A,A,ATD,SDIO4),CONFIGEND(),

    // config 2 - x8 on 2nd set of pins
    BRANCH(3),CONFIG(B,D,GME,SDIO4),CONFIGEND(),

    // config 3 - x4 on 2nd set of pins
    CONFIG(A,A,ATB,SDIO4),CONFIG(A,B,GMA,SDIO4),CONFIGEND(),

    MODULEDONE(),
    SUBROUTINESDONE(),
};

const NvU32 g_Ap20Mux_Nand[] =
{
    CONFIGEND(),

    // config 1 - x16, standard pins
    BRANCH(2), CONFIG(A,A,ATD,NAND),CONFIG(B,A,ATE,NAND),CONFIGEND(),

    // config 2 - x8, standard pins
    CONFIG(A,A,ATA,NAND),CONFIG(A,A,ATB,NAND),CONFIG(A,A,ATC,NAND),
    CONFIGEND(),

    // config 3 - alternate pins
    CONFIG(A,C,KBCA,NAND),CONFIG(A,C,KBCB,NAND),CONFIG(B,C,KBCC,NAND),
    CONFIG(D,G,KBCD,NAND),CONFIG(A,A,KBCE,NAND),CONFIG(A,A,KBCF,NAND),
    CONFIGEND(),

    MODULEDONE(),
};

const NvU32 g_Ap20Mux_Snor[] =
{
    CONFIGEND(),
    // config 1. separate 32b NOR
    BRANCH(2),
    BRANCH(8),
    CONFIGEND(),
    // config 2. muxed 32b NOR
    BRANCH(3),
    CONFIG(A,B,GMC,GMI),CONFIG(A,B,GMA,GMI), CONFIG(B,D,GME,GMI),
    CONFIG(A,C,DAP1,GMI), CONFIGEND(),
    // config 3. muxed 16b NOR
    BRANCH(7),
    CONFIG(B,C,GMB,GMI),CONFIGEND(),
    // config 4. separate 16b NOR
    BRANCH(3),
    BRANCH(8),
    CONFIGEND(),
    // config 5. MuxOneNAND (muxed x16)
    BRANCH(7),
    CONFIG(B,C,GMB,GMI_INT),CONFIG(A,B,GMC,SFLASH),CONFIGEND(),
    // config 6. OneNAND (separate x16)
    BRANCH(5),
    BRANCH(8),
    CONFIGEND(),
    MODULEDONE(),
    // subroutine 1 (config 7).  shared by 16b muxed NOR & muxOneNand
    CONFIG(A,C,DAP4,GMI),CONFIG(A,C,DAP2,GMI),CONFIG(B,D,SPIA,GMI),
    CONFIG(B,D,SPIB,GMI),CONFIG(B,D,SPIC,GMI),CONFIG(B,D,SPID,GMI),
    CONFIG(B,D,SPIE,GMI),
    CONFIG(A,A,ATA,GMI),CONFIG(A,A,ATB,GMI),CONFIG(A,A,ATC,GMI),
    CONFIG(A,A,ATD,GMI),
    CONFIG(B,A,ATE,GMI),CONFIG(B,C,GMD,GMI),CONFIGEND(),
    // subroutine 2 (config 8). extra addr. pins for non-muxed configs
    CONFIG(A,C,IRRX,GMI),CONFIG(A,C,IRTX,GMI),CONFIG(B,B,UCA,GMI),
    CONFIG(B,B,UCB,GMI),CONFIG(A,D,GPU,GMI),CONFIGEND(),

    SUBROUTINESDONE(),
};

const NvU32* g_Ap20MuxControllers[] =
{
    &g_Ap20Mux_Sdio4[0],  // SDMMC
    &g_Ap20Mux_Snor[0],   // SNOR
    &g_Ap20Mux_Sflash[0], // SPI NOR
    &g_Ap20Mux_Nand[0],   // NAND
    &g_Ap20Mux_Nand[0],   // mobileLBA NAND
    &g_Ap20Mux_Snor[0],   // MuxOneNAND
};
