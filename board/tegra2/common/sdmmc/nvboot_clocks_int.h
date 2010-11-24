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

/*
 * nvboot_clocks_int.h - Declarations for clocks support.
 */

#ifndef INCLUDED_NVBOOT_CLOCKS_INT_H
#define INCLUDED_NVBOOT_CLOCKS_INT_H

#include <asm/arch/nvcommon.h>
#include <asm/arch/nvboot_clocks.h>
#include <asm/arch/nvboot_osc.h>

#if defined(__cplusplus)
extern "C"
{
#endif

// Set of clocks supported in the API
// The corresponding enum embeds some information on the register structure
// The lowest byte of the enum is unique per clock and built like this
// Bit 8 == 1: the clock is associated with a bit in register CLK_ENB, offset encoded in bits 6:0
//	       bit 6 == 1 for the _U registe
//             bit 5 == 1 for the _H register and 0 for the _L register
//             bit 4:0 is the offset in the register, that is the _SHIFT part
// Bit 8 == 0: the clock has no associated but in register CLK_ENB, bits 5:0 form a secondary ID
//             it also happens that none of these clocks has a source register in standard format
// The two MSB encode the offset of the source register if of standard format, 0 if not
#define NVBOOT_CLOCKS_STANDARD_ENB  (0x100)
#define NVBOOT_CLOCKS_ENB_BIT_OFFSET_MASK (0x1F)
#define NVBOOT_CLOCKS_H_REG (0x20)
#define NVBOOT_CLOCKS_U_REG (0x40)
#define NVBOOT_CLOCKS_SOURCE_SHIFT (16)
#define NVBOOT_CLOCKS_HAS_STANDARD_ENB(ClockId) (((NvU32) ClockId) & NVBOOT_CLOCKS_STANDARD_ENB)
#define NVBOOT_CLOCKS_BIT_OFFSET(ClockId)       (((NvU32) ClockId) & NVBOOT_CLOCKS_ENB_BIT_OFFSET_MASK)
#define NVBOOT_CLOCKS_REG_OFFSET(ClockId)     ( (((NvU32) ClockId) & (NVBOOT_CLOCKS_H_REG | NVBOOT_CLOCKS_U_REG)) >> 3 )
#define NVBOOT_CLOCKS_SOURCE_OFFSET(ClockId)  ( (((NvU32) ClockId) & ~((1<<NVBOOT_CLOCKS_SOURCE_SHIFT) -1))\
                                                 >> NVBOOT_CLOCKS_SOURCE_SHIFT)
