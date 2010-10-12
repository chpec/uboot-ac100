/*
 * Copyright (c) 2009-2010, Code Aurora Forum. All rights reserved.
 *
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
 *
 * (C) Copyright 2002
 * David Mueller, ELSOFT AG, <d.mueller@elsoft.ch>
 *
 * (C) Copyright 2003
 * Texas Instruments, <www.ti.com>
 * Kshitij Gupta <Kshitij@ti.com>
 *
 * (C) Copyright 2004
 * ARM Ltd.
 * Philippe Robin, <philippe.robin@arm.com>
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
/******************************
 Routine: scorpion_pll_init
 Description: Starts up Scorpion PLL and switches Scorpion clock from
              the AXI clock to the PLL.
******************************/
void scorpion_pll_init (void)
{
#ifdef CPU_IS_QSD8x50A
    // PLL to standby
    IO_WRITE32(PLL_CTL, PLL_CTL__PLL_MODE__STAND_BY);
    udelay(10);

    // Calibrate for 384-1497 MHz
    IO_WRITE32(PLL_CAL, 0x270A0000);
    IO_WRITE32(PLL_CTL, PLL_CTL__PLL_MODE__FULL_CALIBRATION);
    udelay(10);

    // Wait for CAL_ALL_DONE_N to go from 1 to 0
    while((IO_READ32(PLL_STATUS) & PLL_STATUS__CAL_ALL_DONE_N___M) != 0);

    // SHOT to start PLL at 1190.4MHz (31 * 38.4MHz)
    IO_WRITE32(PLL_FSM_CTL_EXT, PLL_FSM_CTL_EXT__STATIC_BITS |
                                PLL_FSM_CTL_EXT__FRESWI_MODE__SHOT |
                                (31 << PLL_FSM_CTL_EXT__TARG_L_VAL___S));
    IO_WRITE32(PLL_CTL, PLL_CTL__PLL_MODE__NORMAL_OPERATION);
    udelay(10);

    // Wait for all bits in SWITCH_IN_PROGRESS_N to go from 1 to 0
    while((IO_READ32(PLL_STATUS) & PLL_STATUS__SWITCH_IN_PROGRESS_N___M) != 0);

    // Switch Scorpion clocks source from AXI clock to Scorpion PLL
    IO_WRITE32(SPSS_CLK_SEL, 2);

#else /* CPU_IS_QSD8x50A */
    uint32_t spare2;
    int target_L_val;

    // This function assumes:
    //   - Scorpion voltage has been set high enough to support the
    //     the frequency set in the HOP step below.
    //   - The peripheral web clock has been enabled (PRPH_WEB_NS_REG)

    // Set defaults
    IO_WRITE32(PLL_TEST_CTL, 0x00000000);
    IO_WRITE32(PLL_INTERNAL, 0x00200155);

    // SCPLL to default/powerdown mode
    IO_WRITE32(PLL_CTL, PLL_CTL__PLL_MODE__POWER_DOWN);
    udelay(5);

    // PLL to standby
    IO_WRITE32(PLL_CTL, PLL_CTL__BYPASSNL___M  |
                        PLL_CTL__PLL_MODE__STAND_BY);
    udelay(5);

    // Calibrate the PLL
    IO_WRITE32(PLL_CTL, PLL_CTL__BYPASSNL___M |
                        PLL_CTL__RESET_N___M  |
                        PLL_CTL__PLL_MODE__FULL_CALIBRATION);
    udelay(5);

    // Wait for CAL_ALL_DONE_N to go from 1 to 0
    while((IO_READ32(PLL_STATUS) & PLL_STATUS__CAL_ALL_DONE_N___M) != 0);

    // Clean
    IO_WRITE32(PLL_CTL, PLL_CTL__PLL_MODE__POWER_DOWN);
    udelay(5);

    // Initial SHOT to start PLL at 384MHz (10 * 38.4MHz)
    IO_WRITE32(PLL_FSM_CTL_EXT, PLL_FSM_CTL_EXT__FRESWI_MODE__SHOT |
                               (10 << PLL_FSM_CTL_EXT__TARG_L_VAL___S));
    udelay(1200);

    // Normal mode
    IO_WRITE32(PLL_CTL, PLL_CTL__BYPASSNL___M |
                        PLL_CTL__RESET_N___M  |
                        PLL_CTL__PLL_MODE__NORMAL_OPERATION);
    udelay(20);

    // Fuse values are read by the ARM9 at boot and placed in the TCSR_SPARE2
    // register.  Bits 7:4 of the TCSR_SPARE2 contain the maximum clock
    // speed supported.
    spare2 = IO_READ32(TCSR_SPARE2);

    // The TCSR_SPARE2 value is valid if bits 15:12 (signature) are 0xA
    if ((spare2 & 0x0000F000) != 0x0000A000)
    {
        // Invalid signature field
        target_L_val = 26;        // 998.4 MHz
    }
    else
    {
        // Valid signature field
        spare2 = ((spare2 & 0x000000F0) >> 4);
        switch(spare2)
        {
        case 1:
            target_L_val = 33;   // 1267.2 MHz
            break;
        case 7:
            target_L_val = 20;   // 768 MHz
            break;
        default:
            target_L_val = 26;   // 998.4 MHz
        }
    }

    // HOP to maximum frequency
    IO_WRITE32(PLL_FSM_CTL_EXT, PLL_FSM_CTL_EXT__FRESWI_MODE__HOP |
                             (target_L_val << PLL_FSM_CTL_EXT__TARG_L_VAL___S));
    udelay(100);

    // Normal mode
    IO_WRITE32(PLL_CTL, PLL_CTL__BYPASSNL___M |
                        PLL_CTL__RESET_N___M  |
                        PLL_CTL__PLL_MODE__NORMAL_OPERATION);
    udelay(20);

    // Switch Scorpion clocks source from AXI clock to Scorpion PLL
    IO_WRITE32(SPSS_CLK_SEL, 2);

#endif /* CPU_IS_QSD8x50A */
}

