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

#include "nvboot_clocks_int.h"
#include "nvboot_util.h"
      
// set of initialization values for usec counter */
// there are no define for the values themselves, only comment in the spec file
// the order must be the order of the NvBootClocksOscFreq enum
//
//    osc clock freq.   dividend/divisor        USEC_DIVIDEND/USEC_DIVISOR
//    --------------------------------------------------------------------
//        13MHz                1/13                     0x00 / 0x0c
//        19.2MHz              5/96                     0x04 / 0x5f
//        12MHz                1/12                     0x00 / 0x0b
//        26MHz                1/26                     0x00 / 0x19           
//
static const NvU32 s_UsecCfgTable[(int) NvBootClocksOscFreq_Num] =
{
    // 13MHz
    NV_DRF_NUM(TIMERUS_USEC, CFG, USEC_DIVIDEND, (1-1)) |
    NV_DRF_NUM(TIMERUS_USEC, CFG, USEC_DIVISOR, (13-1)),

    // 19.2MHz
    NV_DRF_NUM(TIMERUS_USEC, CFG, USEC_DIVIDEND, (5-1)) |
    NV_DRF_NUM(TIMERUS_USEC, CFG, USEC_DIVISOR, (96-1)),

    // 12 MHz
    NV_DRF_NUM(TIMERUS_USEC, CFG, USEC_DIVIDEND, (1-1)) |
    NV_DRF_NUM(TIMERUS_USEC, CFG, USEC_DIVISOR, (12-1)),

    // 26MHz
    NV_DRF_NUM(TIMERUS_USEC, CFG, USEC_DIVIDEND, (1-1)) |
    NV_DRF_NUM(TIMERUS_USEC, CFG, USEC_DIVISOR, (26-1))
};

// Put PLL in bypass to insure an active clock (CYA)
void
NvBootClocksBypassPll(NvBootClocksPllId PllId)
{
    NvU32 RegData;

    NVBOOT_CLOCKS_CHECK_PLLID(PllId);

    RegData = NV_READ32(NV_ADDRESS_MAP_CAR_BASE +
                        NVBOOT_CLOCKS_PLL_BASE(PllId));

    RegData = NV_FLD_SET_DRF_DEF(CLK_RST_CONTROLLER,
                                 PLLP_BASE,
                                 PLLP_BYPASS,
                                 ENABLE,
                                 RegData);

    NV_WRITE32(NV_ADDRESS_MAP_CAR_BASE + NVBOOT_CLOCKS_PLL_BASE(PllId),
               RegData);
}

// Check for stability, i.e. is current time after expected stable time
NvBool
NvBootClocksIsPllStable(NvU32 StableTime)
{
    NvU32 DeltaTime;

    DeltaTime = NV_READ32(NV_ADDRESS_MAP_TMRUS_BASE + TIMERUS_CNTR_1US_0) - 
        StableTime;

    if(DeltaTime &(1U << 31))
    {
        // sign bit set, so DeltaTime is negative, not ready yet 
        return NV_FALSE;
    }
    return NV_TRUE;
}


// Stop PLL, will shut off the output if PLL was not in bypass before that
void
NvBootClocksStopPll(NvBootClocksPllId PllId) {
    
    NvU32 RegData;

    NVBOOT_CLOCKS_CHECK_PLLID(PllId);

    // again relying on common format
    RegData = NV_DRF_DEF(CLK_RST_CONTROLLER, PLLC_BASE, PLLC_ENABLE, DISABLE);
    NV_WRITE32(NV_ADDRESS_MAP_CAR_BASE + NVBOOT_CLOCKS_PLL_BASE(PllId),
               RegData);
}