typedef enum
{
    NvBootClocksClockId_SclkId   = 0x0,
    NvBootClocksClockId_HclkId,
    NvBootClocksClockId_PclkId,
    NvBootClocksClockId_CclkId   = CLK_RST_CONTROLLER_CLK_OUT_ENB_L_0_CLK_ENB_CPU_SHIFT
                                 + NVBOOT_CLOCKS_STANDARD_ENB,

    NvBootClocksClockId_UsbId    = CLK_RST_CONTROLLER_CLK_OUT_ENB_L_0_CLK_ENB_USBD_SHIFT
                                 + NVBOOT_CLOCKS_STANDARD_ENB,

    NvBootClocksClockId_I2cpId   = CLK_RST_CONTROLLER_CLK_OUT_ENB_H_0_CLK_ENB_DVC_I2C_SHIFT
                                 + NVBOOT_CLOCKS_STANDARD_ENB + NVBOOT_CLOCKS_H_REG
                                 + (CLK_RST_CONTROLLER_CLK_SOURCE_DVC_I2C_0 << NVBOOT_CLOCKS_SOURCE_SHIFT),

    NvBootClocksClockId_I2c1Id   = CLK_RST_CONTROLLER_CLK_OUT_ENB_L_0_CLK_ENB_I2C1_SHIFT
                                 + NVBOOT_CLOCKS_STANDARD_ENB
                                 + (CLK_RST_CONTROLLER_CLK_SOURCE_I2C1_0 << NVBOOT_CLOCKS_SOURCE_SHIFT),

    NvBootClocksClockId_I2c2Id   = CLK_RST_CONTROLLER_CLK_OUT_ENB_H_0_CLK_ENB_I2C2_SHIFT
                                 + NVBOOT_CLOCKS_STANDARD_ENB + NVBOOT_CLOCKS_H_REG
                                 + (CLK_RST_CONTROLLER_CLK_SOURCE_I2C2_0 << NVBOOT_CLOCKS_SOURCE_SHIFT),

    NvBootClocksClockId_I2c3Id   = CLK_RST_CONTROLLER_CLK_OUT_ENB_U_0_CLK_ENB_I2C3_SHIFT
                                 + NVBOOT_CLOCKS_STANDARD_ENB + NVBOOT_CLOCKS_U_REG
                                 + (CLK_RST_CONTROLLER_CLK_SOURCE_I2C3_0 << NVBOOT_CLOCKS_SOURCE_SHIFT),

    NvBootClocksClockId_NandId   = CLK_RST_CONTROLLER_CLK_OUT_ENB_L_0_CLK_ENB_NDFLASH_SHIFT
                                 + NVBOOT_CLOCKS_STANDARD_ENB
                                 + (CLK_RST_CONTROLLER_CLK_SOURCE_NDFLASH_0 << NVBOOT_CLOCKS_SOURCE_SHIFT),

    NvBootClocksClockId_Sdmmc1Id  = CLK_RST_CONTROLLER_CLK_OUT_ENB_L_0_CLK_ENB_SDMMC1_SHIFT  
                                 + NVBOOT_CLOCKS_STANDARD_ENB
                                 + (CLK_RST_CONTROLLER_CLK_SOURCE_SDMMC1_0 << NVBOOT_CLOCKS_SOURCE_SHIFT),

    NvBootClocksClockId_Sdmmc2Id  = CLK_RST_CONTROLLER_CLK_OUT_ENB_L_0_CLK_ENB_SDMMC2_SHIFT  
                                 + NVBOOT_CLOCKS_STANDARD_ENB
                                 + (CLK_RST_CONTROLLER_CLK_SOURCE_SDMMC2_0 << NVBOOT_CLOCKS_SOURCE_SHIFT),

    NvBootClocksClockId_Sdmmc3Id  = CLK_RST_CONTROLLER_CLK_OUT_ENB_U_0_CLK_ENB_SDMMC3_SHIFT  
                                 + NVBOOT_CLOCKS_STANDARD_ENB + NVBOOT_CLOCKS_U_REG
                                 + (CLK_RST_CONTROLLER_CLK_SOURCE_SDMMC3_0 << NVBOOT_CLOCKS_SOURCE_SHIFT),

    NvBootClocksClockId_Sdmmc4Id  = CLK_RST_CONTROLLER_CLK_OUT_ENB_L_0_CLK_ENB_SDMMC4_SHIFT  
                                 + NVBOOT_CLOCKS_STANDARD_ENB
                                 + (CLK_RST_CONTROLLER_CLK_SOURCE_SDMMC4_0 << NVBOOT_CLOCKS_SOURCE_SHIFT),

    NvBootClocksClockId_BseaId   = CLK_RST_CONTROLLER_CLK_OUT_ENB_H_0_CLK_ENB_BSEA_SHIFT
                                 + NVBOOT_CLOCKS_STANDARD_ENB + NVBOOT_CLOCKS_H_REG,

    NvBootClocksClockId_BsevId   = CLK_RST_CONTROLLER_CLK_OUT_ENB_H_0_CLK_ENB_BSEV_SHIFT
                                 + NVBOOT_CLOCKS_STANDARD_ENB + NVBOOT_CLOCKS_H_REG,

    NvBootClocksClockId_KbcId    = CLK_RST_CONTROLLER_CLK_OUT_ENB_H_0_CLK_ENB_KBC_SHIFT
                                 + NVBOOT_CLOCKS_STANDARD_ENB + NVBOOT_CLOCKS_H_REG,

    NvBootClocksClockId_VdeId    = CLK_RST_CONTROLLER_CLK_OUT_ENB_H_0_CLK_ENB_VDE_SHIFT
                                 + NVBOOT_CLOCKS_STANDARD_ENB + NVBOOT_CLOCKS_H_REG
                                 + (CLK_RST_CONTROLLER_CLK_SOURCE_VDE_0 << NVBOOT_CLOCKS_SOURCE_SHIFT),

    NvBootClocksClockId_ApbDmaId = CLK_RST_CONTROLLER_CLK_OUT_ENB_H_0_CLK_ENB_APBDMA_SHIFT
                                 + NVBOOT_CLOCKS_STANDARD_ENB + NVBOOT_CLOCKS_H_REG,

    NvBootClocksClockId_AhbDmaId = CLK_RST_CONTROLLER_CLK_OUT_ENB_H_0_CLK_ENB_AHBDMA_SHIFT
                                 + NVBOOT_CLOCKS_STANDARD_ENB + NVBOOT_CLOCKS_H_REG,

    NvBootClocksClockId_SnorId    = CLK_RST_CONTROLLER_CLK_OUT_ENB_H_0_CLK_ENB_NOR_SHIFT
                                 + NVBOOT_CLOCKS_STANDARD_ENB + NVBOOT_CLOCKS_H_REG
                                 + (CLK_RST_CONTROLLER_CLK_SOURCE_NOR_0 << NVBOOT_CLOCKS_SOURCE_SHIFT),

    NvBootClocksClockId_EmcId    = CLK_RST_CONTROLLER_CLK_OUT_ENB_H_0_CLK_ENB_EMC_SHIFT
                                 + NVBOOT_CLOCKS_STANDARD_ENB + NVBOOT_CLOCKS_H_REG,

    NvBootClocksClockId_McId    = CLK_RST_CONTROLLER_CLK_OUT_ENB_H_0_CLK_ENB_MEM_SHIFT
                                 + NVBOOT_CLOCKS_STANDARD_ENB + NVBOOT_CLOCKS_H_REG,

    NvBootClocksClockId_SpiId    = CLK_RST_CONTROLLER_CLK_OUT_ENB_H_0_CLK_ENB_SPI1_SHIFT
                                 + NVBOOT_CLOCKS_STANDARD_ENB + NVBOOT_CLOCKS_H_REG
                                 + (CLK_RST_CONTROLLER_CLK_SOURCE_SPI1_0 << NVBOOT_CLOCKS_SOURCE_SHIFT),

    NvBootClocksClockId_PmcId    = CLK_RST_CONTROLLER_CLK_OUT_ENB_H_0_CLK_ENB_PMC_SHIFT
                                 + NVBOOT_CLOCKS_STANDARD_ENB + NVBOOT_CLOCKS_H_REG,

    NvBootClocksClockId_Usb3Id   = CLK_RST_CONTROLLER_CLK_OUT_ENB_H_0_CLK_ENB_USB3_SHIFT
                                 + NVBOOT_CLOCKS_STANDARD_ENB + NVBOOT_CLOCKS_H_REG,

    NvBootClocksClockId_Force32 = 0x7fffffff
} NvBootClocksClockId;

