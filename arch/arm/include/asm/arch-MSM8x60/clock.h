/*
 * Copyright (c) 2007-2009, Code Aurora Forum. All rights reserved.
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

#ifndef _CLOCK_H_
#define _CLOCK_H_

#include <common.h>

/*SDC related*/
extern void set_sdcard_clk(int instance, int rate);
extern void disable_sdcard_clk(int instance);
extern void mmc_set_clk(unsigned int ns, unsigned int md,int instance);

#define SDCn_HCLK_CTL(x) (CLK_CTL_BASE + 0x2820 + (0x20 * (x-1)) )
#define SDCn_HCLK_FS(x)  (CLK_CTL_BASE + 0x2824 + (0x20 * (x-1)) )
#define CLK_BRANCH_ENA (0x10)
#define FORCE_C_ON (0x40)
#define S_W_VAL_15 (0x15)

// SD MCLK definitions
#define MCLK_MD_400KHZ    0x0001000F
#define MCLK_NS_400KHZ    0x0010005B

#define MCLK_MD_16MHZ     0x000100F9
#define MCLK_NS_16MHZ     0x00FA005B

#define MCLK_MD_17MHZ     0x000200D2
#define MCLK_NS_17MHZ     0x00D40043

#define MCLK_MD_20MHZ     0x000100EC
#define MCLK_NS_20MHZ     0x00ED0043

#define MCLK_MD_24MHZ     0x000100FB
#define MCLK_NS_24MHZ     0x00FC005B

#define MCLK_MD_48MHZ     0x000100FD
#define MCLK_NS_48MHZ     0x00FE005B

#endif /* __CLOCK_H_ */