// Configure the clock source and divider
void
NvBootClocksConfigureClock(NvBootClocksClockId ClockId, 
                           NvU32 Divider,
                           NvU32 Source ) {

    NvU32 RegOffset;
    NvU32 RegData;

    NVBOOT_CLOCKS_CHECK_CLOCKID(ClockId);

    // Process source first, starting with the standard one
    RegOffset = NVBOOT_CLOCKS_SOURCE_OFFSET(ClockId);
    if(RegOffset != 0)
    {
        // we do abuse a little bit, always using the 16 bit format for the
        // divider, extra bits dropped by HW when needed, this is why we use
        // I2C1 as a template
        RegData = NV_DRF_NUM(CLK_RST_CONTROLLER,
                             CLK_SOURCE_I2C1,
                             I2C1_CLK_SRC,
                             Source) |
                  NV_DRF_NUM(CLK_RST_CONTROLLER,
                             CLK_SOURCE_I2C1,
                             I2C1_CLK_DIVISOR,
                             Divider);

        NV_WRITE32(NV_ADDRESS_MAP_CAR_BASE + RegOffset, RegData);
    } 
    else
    { 
        // we need to work more
        switch(ClockId)
        {
        case NvBootClocksClockId_SclkId:  
            RegData = NV_DRF_DEF(CLK_RST_CONTROLLER,
                                 SCLK_BURST_POLICY,
                                 SYS_STATE,
                                 RUN) |
                      NV_DRF_NUM(CLK_RST_CONTROLLER,
                                 SCLK_BURST_POLICY,
                                 SWAKEUP_RUN_SOURCE,
                                 Source);
            NV_WRITE32(NV_ADDRESS_MAP_CAR_BASE +
                       CLK_RST_CONTROLLER_SCLK_BURST_POLICY_0,
                       RegData);

            // we reprogram PLLP_OUT4, better be that the source, otherwise
            // no effect, RMW here 
            NV_READ32_(NV_ADDRESS_MAP_CAR_BASE+CLK_RST_CONTROLLER_PLLP_OUTB_0, RegData);
            RegData = NV_FLD_SET_DRF_NUM(CLK_RST_CONTROLLER,
                                         PLLP_OUTB,
                                         PLLP_OUT4_RATIO,
                                         Divider,
                                         RegData);
            RegData = NV_FLD_SET_DRF_DEF(CLK_RST_CONTROLLER,
                                         PLLP_OUTB,
                                         PLLP_OUT4_OVRRIDE,
                                         ENABLE,
                                         RegData);
            NV_WRITE32(NV_ADDRESS_MAP_CAR_BASE +
                       CLK_RST_CONTROLLER_PLLP_OUTB_0,
                       RegData);
            break;
            
        case NvBootClocksClockId_CclkId :
            RegData = NV_DRF_DEF(CLK_RST_CONTROLLER,
                                 CCLK_BURST_POLICY,
                                 CPU_STATE,
                                 RUN) |
                      NV_DRF_NUM(CLK_RST_CONTROLLER, 
                                 CCLK_BURST_POLICY,
                                 CWAKEUP_RUN_SOURCE,
                                 Source);
            NV_WRITE32(NV_ADDRESS_MAP_CAR_BASE +
                       CLK_RST_CONTROLLER_CCLK_BURST_POLICY_0,
                       RegData);

            RegData = NV_DRF_DEF(CLK_RST_CONTROLLER,
                                 SUPER_CCLK_DIVIDER,
                                 SUPER_CDIV_ENB,
                                 ENABLE) |
                      NV_DRF_NUM(CLK_RST_CONTROLLER,
                                 SUPER_CCLK_DIVIDER,
                                 SUPER_CDIV_DIVIDEND,
                                 (1- 1)) | 
                      NV_DRF_NUM(CLK_RST_CONTROLLER,
                                 SUPER_CCLK_DIVIDER,
                                 SUPER_CDIV_DIVISOR,
                                 Divider);
            NV_WRITE32(NV_ADDRESS_MAP_CAR_BASE + 
                       CLK_RST_CONTROLLER_SUPER_CCLK_DIVIDER_0,
                       RegData);
            break;
            
        case NvBootClocksClockId_HclkId : /* cannot change the source */
            NV_READ32_(NV_ADDRESS_MAP_CAR_BASE+CLK_RST_CONTROLLER_CLK_SYSTEM_RATE_0, RegData);
            RegData = NV_FLD_SET_DRF_NUM(CLK_RST_CONTROLLER,
                                         CLK_SYSTEM_RATE,
                                         AHB_RATE,
                                         Divider,
                                         RegData);
            NV_WRITE32(NV_ADDRESS_MAP_CAR_BASE +
                       CLK_RST_CONTROLLER_CLK_SYSTEM_RATE_0,
                       RegData);
            break;

        case NvBootClocksClockId_PclkId : /* cannot change the source */
            RegData = NV_READ32(NV_ADDRESS_MAP_CAR_BASE +
                                CLK_RST_CONTROLLER_CLK_SYSTEM_RATE_0);
            RegData = NV_FLD_SET_DRF_NUM(CLK_RST_CONTROLLER,
                                         CLK_SYSTEM_RATE,
                                         APB_RATE,
                                         Divider,
                                         RegData);
            NV_WRITE32(NV_ADDRESS_MAP_CAR_BASE +
                       CLK_RST_CONTROLLER_CLK_SYSTEM_RATE_0,
                       RegData);
            break;

        case NvBootClocksClockId_EmcId :
            // special mux type and two clock enable present in the source
            // for 1x and 2x
            RegData = NV_READ32(NV_ADDRESS_MAP_CAR_BASE +
                                CLK_RST_CONTROLLER_CLK_SOURCE_EMC_0);
            RegData = NV_FLD_SET_DRF_NUM(CLK_RST_CONTROLLER,
                                         CLK_SOURCE_EMC,
                                         EMC_2X_CLK_SRC,
                                         Source,
                                         RegData);
            RegData = NV_FLD_SET_DRF_NUM(CLK_RST_CONTROLLER,
                                         CLK_SOURCE_EMC,
                                         EMC_2X_CLK_DIVISOR,
                                         Divider,
                                         RegData);
            NV_WRITE32(NV_ADDRESS_MAP_CAR_BASE +
                       CLK_RST_CONTROLLER_CLK_SOURCE_EMC_0,
                       RegData); 
            break; 

        default :
            // nothing for other enums here, make that clear to the compiler
            break;
        };
    };
    NvBootUtilWaitUS(NVBOOT_CLOCKS_CLOCK_STABILIZATION_TIME);

    // jz
#if 0
    if(RegOffset != 0)
    {
        NV_READ32_(NV_ADDRESS_MAP_CAR_BASE + RegOffset, RegData);
    };
#endif
}