#define NVBOOT_CLOCKS_CHECK_CLOCKID(ClockId) \
    NV_ASSERT( (ClockId == NvBootClocksClockId_SclkId) || \
               (ClockId == NvBootClocksClockId_HclkId) || \
               (ClockId == NvBootClocksClockId_PclkId) || \
               (ClockId == NvBootClocksClockId_CclkId) || \
               (ClockId == NvBootClocksClockId_UsbId)  || \
               (ClockId == NvBootClocksClockId_Usb3Id) || \
               (ClockId == NvBootClocksClockId_I2c1Id) || \
               (ClockId == NvBootClocksClockId_NandId) || \
               (ClockId == NvBootClocksClockId_Sdmmc1Id)||  \
               (ClockId == NvBootClocksClockId_Sdmmc2Id)||  \
               (ClockId == NvBootClocksClockId_Sdmmc3Id)||  \
               (ClockId == NvBootClocksClockId_Sdmmc4Id)||  \
               (ClockId == NvBootClocksClockId_BseaId)  || \
               (ClockId == NvBootClocksClockId_BsevId)  || \
               (ClockId == NvBootClocksClockId_KbcId)   || \
               (ClockId == NvBootClocksClockId_VdeId)   || \
               (ClockId == NvBootClocksClockId_ApbDmaId) || \
               (ClockId == NvBootClocksClockId_AhbDmaId) || \
               (ClockId == NvBootClocksClockId_SnorId)    || \
               (ClockId == NvBootClocksClockId_EmcId)    || \
               (ClockId == NvBootClocksClockId_McId)    || \
               (ClockId == NvBootClocksClockId_PmcId)    || \
               (ClockId == NvBootClocksClockId_SpiId)    );

// similar when changing a clock, much smaller than a PLL
#define NVBOOT_CLOCKS_CLOCK_STABILIZATION_TIME (0x2)

