/*
 * Copyright (c) 2010, Code Aurora Forum. All rights reserved.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <common.h>
#include <asm/arch/clock.h>
#include <qsd_sdcc.h>

void set_sdcard_clk(int instance, int rate)
{
    uint32_t md = 0;
    uint32_t ns = 0;
    switch (rate)
    {
    case MCLK_400KHz :
       md = MCLK_MD_400KHZ;
       ns = MCLK_NS_400KHZ;
       break;
    case MCLK_16MHz :
       md = MCLK_MD_16MHZ;
       ns = MCLK_NS_16MHZ;
       break;
    case MCLK_17MHz :
       md = MCLK_MD_17MHZ;
       ns = MCLK_NS_17MHZ;
       break;
    case MCLK_20MHz :
       md = MCLK_MD_20MHZ;
       ns = MCLK_NS_20MHZ;
       break;
    case MCLK_24MHz :
       md = MCLK_MD_24MHZ;
       ns = MCLK_NS_24MHZ;
       break;
    case MCLK_48MHz :
       md = MCLK_MD_48MHZ;
       ns = MCLK_NS_48MHZ;
       break;
    default:
       printf("Unsupported Speed\n");
       return;
    }
    printf("Changing clock speed to %d Hz.\n", rate);
    mmc_set_clk(ns, md, instance);
    IO_WRITE32(SDCn_HCLK_CTL(instance),CLK_BRANCH_ENA);
    IO_WRITE32(SDCn_HCLK_FS(instance),FORCE_C_ON | S_W_VAL_15);
}

void mmc_set_clk(unsigned int ns, unsigned int md,int instance)
{
    unsigned int val;
    /*Clock Init*/
    // 1. Set bit 7 in the NS registers
    val = 1 << 7;
    IO_WRITE32(SDC_NS(instance),val);

    //2. Program MD registers
    IO_WRITE32(SDC_MD(instance),md);

    //3. Program NS resgister OR'd with Bit 7
    val = 1 << 7;
    val |= ns;
    IO_WRITE32(SDC_NS(instance),val);

    //4. Clear bit 7 of NS register
    val = 1 << 7;
    val = ~val;
    val = val & IO_READ32(SDC_NS(instance));
    IO_WRITE32(SDC_NS(instance),val);

    //5. For MD != NA set bit 8 of NS register
    val = 1 << 8;
    val = val | IO_READ32(SDC_NS(instance));
    IO_WRITE32(SDC_NS(instance),val);

    //6. Set bit 11 in NS register
    val = 1 << 11;
    val = val | IO_READ32(SDC_NS(instance));
    IO_WRITE32(SDC_NS(instance),val);

    //7. Set bit 9 in NS register
    val = 1 << 9;
    val = val | IO_READ32(SDC_NS(instance));
    IO_WRITE32(SDC_NS(instance),val);
}