// Set the AVP clock to 108MHz, called before scatter loading so
// must not rely on any non const static, stack variables OK, static const
// variables OK.  Assumes PLLP is stable.
void
NvBootClocksSetAvpClockBeforeScatterLoad()
{
    NvU32 RegData;

    // switch system clock to PLLP_out 4(108 MHz) MHz, AVP will now run
    // at 108 MHz.  This is glitch free as only the source is changed,
    // no special precaution needed
    RegData = NV_DRF_DEF(CLK_RST_CONTROLLER,
                         SCLK_BURST_POLICY,
                         SYS_STATE,
                         RUN) |
              NV_DRF_DEF(CLK_RST_CONTROLLER,
                         SCLK_BURST_POLICY, 
                         SWAKEUP_RUN_SOURCE,
                         PLLP_OUT4);
    NV_WRITE32(NV_ADDRESS_MAP_CAR_BASE +
               CLK_RST_CONTROLLER_SCLK_BURST_POLICY_0,
               RegData);
}

// Start PLLP and configure microsecond timer, called before scatter loading so
// must not rely on any non const static, stack variables OK, static const
// variables OK
void
NvBootClocksStartPllpBeforeScatterLoad(NvBootClocksOscFreq OscFreq)
{
    // The purpose of this is to put the system clock on PLLP / 4 to
    // accelerate as fast as possible the C library functions that build
    // the C runtime environment.
    // This implies restrictions on what can be done here, especially any
    // global dynamic variables should be avoided, they would be
    // overwritten during the scatter loading

    NvU32 RegData;

    NV_ASSERT((NvU32) OscFreq <(NvU32) NvBootClocksOscFreq_Num);

    // Now start PLLP and wait for the stabilization time, writing MISC
    // then BASE only CPCON is configuration dependent and not zero in MISC
    RegData = NV_DRF_NUM(CLK_RST_CONTROLLER,
                         PLLP_MISC,
                         PLLP_CPCON, 
                         (OscFreq == NvBootClocksOscFreq_19_2) ?
                         NVBOOT_CLOCKS_PLLP_CPCON_19_2 :
                         NVBOOT_CLOCKS_PLLP_CPCON_DEFAULT);
    NV_WRITE32(NV_ADDRESS_MAP_CAR_BASE + CLK_RST_CONTROLLER_PLLP_MISC_0,
               RegData);

    // BASE, only enable
    RegData = CLK_RST_CONTROLLER_PLLP_BASE_0_RESET_VAL |
              NV_DRF_DEF(CLK_RST_CONTROLLER, PLLP_BASE, PLLP_ENABLE, ENABLE);
    NV_WRITE32(NV_ADDRESS_MAP_CAR_BASE + CLK_RST_CONTROLLER_PLLP_BASE_0,
               RegData);
   
    // wait until stable
    NvBootUtilWaitUS(NVBOOT_CLOCKS_PLL_STABILIZATION_DELAY);
}