// Set of PLL supported in the API
// The enum encodes their base and misc offset
typedef enum
{
    NvBootClocksPllId_PllC = (CLK_RST_CONTROLLER_PLLC_BASE_0 << 16) | CLK_RST_CONTROLLER_PLLC_MISC_0,
    NvBootClocksPllId_PllM = (CLK_RST_CONTROLLER_PLLM_BASE_0 << 16) | CLK_RST_CONTROLLER_PLLM_MISC_0,
    NvBootClocksPllId_PllU = (CLK_RST_CONTROLLER_PLLU_BASE_0 << 16) | CLK_RST_CONTROLLER_PLLU_MISC_0,
    NvBootClocksPllId_PllP = (CLK_RST_CONTROLLER_PLLP_BASE_0 << 16) | CLK_RST_CONTROLLER_PLLP_MISC_0,
    NvBootClocksPllId_PllX = (CLK_RST_CONTROLLER_PLLX_BASE_0 << 16) | CLK_RST_CONTROLLER_PLLX_MISC_0,
    NvBootClocksPllId_Force32 = 0x7fffffff
} NvBootClocksPllId;

#define NVBOOT_CLOCKS_CHECK_PLLID(PllId) \
    NV_ASSERT( (PllId == NvBootClocksPllId_PllC) || \
               (PllId == NvBootClocksPllId_PllM) || \
               (PllId == NvBootClocksPllId_PllU) || \
               (PllId == NvBootClocksPllId_PllP) || \
               (PllId == NvBootClocksPllId_PllX)    ) ;

#define NVBOOT_CLOCKS_PLL_BASE(PLLID) ((((NvU32) PLLID) >> 16 ) & 0xFFFF)
#define NVBOOT_CLOCKS_PLL_MISC(PLLID) ((((NvU32) PLLID) >>  0 ) & 0xFFFF)

/*
 * NvBootClocksOscFreq NvBootClocksGetOscFreq(): get the current osc frequency
 */

NvBootClocksOscFreq
NvBootClocksGetOscFreq(void);

/*
 * void NvBootClocksSetOscFreq(NvBootClocksOscFreq): set the current
 * osc frequency
 */

void
NvBootClocksSetOscFreq(NvBootClocksOscFreq OscFreq);

/*
 * NvBootCLocksBypassPll(): Put the PLL in bypass, insuring an active clock
 *
 */
void
NvBootClocksBypassPll(NvBootClocksPllId PllId) ;

/*
 * NvBootClocksStartPll(): Start the identified PLL, track its stabilization time
 */
void
NvBootClocksStartPll(NvBootClocksPllId PllId,
                     NvU32 M,
                     NvU32 N,
                     NvU32 P,
                     NvU32 CPCON,
                     NvU32 LFCON,
                     NvU32 *StableTime) ;            // Must be a valid address

/*
 * NvBootClocksIsPllStable(): Check if the identified PLL is stable
 */
NvBool
NvBootClocksIsPllStable(NvU32 StableTime);

/*
 * NvBootClocksStopPll(): Stop the identified PLL, no check it is in use or not
 */
void
NvBootClocksStopPll(NvBootClocksPllId PllId);

/*
 * NvBootClocksConfigureClock(): Configure the identified clock
 */
void
NvBootClocksConfigureClock(NvBootClocksClockId ClockId,
                           NvU32 Divider,
                           NvU32 Source);

/* For 7.1 the ratio can be an integer multiple of 0.5 larger than 1 */
/* the divider value to program equal the desired ratio * 2 - 2 or alternately 2 * (desired ratio - 1) */
/* to avoid floating point, this is done as 2 * integer part of ratio + 1 if odd multiple of 0.5 - 2 */
#define NVBOOT_CLOCKS_7_1_DIVIDER_BY(INT_RATIO, PLUS_HALF) ( 2 * INT_RATIO + PLUS_HALF - 2)
#define NVBOOT_CLOCKS_7_1_DIVIDER_BY_1    NVBOOT_CLOCKS_7_1_DIVIDER_BY( 1, 0)
#define NVBOOT_CLOCKS_7_1_DIVIDER_BY_2    NVBOOT_CLOCKS_7_1_DIVIDER_BY( 2, 0)
#define NVBOOT_CLOCKS_7_1_DIVIDER_BY_3    NVBOOT_CLOCKS_7_1_DIVIDER_BY( 3, 0)
#define NVBOOT_CLOCKS_7_1_DIVIDER_BY_4    NVBOOT_CLOCKS_7_1_DIVIDER_BY( 4, 0)
#define NVBOOT_CLOCKS_7_1_DIVIDER_BY_5    NVBOOT_CLOCKS_7_1_DIVIDER_BY( 5, 0)
#define NVBOOT_CLOCKS_7_1_DIVIDER_BY_9    NVBOOT_CLOCKS_7_1_DIVIDER_BY( 9, 0)
#define NVBOOT_CLOCKS_7_1_DIVIDER_BY_17   NVBOOT_CLOCKS_7_1_DIVIDER_BY(17, 0)
#define NVBOOT_CLOCKS_7_1_DIVIDER_BY_67_5 NVBOOT_CLOCKS_7_1_DIVIDER_BY(67, 1)

/* for devices, reference is PLLP, so we can express that in absolute frequency */
#define NVBOOT_CLOCKS_PLLP_OUT0_DIV_TO_432_0_MHZ NVBOOT_CLOCKS_7_1_DIVIDER_BY_1
#define NVBOOT_CLOCKS_PLLP_OUT0_DIV_TO_216_0_MHZ NVBOOT_CLOCKS_7_1_DIVIDER_BY_2
#define NVBOOT_CLOCKS_PLLP_OUT0_DIV_TO_144_0_MHZ NVBOOT_CLOCKS_7_1_DIVIDER_BY_3
#define NVBOOT_CLOCKS_PLLP_OUT0_DIV_TO_108_0_MHZ NVBOOT_CLOCKS_7_1_DIVIDER_BY_4
#define NVBOOT_CLOCKS_PLLP_OUT0_DIV_TO_86_4_MHZ  NVBOOT_CLOCKS_7_1_DIVIDER_BY_5
#define NVBOOT_CLOCKS_PLLP_OUT0_DIV_TO_48_0_MHZ  NVBOOT_CLOCKS_7_1_DIVIDER_BY_9
#define NVBOOT_CLOCKS_PLLP_OUT0_DIV_TO_25_4_MHZ  NVBOOT_CLOCKS_7_1_DIVIDER_BY_17
#define NVBOOT_CLOCKS_PLLP_OUT0_DIV_TO_6_4_MHZ   NVBOOT_CLOCKS_7_1_DIVIDER_BY_67_5

/* some clocks have straight divider in n-1 format */
#define NVBOOT_CLOCKS_N_DIVIDER_BY(N) (N-1)
#define NVBOOT_CLOCKS_N_DIVIDER_BY_1 NVBOOT_CLOCKS_N_DIVIDER_BY(1)
#define NVBOOT_CLOCKS_N_DIVIDER_BY_2 NVBOOT_CLOCKS_N_DIVIDER_BY(2)
#define NVBOOT_CLOCKS_N_DIVIDER_BY_3 NVBOOT_CLOCKS_N_DIVIDER_BY(3)
#define NVBOOT_CLOCKS_N_DIVIDER_BY_4 NVBOOT_CLOCKS_N_DIVIDER_BY(4)

/*
 * NvBootClocksSetEnable(): Change the enable status
 */
void
NvBootClocksSetEnable(NvBootClocksClockId ClockId, NvBool Enable);

/*
 * NvBootClocksSetAvpClockBeforeScatterLoad(): Special function that runs
 * before scatter loading. Assumes PLLP is stable.
 * Cannot use anything that rely on scatter loading, i.e. no dynamic global
 * Local variables (stack) and static global (ROM) should be OK
 */
void
NvBootClocksSetAvpClockBeforeScatterLoad(void);

/*
 * NvBootClocksStartPllpBeforeScatterLoad(): Special function that runs
 * before scatter loading
 * Cannot use anything that rely on scatter loading, i.e. no dynamic global
 * Local variables (stack) and static global (ROM) should be OK
 */
void
NvBootClocksStartPllpBeforeScatterLoad(NvBootClocksOscFreq OscFreq);

/**
 * NvBootClocksMeasureOscFreq()
 *
 * This function use the HW engine in CAR to measure the Osc Frequency
 * against the 32 kHz clock.
 * This function may be called while in FA mode and so must be in
 * the non secure area of memory
 *
 * @return: The osc frequency
 *
 */
NvBootClocksOscFreq
NvBootClocksMeasureOscFreq(void);

/*
 * NvBootConfigureUsecTimer(): Special function that runs
 * before scatter loading
 * Cannot use anything that rely on scatter loading, i.e. no dynamic global
 * Local variables (stack) and static global (ROM) should be OK
 */
void
NvBootClocksConfigureUsecTimer(NvBootClocksOscFreq OscFreq);

#if defined(__cplusplus)
}
#endif

#endif /* #ifndef INCLUDED_NVBOOT_CLOCKS_INT_H */