// Get the oscillator frequency, from the corresponding HW configuration field
NvBootClocksOscFreq 
NvBootClocksGetOscFreq(void)
{
    NvU32 RegData;
    NV_READ32_(NV_ADDRESS_MAP_CAR_BASE + 
                        CLK_RST_CONTROLLER_OSC_CTRL_0, RegData);
    return (NvBootClocksOscFreq) NV_DRF_VAL(CLK_RST_CONTROLLER,
                                            OSC_CTRL,
                                            OSC_FREQ,
                                            RegData); 
}

//  Start PLL using the provided configuration parameters
void
NvBootClocksStartPll(NvBootClocksPllId PllId,
                     NvU32 M,
                     NvU32 N,
                     NvU32 P,
                     NvU32 CPCON,
                     NvU32 LFCON,
                     NvU32 *StableTime ) {
    NvU32 RegData;

    NVBOOT_CLOCKS_CHECK_PLLID(PllId);
    NV_ASSERT (StableTime != NULL);

    // we cheat by treating all PLL (except PLLU) in the same fashion
    // this works only because
    // - same fields are always mapped at same offsets, except DCCON
    // - DCCON is always 0, doesn't conflict
    // - M,N, P of PLLP values are ignored for PLLP

    if (PllId == NvBootClocksPllId_PllU)
    {
        RegData = NV_DRF_NUM(CLK_RST_CONTROLLER,
                             PLLU_MISC,
                             PLLU_CPCON,
                             CPCON) |
                  NV_DRF_NUM(CLK_RST_CONTROLLER,
                             PLLU_MISC,
                             PLLU_LFCON,
                             LFCON);
        NV_WRITE32(NV_ADDRESS_MAP_CAR_BASE + NVBOOT_CLOCKS_PLL_MISC(PllId),
                   RegData);
        
        RegData = NV_DRF_NUM(CLK_RST_CONTROLLER, PLLU_BASE, PLLU_DIVM, M)     |
                  NV_DRF_NUM(CLK_RST_CONTROLLER, PLLU_BASE, PLLU_DIVN, N)     |
                  NV_DRF_NUM(CLK_RST_CONTROLLER, PLLU_BASE, PLLU_VCO_FREQ, P) |
                  NV_DRF_DEF(CLK_RST_CONTROLLER, PLLU_BASE, PLLU_BYPASS,
                             DISABLE)                                         |
                  NV_DRF_DEF(CLK_RST_CONTROLLER, PLLU_BASE, PLLU_ENABLE,
                             ENABLE);        
        NV_WRITE32(NV_ADDRESS_MAP_CAR_BASE + NVBOOT_CLOCKS_PLL_BASE(PllId),
                   RegData);
    }
    else 
    {
        RegData = NV_DRF_NUM(CLK_RST_CONTROLLER, PLLC_MISC, PLLC_CPCON,
                             CPCON) |
                  NV_DRF_NUM(CLK_RST_CONTROLLER, PLLC_MISC, PLLC_LFCON, LFCON);
        NV_WRITE32(NV_ADDRESS_MAP_CAR_BASE + NVBOOT_CLOCKS_PLL_MISC(PllId),
                   RegData);

        RegData = NV_DRF_NUM(CLK_RST_CONTROLLER, PLLC_BASE, PLLC_DIVP, P) |
                  NV_DRF_NUM(CLK_RST_CONTROLLER, PLLC_BASE, PLLC_DIVM, M) |
                  NV_DRF_NUM(CLK_RST_CONTROLLER, PLLC_BASE, PLLC_DIVN, N) |
                  NV_DRF_DEF(CLK_RST_CONTROLLER, PLLC_BASE, PLLC_BYPASS,
                             DISABLE) |
                  NV_DRF_DEF(CLK_RST_CONTROLLER, PLLC_BASE, PLLC_ENABLE,
                             ENABLE);        
        NV_WRITE32(NV_ADDRESS_MAP_CAR_BASE + NVBOOT_CLOCKS_PLL_BASE(PllId),
                   RegData);
    }
    // calculate the stable time
    NV_READ32_(NV_ADDRESS_MAP_TMRUS_BASE + TIMERUS_CNTR_1US_0, *StableTime);
    *StableTime += NVBOOT_CLOCKS_PLL_STABILIZATION_DELAY;
}

// Enable the clock, this corresponds generally to level 1 clock gating
void
NvBootClocksSetEnable(NvBootClocksClockId ClockId, NvBool Enable) {
    NvU32 RegData;
    NvU8  BitOffset;
    NvU8  RegOffset;

    NVBOOT_CLOCKS_CHECK_CLOCKID(ClockId);
    NV_ASSERT(((int) Enable == 0) ||((int) Enable == 1));

    // The simplest case is via bits in register ENB_CLK
    // But there are also special cases 
    if(NVBOOT_CLOCKS_HAS_STANDARD_ENB(ClockId))
    {
        // there is a CLK_ENB bit to kick
        BitOffset = NVBOOT_CLOCKS_BIT_OFFSET(ClockId);
        RegOffset = NVBOOT_CLOCKS_REG_OFFSET(ClockId);
        NV_READ32_(NV_ADDRESS_MAP_CAR_BASE +
                  CLK_RST_CONTROLLER_CLK_OUT_ENB_L_0 +
                  RegOffset, RegData);
#if 0
printf("ClkEna: R reg: 0x%08x, Curr: 0x%08x, Off 0x%x, Shift %d, Ena: %d\n",
                              NV_ADDRESS_MAP_CAR_BASE +
                              CLK_RST_CONTROLLER_CLK_OUT_ENB_L_0 +
                              RegOffset, RegData, RegOffset, BitOffset, Enable);
#endif

        /* no simple way to use the access macro in this case */
        if(Enable)
        {
            RegData |=  (1 << BitOffset);
        }
        else 
        {
            RegData &= ~(1 << BitOffset);
        }
#if 0
printf("ClkEna: W reg: 0x%08x, val: 0x%08x\n",
                              NV_ADDRESS_MAP_CAR_BASE +
                              CLK_RST_CONTROLLER_CLK_OUT_ENB_L_0 +
                              RegOffset, RegData);
#endif
 
        NV_WRITE32(NV_ADDRESS_MAP_CAR_BASE +
                   CLK_RST_CONTROLLER_CLK_OUT_ENB_L_0 +
                   RegOffset,
                   RegData);

        // EMC is also special with two enable bits x1 and x2 in the source register
        switch(ClockId)
        {
        case NvBootClocksClockId_EmcId:
            RegData = NV_READ32(NV_ADDRESS_MAP_CAR_BASE +
                                CLK_RST_CONTROLLER_CLK_SOURCE_EMC_0);
            if(Enable)
            {
                RegData = NV_FLD_SET_DRF_DEF(CLK_RST_CONTROLLER,
                                             CLK_SOURCE_EMC,
                                             EMC_2X_CLK_ENB,
                                             ENABLE,
                                             RegData);
                RegData = NV_FLD_SET_DRF_DEF(CLK_RST_CONTROLLER,
                                             CLK_SOURCE_EMC,
                                             EMC_1X_CLK_ENB,
                                             ENABLE,
                                             RegData);
            }
            else
            {
                RegData = NV_FLD_SET_DRF_DEF(CLK_RST_CONTROLLER,
                                             CLK_SOURCE_EMC,
                                             EMC_2X_CLK_ENB,
                                             DISABLE,
                                             RegData);
                RegData = NV_FLD_SET_DRF_DEF(CLK_RST_CONTROLLER,
                                             CLK_SOURCE_EMC,
                                             EMC_1X_CLK_ENB,
                                             DISABLE,
                                             RegData);
            }
            NV_WRITE32(NV_ADDRESS_MAP_CAR_BASE +
                       CLK_RST_CONTROLLER_CLK_SOURCE_EMC_0,
                       RegData); 
            break;

        default:
            // nothing here for other enums, make that explicit to the compiler
            break;
        }
    } 
    else
    {
        // there is no bit in CLK_ENB, less regular processing needed
        switch(ClockId)
        {
        case NvBootClocksClockId_SclkId:
            // there is no way to stop Sclk, for documentation purpose
            break;

        case NvBootClocksClockId_HclkId:
            NV_READ32_(NV_ADDRESS_MAP_CAR_BASE +
                                CLK_RST_CONTROLLER_CLK_SYSTEM_RATE_0, RegData);
            RegData = NV_FLD_SET_DRF_NUM(CLK_RST_CONTROLLER,
                                         CLK_SYSTEM_RATE,
                                         HCLK_DIS,
                                         Enable,
                                         RegData);
            NV_WRITE32(NV_ADDRESS_MAP_CAR_BASE +
                       CLK_RST_CONTROLLER_CLK_SYSTEM_RATE_0,
                       RegData);
            break;

        case NvBootClocksClockId_PclkId:
            NV_READ32_(NV_ADDRESS_MAP_CAR_BASE +
                                CLK_RST_CONTROLLER_CLK_SYSTEM_RATE_0, RegData);
            RegData = NV_FLD_SET_DRF_NUM(CLK_RST_CONTROLLER,
                                         CLK_SYSTEM_RATE,
                                         PCLK_DIS,
                                         Enable,
                                         RegData);
            NV_WRITE32(NV_ADDRESS_MAP_CAR_BASE +
                       CLK_RST_CONTROLLER_CLK_SYSTEM_RATE_0,
                       RegData);
            break;

        default :
            // nothing for other enums, make that explicit for compiler
            break;
        };
    };

    NvBootUtilWaitUS(NVBOOT_CLOCKS_CLOCK_STABILIZATION_TIME);
}

void
NvBootClocksSetOscFreq(NvBootClocksOscFreq OscFreq)
{
    NvU32 RegData;

    NV_READ32_(NV_ADDRESS_MAP_CAR_BASE +
                        CLK_RST_CONTROLLER_OSC_CTRL_0, RegData);

    RegData = NV_FLD_SET_DRF_NUM(CLK_RST_CONTROLLER,
                                 OSC_CTRL,
                                 OSC_FREQ,
                                (int)OscFreq,
                                 RegData);

    NV_WRITE32(NV_ADDRESS_MAP_CAR_BASE + CLK_RST_CONTROLLER_OSC_CTRL_0,
               RegData);
}


NvBootClocksOscFreq
NvBootClocksMeasureOscFreq(void)
{
    NvU32 Cnt;

#if NVBOOT_TARGET_RTL && NVBOOT_SKIP_OSC_FREQ_IN_RTL
    Cnt = NVBOOT_CLOCKS_MIN_CNT_13;
#else
    NvU32 RegData;
    // start measurement, window size uses n-1 coding
    NV_WRITE32(NV_ADDRESS_MAP_CAR_BASE + CLK_RST_CONTROLLER_OSC_FREQ_DET_0, 
                NV_DRF_DEF(CLK_RST_CONTROLLER,
                           OSC_FREQ_DET,
                           OSC_FREQ_DET_TRIG,
                           ENABLE) |
                NV_DRF_NUM(CLK_RST_CONTROLLER,
                           OSC_FREQ_DET,
                           REF_CLK_WIN_CFG,
                           (1-1)));

    // wait until the measurement is done
    do
    {
        NV_READ32_(NV_ADDRESS_MAP_CAR_BASE +
                            CLK_RST_CONTROLLER_OSC_FREQ_DET_STATUS_0, RegData);
    } 
    while(NV_DRF_VAL(CLK_RST_CONTROLLER,
                     OSC_FREQ_DET_STATUS,
                     OSC_FREQ_DET_BUSY,
                     RegData));

    Cnt = NV_DRF_VAL(CLK_RST_CONTROLLER,
                     OSC_FREQ_DET_STATUS,
                     OSC_FREQ_DET_CNT,
                     RegData);  
#endif  

    if((Cnt >= NVBOOT_CLOCKS_MIN_CNT_12) &&
       (Cnt <= NVBOOT_CLOCKS_MAX_CNT_12))
    {
        return NvBootClocksOscFreq_12;
    } 
    if((Cnt >= NVBOOT_CLOCKS_MIN_CNT_13) &&
       (Cnt <= NVBOOT_CLOCKS_MAX_CNT_13))
    {
        return NvBootClocksOscFreq_13;
    } 
    if((Cnt >= NVBOOT_CLOCKS_MIN_CNT_19_2) &&
       (Cnt <= NVBOOT_CLOCKS_MAX_CNT_19_2))
    {
        return NvBootClocksOscFreq_19_2;
    } 
    if((Cnt >= NVBOOT_CLOCKS_MIN_CNT_26) &&
       (Cnt <= NVBOOT_CLOCKS_MAX_CNT_26))
    {
        return NvBootClocksOscFreq_26;    
    } 
    return NvBootClocksOscFreq_Unknown;
}

void
NvBootClocksConfigureUsecTimer(NvBootClocksOscFreq OscFreq)
{
    NvU32 UsecCfg;

    if(OscFreq == NvBootClocksOscFreq_Unknown)
    {
        // Note: This case should never happen.
        // Discussion has raged about how to handle this case.  The final
        // consensus was to force operation as if the oscillator
        // frequency was 26 MHz. Rejected (but somehow reluctantly)
        // was an alternate proposal to calculate
        // approximately correct M,N,P based on the measured frequency.
        // We do remember the error by returning an error code (but
        // currently dropped)
        UsecCfg = s_UsecCfgTable[(NvU32) NvBootClocksOscFreq_26];
        OscFreq = NvBootClocksOscFreq_26;
    }
    else
    {
        UsecCfg = s_UsecCfgTable[(NvU32) OscFreq];
    }
    NV_WRITE32(NV_ADDRESS_MAP_TMRUS_BASE + TIMERUS_USEC_CFG_0, UsecCfg);
}
