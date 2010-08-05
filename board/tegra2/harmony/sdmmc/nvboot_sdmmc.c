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

#define NV_SDMMC4
//#undef NV_SDMMC4

#include <nv_sdmmc.h>
#include <nvboot_device.h>
#include <nvboot_sdmmc_param.h>
#include <nvboot_sdmmc_context.h>
#include "nvboot_pads.h"
#include "nvboot_clocks_int.h"
#include "nvboot_sdmmc_int.h"
#include "nvboot_reset.h"
#include "nvboot_util.h"

#define DEBUG_SDMMC 0

#ifdef NV_SDMMC4
#define NVBOOT_SDMMC_BASE_ADDRESS NV_ADDRESS_MAP_SDMMC4_BASE	
#else
#define NVBOOT_SDMMC_BASE_ADDRESS NV_ADDRESS_MAP_SDMMC2_BASE    // SD only
#endif

#if DEBUG_SDMMC
#define PRINT_SDMMC_REG_ACCESS(...)  printf(__VA_ARGS__);
#define PRINT_SDMMC_MESSAGES(...)    printf(__VA_ARGS__);
#define PRINT_SDMMC_ERRORS(...)      printf(__VA_ARGS__);
#else
#define PRINT_SDMMC_REG_ACCESS(...)
#define PRINT_SDMMC_MESSAGES(...)
#define PRINT_SDMMC_ERRORS(...)
#endif

#if DEBUG_SDMMC
#define NV_SDMMC_READ(reg, value) \
    do \
    { \
        NV_READ32_((NVBOOT_SDMMC_BASE_ADDRESS + SDMMC_##reg##_0), value); \
        PRINT_SDMMC_REG_ACCESS("%s: R (0x%08x) %s = 0x%8.8x\n", __FUNCTION__, \
                          (NVBOOT_SDMMC_BASE_ADDRESS + SDMMC_##reg##_0), \
                            #reg, value);\
    } while (0)

#define NV_SDMMC_WRITE(reg, value) \
    do { \
        NV_WRITE32_((NVBOOT_SDMMC_BASE_ADDRESS + SDMMC_##reg##_0), value); \
        PRINT_SDMMC_REG_ACCESS("%s: W (0x%08x) %s = 0x%8.8x\n", __FUNCTION__,\
                          (NVBOOT_SDMMC_BASE_ADDRESS + SDMMC_##reg##_0), \
                            #reg, value);\
    } while (0)

#else
#define NV_SDMMC_READ(reg, value) \
    do \
    { \
        NV_READ32_((NVBOOT_SDMMC_BASE_ADDRESS + SDMMC_##reg##_0), value); \
    } while (0)

#define NV_SDMMC_WRITE(reg, value) \
    do { \
        NV_WRITE32_((NVBOOT_SDMMC_BASE_ADDRESS + SDMMC_##reg##_0), value); \
    } while (0)

#endif

#define NV_SDMMC_WRITE_08(reg, offset, value) \
    do { \
        NV_WRITE08((NVBOOT_SDMMC_BASE_ADDRESS + SDMMC_##reg##_0 + offset), value); \
        PRINT_SDMMC_REG_ACCESS("W (0x%08x) Byte %s = 0x%2x",\
            (NVBOOT_SDMMC_BASE_ADDRESS + SDMMC_##reg##_0 + offset), #reg, \
            value); \
    } while (0)


// Can't use do while for this, as it is also called from PRINT_SDMMC_XXX.
#define QUOTIENT_CEILING(dividend, divisor) \
    ((dividend + divisor - 1) / divisor)

static NvBootSdmmcParams s_DefaultSdmmcParams;
static NvBootSdmmcContext *s_SdmmcContext = NULL;
static NvBool s_IsBootModeDataValid = NV_FALSE;

// This struct holds the Info from fuses.
typedef struct
{
    /*
     * Voltage range to use during card identification.
     * This must be a static variable to preserve its value from GetParams()
     * to Init(). The fuse value, as opposed to the OCR register value, is
     * stored in the static variable because it is easier to perform validation
     * on the fuse value enumeration.
     */
    NvBootSdmmcVoltageRange VoltageRange;
    // Holds Boot mode support.
    NvBool DisableBootMode;
    // Holds the card type (EMMC or SD).
    NvBootSdmmcCardType CardType;
    // Pinmux selection.
    NvU8 PinmuxSelection;
} NvBootSdmmcFuseInfo;

static NvBootSdmmcFuseInfo s_FuseInfo = 
{NvBootSdmmcVoltageRange_QueryVoltage, NV_FALSE, NvBootSdmmcCardType_Emmc, 0};

// Table that maps fuse values to CMD1 OCR argument values.
static NvU32 s_OcrVoltageRange[] = 
{
    EmmcOcrVoltageRange_QueryVoltage, // NvBootSdmmcVoltageRange_QueryVoltage
    EmmcOcrVoltageRange_HighVoltage,  // NvBootSdmmcVoltageRange_HighVoltage
    EmmcOcrVoltageRange_DualVoltage,  // NvBootSdmmcVoltageRange_DualVoltage
    EmmcOcrVoltageRange_LowVoltage    // NvBootSdmmcVoltageRange_LowVoltage
};

/* Forward Private Function declarations. */

/* Private Function Definitions. */

/*
 * Functions HwSdmmcxxx --> does Sdmmc register programming.
 * Functions Sdmmcxxx --> Common to Emmc and Esd cards.
 * Functions Emmcxxx --> Specific to Emmc card.
 * Functions Esdxxx --> Specific to Esd card.
 * Functions NvBootSdmmcxxx --> Public API's
 */

static NvBootError HwSdmmcResetController(void)
{
    NvU32 StcReg;
    NvU32 ResetInProgress;
    NvU32 TimeOut = SDMMC_TIME_OUT_IN_US;
    
    // Reset Controller's All reg's.
    StcReg = NV_DRF_DEF(SDMMC, SW_RESET_TIMEOUT_CTRL_CLOCK_CONTROL,
                SW_RESET_FOR_ALL, RESETED);
    NV_SDMMC_WRITE(SW_RESET_TIMEOUT_CTRL_CLOCK_CONTROL, StcReg);
    // Wait till Reset is completed.
    while (TimeOut)
    {
        NV_SDMMC_READ(SW_RESET_TIMEOUT_CTRL_CLOCK_CONTROL, StcReg);
        ResetInProgress = NV_DRF_VAL(SDMMC, SW_RESET_TIMEOUT_CTRL_CLOCK_CONTROL,
                            SW_RESET_FOR_ALL, StcReg);
        if (!ResetInProgress)
            break;
        NvBootUtilWaitUS(1);
        TimeOut--;
        if (!TimeOut)
        {
            PRINT_SDMMC_ERRORS("Reset all timed out.\n");
            return NvBootError_HwTimeOut;
        }
    }
    return NvBootError_Success;
}

/* Public Function Definitions. */

void FFAConfiguration()
{
    // Config PLLP
    NV_WRITE32(NV_ADDRESS_MAP_CAR_BASE + CLK_RST_CONTROLLER_OSC_CTRL_0, 0xC00003f1);
    NV_WRITE32(NV_ADDRESS_MAP_CAR_BASE + CLK_RST_CONTROLLER_PLLP_BASE_0, 0x8001B01A);
    NV_WRITE32(NV_ADDRESS_MAP_CAR_BASE + CLK_RST_CONTROLLER_PLLP_BASE_0, 0xC001B01A);
    NV_WRITE32(NV_ADDRESS_MAP_CAR_BASE + CLK_RST_CONTROLLER_PLLP_MISC_0, 0x00000800);
    NV_WRITE32(NV_ADDRESS_MAP_CAR_BASE + CLK_RST_CONTROLLER_PLLP_BASE_0, 0xC001B01A);
    NV_WRITE32(NV_ADDRESS_MAP_CAR_BASE + CLK_RST_CONTROLLER_PLLP_BASE_0, 0x4001B01A);
}

void
NvBootSdmmcGetParams(
    const NvU32 ParamIndex,
    NvBootSdmmcParams **Params)
{
    NvU32 Index;
    NV_ASSERT(Params != NULL);
    
    // Extract Data width from Param Index, which comes from fuses.
    Index = NV_DRF_VAL(SDMMC_DEVICE, CONFIG, DATA_WIDTH, ParamIndex);
    /*
     * One Fuse bit is used for Data width. The value starting from
     * zero to one, corresponds to data widths 4 and 8 bits.
     * The enum value for 4bit data width is 1 and 2 for 8-bit data width enum.
     */
    s_DefaultSdmmcParams.DataWidth = (NvBootSdmmcDataWidth)(Index + 1);
    /*
     * Extract the card type from Param Index, which comes from fuses.
     * One Fuse bit is used for card type identification. The value starting 
     * from zero to one, corresponds to cards eMMC and SD.
     */
    s_FuseInfo.CardType = (NvBootSdmmcCardType)NV_DRF_VAL(SDMMC_DEVICE, CONFIG, 
                            CARD_TYPE, ParamIndex);
    /*
     * Extract the voltage range to use from Param Index, which comes from fuses.
     * Two Fuse bits are used for voltage range selection. The value starting 
     * from zero to thress, corresponds to query, high, dual and low voltage ranges.
     */
    s_FuseInfo.VoltageRange = (NvBootSdmmcVoltageRange)NV_DRF_VAL(SDMMC_DEVICE, 
                                CONFIG, VOLTAGE_RANGE, ParamIndex);
    /*
     * Extract the Boot mode support from Param Index, which comes from fuses.
     * One Fuse bit is used for Boot mode support disable/enable. The value 
     * starting from zero to one, corresponds to enable and disable.
     */
    s_FuseInfo.DisableBootMode = NV_DRF_VAL(SDMMC_DEVICE, CONFIG, 
                                    DISABLE_BOOT_MODE, ParamIndex);
    /*
     * Extract the Pinmux selection from Param Index, which comes from fuses.
     * One Fuse bit is used for Pinmux selection. The value 
     * starting from zero to one, corresponds to primary and secondary.
     */
    s_FuseInfo.PinmuxSelection = NV_DRF_VAL(SDMMC_DEVICE, CONFIG, 
                                    PINMUX_SELECTION, ParamIndex);
    /*
     * Set the MoviNand clock source frquency as 20MHz before BCT is read. This
     * is for backward compatibility of cards, which don't support high speed mode.
     * The Clock Source to MoviNand is PLLP and which is operating at 432MHz.
     * To find out clock divisor value, divide it by 20 (432/20=21.6) and 
     * convert it to ceiling value, which is 22. 432/22 = 19.63MHz. The frequency
     * should not be more than 20MHz to run in normal speed for EMMC cards of 
     * version less than 4.3.
     */
    s_DefaultSdmmcParams.ClockDivider = QUOTIENT_CEILING(SDMMC_PLL_FREQ_IN_MHZ, 20);
    /*
     * Max Power class supported by target board is unknown. Bct would give us
     * the Max power class supported. So, Till that time, Let it be 0 and work
     * with power calss 0..Target board must support power class 0.
     */
    s_DefaultSdmmcParams.MaxPowerClassSupported = 0;
    
    *Params = (NvBootSdmmcParams*)&s_DefaultSdmmcParams;
#if NVRM
    s_SdmmcBitInfo->FuseDataWidth = s_DefaultSdmmcParams.DataWidth;
    s_SdmmcBitInfo->FuseCardType = s_FuseInfo.CardType;
    s_SdmmcBitInfo->FuseVoltageRange = s_FuseInfo.VoltageRange;
    s_SdmmcBitInfo->FuseDisableBootMode = s_FuseInfo.DisableBootMode;
    s_SdmmcBitInfo->FusePinmuxSelection = s_FuseInfo.PinmuxSelection;
#endif
    
    PRINT_SDMMC_MESSAGES("ParamIndex=0x%x, DataWidth=%d (1->4bit, 2->8bit), "
        "CardType=%d (0->EMMC, 1->ESD),\r\nVoltageRange=%d(0->query, 1->high, "
        "2-> dual, 3->low), DisableBootMode=%d(0:E,1:D),\r\nClockDivider=%d, MaxPowClass"
        "Supported=%d\n", ParamIndex, s_DefaultSdmmcParams.DataWidth, 
        s_FuseInfo.CardType, s_FuseInfo.VoltageRange, s_FuseInfo.DisableBootMode, 
        s_DefaultSdmmcParams.ClockDivider, s_DefaultSdmmcParams.MaxPowerClassSupported);
}


void
NvBootSdmmcGetBlockSizes(
    const NvBootSdmmcParams *Params,
    NvU32 *BlockSizeLog2,
    NvU32 *PageSizeLog2)
{
    NV_ASSERT(Params != NULL);
    NV_ASSERT(BlockSizeLog2 != NULL);
    NV_ASSERT(PageSizeLog2 != NULL);
    NV_ASSERT(s_SdmmcContext != NULL);
    
    *BlockSizeLog2 = s_SdmmcContext->BlockSizeLog2;
    *PageSizeLog2 = s_SdmmcContext->PageSizeLog2;
    PRINT_SDMMC_MESSAGES("BlockSize=%d, PageSize=%d, PagesPerBlock=%d\n", 
        (1 << s_SdmmcContext->BlockSizeLog2),(1 << s_SdmmcContext->PageSizeLog2),
        (1 << s_SdmmcContext->PagesPerBlockLog2));
}

static NvBootError HwSdmmcWaitForClkStable(void)
{
    NvU32 StcReg;
    NvU32 ClockReady;
    NvU32 TimeOut = SDMMC_TIME_OUT_IN_US;
    
    while (TimeOut)
    {
        NV_SDMMC_READ(SW_RESET_TIMEOUT_CTRL_CLOCK_CONTROL, StcReg);
        ClockReady = NV_DRF_VAL(SDMMC, SW_RESET_TIMEOUT_CTRL_CLOCK_CONTROL,
                        INTERNAL_CLOCK_STABLE, StcReg);
        if (ClockReady)
            break;
        NvBootUtilWaitUS(1);
        TimeOut--;
        if (!TimeOut)
        {
            PRINT_SDMMC_ERRORS("HwSdmmcInitController()-Clk stable timed out.\n");
            return NvBootError_HwTimeOut;
        }
    }
    return NvBootError_Success;
}

static NvBootError HwSdmmcSetCardClock(NvBootSdmmcCardClock ClockRate)
{
    NvU32 taac;
    NvU32 nsac;
    NvU32 StcReg;
    NvBootError e;
    NvU32 CardClockInMHz;
    NvU32 CardClockDivisor;
    NvU32 TimeOutCounter = 0;
    NvU32 ClockCyclesRequired;
    NvU32 ControllerClockInMHz;
    NvU32 CardCycleTimeInNanoSec;
    NvU32 ControllerClockDivisor;
    NvU32 ContollerCycleTimeInNanoSec;
    // These array values are as per Emmc/Esd Spec's.
    const NvU32 TaacTimeUnitArray[] = {1, 10, 100, 1000, 10000, 100000, 1000000, 
                                       10000000};
    const NvU32 TaacMultiplierArray[] = {10, 10, 12, 13, 15, 20, 25, 30, 35, 40, 
                                         45, 50, 55, 60, 70, 80};
    
    s_SdmmcContext->CurrentClockRate = ClockRate;
    // Disable Card clock before changing it's Frequency.
    NV_SDMMC_READ(SW_RESET_TIMEOUT_CTRL_CLOCK_CONTROL, StcReg);
    StcReg = NV_FLD_SET_DRF_DEF(SDMMC, SW_RESET_TIMEOUT_CTRL_CLOCK_CONTROL,
                SD_CLOCK_EN, DISABLE, StcReg);
    NV_SDMMC_WRITE(SW_RESET_TIMEOUT_CTRL_CLOCK_CONTROL, StcReg);
    
    if (ClockRate == NvBootSdmmcCardClock_Identification)
    {
        /*
         * Set the clock divider as 18 for card identification. With clock divider
         * as 18, controller gets a frequency of 432/18 = 24MHz and it will be
         * furthur divided by 64. After dividing it by 64, card gets a 
         * frequency of 375KHz. It is the frequency at which card should 
         * be identified.
         */
        ControllerClockDivisor = 18;
        CardClockDivisor = 64;
    }
    else if (ClockRate == NvBootSdmmcCardClock_DataTransfer)
    {
        ControllerClockDivisor = s_SdmmcContext->ClockDivisor;
        CardClockDivisor = s_SdmmcContext->CardClockDivisor;
    }
    else //if (ClockRate == NvBootSdmmcCardClock_20MHz)
    {
        ControllerClockDivisor = 22;
        CardClockDivisor = 1;
    }
    
    ControllerClockInMHz = QUOTIENT_CEILING(SDMMC_PLL_FREQ_IN_MHZ, 
                                ControllerClockDivisor);
    ContollerCycleTimeInNanoSec = QUOTIENT_CEILING(1000, ControllerClockInMHz);
    CardClockInMHz = QUOTIENT_CEILING(SDMMC_PLL_FREQ_IN_MHZ, 
                        (ControllerClockDivisor * CardClockDivisor));
    CardCycleTimeInNanoSec = QUOTIENT_CEILING(1000, CardClockInMHz);
    // Find read time out.
    if (s_SdmmcContext->taac != 0)
    {
        // For Emmc, Read time is 10 times of (TAAC + NSAC).
        // for Esd, Read time is 100 times of (TAAC + NSAC) or 100ms, which ever
        // is lower.
        taac = TaacTimeUnitArray[s_SdmmcContext->taac & 
               EMMC_CSD_TAAC_TIME_UNIT_MASK] * 
               TaacMultiplierArray[(s_SdmmcContext->taac >> 
               EMMC_CSD_TAAC_TIME_VALUE_OFFSET) & EMMC_CSD_TAAC_TIME_VALUE_MASK];
        nsac = CardCycleTimeInNanoSec * s_SdmmcContext->nsac * 1000;
        // taac and nsac are already multiplied by 10.
// jz
//        s_SdmmcContext->ReadTimeOutInUs = QUOTIENT_CEILING((taac + nsac), 1000);
        PRINT_SDMMC_MESSAGES("Card ReadTimeOutInUs=%d\n", 
            s_SdmmcContext->ReadTimeOutInUs);
        // Use 200ms time out instead of 100ms. This could be helpful in case
        // old version of cards.
        if (s_SdmmcContext->ReadTimeOutInUs < 200000)
            s_SdmmcContext->ReadTimeOutInUs = 200000;
        else if (s_SdmmcContext->ReadTimeOutInUs > 800000)
        {
            //NV_ASSERT(NV_FALSE);
            // Calculation seem to have gone wrong or TAAc is not valid. 
            // Set it to 800msec, which is max timeout.
            s_SdmmcContext->ReadTimeOutInUs = 800000;
        }
    }
    PRINT_SDMMC_MESSAGES("Base Clock=%dMHz\n", 
        QUOTIENT_CEILING(SDMMC_PLL_FREQ_IN_MHZ, ControllerClockDivisor));
    PRINT_SDMMC_MESSAGES("HwSdmmcSetCardClock Div=%d\n", CardClockDivisor);
    
    NvBootClocksConfigureClock(NvBootClocksClockId_SdmmcId,
        NVBOOT_CLOCKS_7_1_DIVIDER_BY(ControllerClockDivisor, 0),
#ifdef NV_SDMMC4
        CLK_RST_CONTROLLER_CLK_SOURCE_SDMMC4_0_SDMMC4_CLK_SRC_PLLP_OUT0);
#else
        CLK_RST_CONTROLLER_CLK_SOURCE_SDMMC2_0_SDMMC2_CLK_SRC_PLLP_OUT0);
#endif
    // If the card clock divisor is 64, the register should be written with 32.
    StcReg = NV_FLD_SET_DRF_NUM(SDMMC, SW_RESET_TIMEOUT_CTRL_CLOCK_CONTROL,
                SDCLK_FREQUENCYSELECT, (CardClockDivisor >> 1), StcReg);
    NV_SDMMC_WRITE(SW_RESET_TIMEOUT_CTRL_CLOCK_CONTROL, StcReg);
    // Wait till clock is stable.
    NV_BOOT_CHECK_ERROR(HwSdmmcWaitForClkStable());
    // Reload reg value after clock is stable.
    NV_SDMMC_READ(SW_RESET_TIMEOUT_CTRL_CLOCK_CONTROL, StcReg);
    // Enable card's clock after clock frequency is changed.
    StcReg = NV_FLD_SET_DRF_DEF(SDMMC, SW_RESET_TIMEOUT_CTRL_CLOCK_CONTROL,
                SD_CLOCK_EN, ENABLE, StcReg);
    /*
     * Set Data timeout.
     * If the time out bit field is set to 0xd here, which means the time out at
     * base clock 63MHz is 1065 msec. i.e 2^26 / 63MHz = 1065msec.
     * ControllerClockInMHz = SDMMC_PLL_FREQ_IN_MHZ/ ControllerClockDivisor;
     * ControllerCycleTimeInNanoSec = 1000 / ControllerClockInMHz;
     * ClockCyclesRequired = (s_SdmmcContext->ReadTimeOutInUs * 1000) / 
     *                       ControllerClockTimeInNanoSec;
     *                     = (s_SdmmcContext->ReadTimeOutInUs * 1000)/ 
     *                       (1000 / ControllerClockInMHz);
     *                     = (s_SdmmcContext->ReadTimeOutInUs * ControllerClockInMHz);
     *                     = (s_SdmmcContext->ReadTimeOutInUs * 
     *                       (SDMMC_PLL_FREQ_IN_MHZ/ ControllerClockDivisor));
     *                     = (s_SdmmcContext->ReadTimeOutInUs * SDMMC_PLL_FREQ_IN_MHZ) / 
     *                       ControllerClockDivisor;
     */
    ClockCyclesRequired = QUOTIENT_CEILING( (s_SdmmcContext->ReadTimeOutInUs * 
                            SDMMC_PLL_FREQ_IN_MHZ), ControllerClockDivisor );
    // TimeOutCounter value zero means that the time out is (1 << 13).
    while ( ClockCyclesRequired > (1 << (13 + TimeOutCounter)) )
    {
        TimeOutCounter++;
        // This is max value. so break out from here.
        if (TimeOutCounter == 0xE)
            break;
    }
    // Recalculate the ReadTimeOutInUs based value that is set to register.
    // We shouldn't timout in the code before the controller times out.
    s_SdmmcContext->ReadTimeOutInUs = (1 << (13 + TimeOutCounter));
    s_SdmmcContext->ReadTimeOutInUs = QUOTIENT_CEILING(
                                        s_SdmmcContext->ReadTimeOutInUs, 1000);
    s_SdmmcContext->ReadTimeOutInUs = s_SdmmcContext->ReadTimeOutInUs * 
                                      ContollerCycleTimeInNanoSec;
    // The code should never time out before controller. Give some extra time for
    // read time out. Add 50msecs.
    s_SdmmcContext->ReadTimeOutInUs += 50000;
    if (s_SdmmcContext->ReadTimeOutInUs < 200000)
        s_SdmmcContext->ReadTimeOutInUs = 200000;
    else if (s_SdmmcContext->ReadTimeOutInUs > 800000)
    {
        //NV_ASSERT(NV_FALSE);
        // Calculation seem to have gone wrong. Set it to 800msec, which
        // is max timeout.
        s_SdmmcContext->ReadTimeOutInUs = 800000;
    }
    
    StcReg = NV_FLD_SET_DRF_NUM(SDMMC, SW_RESET_TIMEOUT_CTRL_CLOCK_CONTROL,
                DATA_TIMEOUT_COUNTER_VALUE, TimeOutCounter, StcReg);
    NV_SDMMC_WRITE(SW_RESET_TIMEOUT_CTRL_CLOCK_CONTROL, StcReg);
    PRINT_SDMMC_MESSAGES("TimeOutCounter=%d, ClockCyclesRequired=%d, "
        "CardCycleTimeInNanoSec=%d,\r\nContollerCycleTimeInNanoSec=%d\n", 
        TimeOutCounter, ClockCyclesRequired, CardCycleTimeInNanoSec, 
        ContollerCycleTimeInNanoSec);
    PRINT_SDMMC_MESSAGES("Recalc ReadTimeOutInUs=%d, clk cycles in ns=%d\n", 
        s_SdmmcContext->ReadTimeOutInUs, ((1 << (13 + TimeOutCounter)) * 
        ContollerCycleTimeInNanoSec));
    return NvBootError_Success;
}

static void HwSdmmcSetDataWidth(NvBootSdmmcDataWidth DataWidth)
{
    NvU32 PowerControlHostReg = 0;
    
    PRINT_SDMMC_MESSAGES("%s: DataWidth: %c\n", __FUNCTION__, (DataWidth == NvBootSdmmcDataWidth_8Bit) ? '8' : '4');
    NV_SDMMC_READ(POWER_CONTROL_HOST, PowerControlHostReg);
    PowerControlHostReg = NV_FLD_SET_DRF_NUM(SDMMC, POWER_CONTROL_HOST,
                            DATA_XFER_WIDTH, DataWidth, PowerControlHostReg);
    // When 8-bit data width is enabled, the bit field DATA_XFER_WIDTH 
    // value is not valid.
    PowerControlHostReg = NV_FLD_SET_DRF_NUM(SDMMC, POWER_CONTROL_HOST,
                            EXTENDED_DATA_TRANSFER_WIDTH, 
                            ((DataWidth == NvBootSdmmcDataWidth_8Bit) ? 1 : 0),
                            PowerControlHostReg);
    NV_SDMMC_WRITE(POWER_CONTROL_HOST, PowerControlHostReg);
#if NVRM
    s_SdmmcBitInfo->DataWidthUnderUse = DataWidth;
#endif
}

static void HwSdmmcSetNumOfBlocks(NvU32 BlockLength, NvU32 NumOfBlocks)
{
    NvU32 BlockReg;
    
    BlockReg = NV_DRF_NUM(SDMMC, BLOCK_SIZE_BLOCK_COUNT, BLOCKS_COUNT, 
                NumOfBlocks) |
               NV_DRF_DEF(SDMMC, BLOCK_SIZE_BLOCK_COUNT,
               /*
                * This makes controller halt when ever it detects 512KB boundary.
                * When controller halts on this boundary, need to clear the 
                * dma block boundary event and write SDMA base address again.
                * Writing address again triggers controller to continue.
                * We can't disable this. We have to live with it.
                */
                HOST_DMA_BUFFER_SIZE, DMA4K) |
               NV_DRF_NUM(SDMMC, BLOCK_SIZE_BLOCK_COUNT,
                XFER_BLOCK_SIZE_11_0, BlockLength);
// jz
//printf("%s: Before, BlockReg: 0x%08x\n", __FUNCTION__, BlockReg);
//    BlockReg &= ~0x00007000;       // DMA buffer 4k
//printf("%s: After, BlockReg: 0x%08x\n", __FUNCTION__, BlockReg);

    NV_SDMMC_WRITE(BLOCK_SIZE_BLOCK_COUNT, BlockReg);
}
 
static void HwSdmmcSetupDma(NvU8 *pBuffer, NvU32 NumOfBytes)
{
    // Program Single DMA base address.
    NV_SDMMC_WRITE(SYSTEM_ADDRESS, (NvU32)(pBuffer));
}

// This array indicates the response formats that need to be programmed to
static NvBootError HwSdmmcInitController(void)
{
    NvU32 StcReg;
    NvBootError e;
    NvU32 CapabilityReg;
    NvU32 PinmuxSelection;
    NvU32 IntStatusEnableReg;
    NvU32 PowerControlHostReg;
    NvU32 RegData;
    
#if 1    
    NV_ASSERT(s_FuseInfo.PinmuxSelection <= 1);
    if (s_FuseInfo.PinmuxSelection)
    {
        PRINT_SDMMC_MESSAGES("Setting Pinmux for Alt\n");
        PinmuxSelection = NvBootPinmuxConfig_Sdmmc_Alt;
    }
    else if (s_SdmmcContext->DataWidth == NvBootSdmmcDataWidth_8Bit)
    {
        PRINT_SDMMC_MESSAGES("Setting Pinmux for 8-bit\n");
        PinmuxSelection = NvBootPinmuxConfig_Sdmmc_Std_x8;
    }
    else
    {
        PRINT_SDMMC_MESSAGES("Setting Pinmux for 4-bit\n");
        PinmuxSelection = NvBootPinmuxConfig_Sdmmc_Std_x4;
    }
#if 0 // original, jz
    (void)NvBootPadsConfigForBootDevice(NvBootFuseBootDevice_Sdmmc, 
        PinmuxSelection);
#else
    // 0x70000080
    NV_READ32_((NV_ADDRESS_MAP_APB_MISC_BASE +
                     APB_MISC_PP_PIN_MUX_CTL_A_0), RegData );

    RegData = 0x0063231f;
    NV_WRITE32((NV_ADDRESS_MAP_APB_MISC_BASE +
                     APB_MISC_PP_PIN_MUX_CTL_A_0), RegData );

    // 0x70000084
    NV_READ32_((NV_ADDRESS_MAP_APB_MISC_BASE +
                     APB_MISC_PP_PIN_MUX_CTL_A_0 + 4), RegData );

    RegData = 0x10100103;
    NV_WRITE32((NV_ADDRESS_MAP_APB_MISC_BASE +
                     APB_MISC_PP_PIN_MUX_CTL_A_0 + 4), RegData );

    // 0x70000014
    NV_READ32_((NV_ADDRESS_MAP_APB_MISC_BASE +
                     APB_MISC_PP_TRISTATE_REG_A_0), RegData );

#ifdef NV_SDMMC4
    RegData = 0xc003eff0;
#else
    RegData = 0xc003a7f0;
#endif
    NV_WRITE32((NV_ADDRESS_MAP_APB_MISC_BASE +
                     APB_MISC_PP_TRISTATE_REG_A_0), RegData );

#if 0 // jz
    RegData &= ~0x00005800;
    NV_WRITE32((NV_ADDRESS_MAP_APB_MISC_BASE +
                     APB_MISC_PP_TRISTATE_REG_A_0), RegData );

    // set voltage to 3.3v
    // 0x70000870
    NV_READ32_((NV_ADDRESS_MAP_APB_MISC_BASE + 
                        APB_MISC_GP_ATCFG1PADCTRL_0), RegData);

    RegData = 0xf1f1f038;
    NV_WRITE32((NV_ADDRESS_MAP_APB_MISC_BASE +
                     APB_MISC_GP_ATCFG1PADCTRL_0), RegData);

    // 0x70000874
    NV_READ32_((NV_ADDRESS_MAP_APB_MISC_BASE + 
                        APB_MISC_GP_ATCFG2PADCTRL_0), RegData);

    RegData = 0xf1f1f038;
    NV_WRITE32((NV_ADDRESS_MAP_APB_MISC_BASE +
                     APB_MISC_GP_ATCFG2PADCTRL_0), RegData);

    // 0x700008f4
    NV_READ32_((NV_ADDRESS_MAP_APB_MISC_BASE + 
                        APB_MISC_GP_GMACFGPADCTRL_0), RegData);

    RegData = 0xf1612030;
    NV_WRITE32((NV_ADDRESS_MAP_APB_MISC_BASE +
                     APB_MISC_GP_GMACFGPADCTRL_0), RegData);
#endif

#endif
#endif

    // Keep the controller in Reset.
    PRINT_SDMMC_MESSAGES("%s: set contrl in Reset\n", __FUNCTION__);
    NvBootResetSetEnable(NvBootResetDeviceId_SdmmcId, NV_TRUE);

    // Enable the clock.
    PRINT_SDMMC_MESSAGES("%s: enable clock\n", __FUNCTION__);
    NvBootClocksSetEnable(NvBootClocksClockId_SdmmcId, NV_TRUE);

    // Configure the clock source with divider 18, which gives 24MHz.
    PRINT_SDMMC_MESSAGES("Base Clock=%dMHz\n", 
        QUOTIENT_CEILING(SDMMC_PLL_FREQ_IN_MHZ, 18));
    NvBootClocksConfigureClock(NvBootClocksClockId_SdmmcId,
        NVBOOT_CLOCKS_7_1_DIVIDER_BY(18, 0),
#ifdef NV_SDMMC4
        CLK_RST_CONTROLLER_CLK_SOURCE_SDMMC4_0_SDMMC4_CLK_SRC_PLLP_OUT0);
#else
        CLK_RST_CONTROLLER_CLK_SOURCE_SDMMC2_0_SDMMC2_CLK_SRC_PLLP_OUT0);
#endif

    // Enable the clock.
//    NvBootClocksSetEnable(NvBootClocksClockId_SdmmcId, NV_TRUE);

    // Remove the controller from Reset.
    NvBootResetSetEnable(NvBootResetDeviceId_SdmmcId, NV_FALSE);
    // Reset Controller's All registers.
    NV_BOOT_CHECK_ERROR(HwSdmmcResetController());
    
    // Set Internal Clock Enable and SDCLK Frequency Select in the 
    // Clock Control register.
    StcReg = NV_DRF_DEF(SDMMC, SW_RESET_TIMEOUT_CTRL_CLOCK_CONTROL,
                INTERNAL_CLOCK_EN, OSCILLATE) | 
             NV_DRF_DEF(SDMMC, SW_RESET_TIMEOUT_CTRL_CLOCK_CONTROL,
                SDCLK_FREQUENCYSELECT, DIV64);

    NV_SDMMC_WRITE(SW_RESET_TIMEOUT_CTRL_CLOCK_CONTROL, StcReg);
    // Wait till clock is stable.
    NV_BOOT_CHECK_ERROR(HwSdmmcWaitForClkStable());
    // Reload reg value after clock is stable.
    NV_SDMMC_READ(SW_RESET_TIMEOUT_CTRL_CLOCK_CONTROL, StcReg);
    
    // Find out what volatage is supported.
    NV_SDMMC_READ(CAPABILITIES, CapabilityReg);
    PowerControlHostReg = 0;
    if (NV_DRF_VAL(SDMMC, CAPABILITIES, VOLTAGE_SUPPORT_3_3_V, CapabilityReg))
    {
        PowerControlHostReg |= NV_DRF_DEF(SDMMC, POWER_CONTROL_HOST,
                                SD_BUS_VOLTAGE_SELECT, V3_3);
    }
    else if (NV_DRF_VAL(SDMMC, CAPABILITIES, VOLTAGE_SUPPORT_3_0_V, CapabilityReg))
    {
        PowerControlHostReg |= NV_DRF_DEF(SDMMC, POWER_CONTROL_HOST,
                                SD_BUS_VOLTAGE_SELECT, V3_0);
    }
    else
    {
        PowerControlHostReg |= NV_DRF_DEF(SDMMC, POWER_CONTROL_HOST,
                                SD_BUS_VOLTAGE_SELECT, V1_8);
    }
    // Enable bus power.
    PowerControlHostReg |= NV_DRF_DEF(SDMMC, POWER_CONTROL_HOST, SD_BUS_POWER, 
                            POWER_ON);
    NV_SDMMC_WRITE(POWER_CONTROL_HOST, PowerControlHostReg);
    s_SdmmcContext->HostSupportsHighSpeedMode = NV_FALSE;
    if (NV_DRF_VAL(SDMMC, CAPABILITIES, HIGH_SPEED_SUPPORT, CapabilityReg))
        s_SdmmcContext->HostSupportsHighSpeedMode = NV_TRUE;
    PRINT_SDMMC_MESSAGES("HostSupportsHighSpeedMode=%d\n", 
        s_SdmmcContext->HostSupportsHighSpeedMode);
    // Enable Command complete, Transfer complete and various error events.
    IntStatusEnableReg = 
        NV_DRF_DEF(SDMMC, INTERRUPT_STATUS_ENABLE, DATA_END_BIT_ERR, ENABLE) |
        NV_DRF_DEF(SDMMC, INTERRUPT_STATUS_ENABLE, DATA_CRC_ERR, ENABLE) |
        NV_DRF_DEF(SDMMC, INTERRUPT_STATUS_ENABLE, DATA_TIMEOUT_ERR, ENABLE) |
        NV_DRF_DEF(SDMMC, INTERRUPT_STATUS_ENABLE, COMMAND_INDEX_ERR, ENABLE) |
        NV_DRF_DEF(SDMMC, INTERRUPT_STATUS_ENABLE, COMMAND_END_BIT_ERR, ENABLE) |
        NV_DRF_DEF(SDMMC, INTERRUPT_STATUS_ENABLE, COMMAND_CRC_ERR, ENABLE) |
        NV_DRF_DEF(SDMMC, INTERRUPT_STATUS_ENABLE, COMMAND_TIMEOUT_ERR, ENABLE) |
        NV_DRF_DEF(SDMMC, INTERRUPT_STATUS_ENABLE, CARD_REMOVAL, ENABLE) |
        NV_DRF_DEF(SDMMC, INTERRUPT_STATUS_ENABLE, CARD_INSERTION, ENABLE) |
        NV_DRF_DEF(SDMMC, INTERRUPT_STATUS_ENABLE, DMA_INTERRUPT, ENABLE) |
        NV_DRF_DEF(SDMMC, INTERRUPT_STATUS_ENABLE, TRANSFER_COMPLETE, ENABLE) |
        NV_DRF_DEF(SDMMC, INTERRUPT_STATUS_ENABLE, COMMAND_COMPLETE, ENABLE);
    NV_SDMMC_WRITE(INTERRUPT_STATUS_ENABLE, IntStatusEnableReg);
    // This method resets card clock divisor. So, set it again.
    HwSdmmcSetCardClock(s_SdmmcContext->CurrentClockRate);
    HwSdmmcSetDataWidth(s_SdmmcContext->DataWidth);
    return NvBootError_Success;
}

static void
HwSdmmcReadResponse(
    SdmmcResponseType ResponseType,
    NvU32* pRespBuffer)
{
    NvU32* pTemp = pRespBuffer;
    
    switch (ResponseType)
    {
        case SdmmcResponseType_R1:
        case SdmmcResponseType_R1B:
        case SdmmcResponseType_R3:
        case SdmmcResponseType_R4:
        case SdmmcResponseType_R5:
        case SdmmcResponseType_R6:
        case SdmmcResponseType_R7:
            // bits 39:8 of response are mapped to 31:0.
            NV_SDMMC_READ(RESPONSE_R0_R1, *pTemp);
            break;
        case SdmmcResponseType_R2:
            // bits 127:8 of response are mapped to 119:0.
            NV_SDMMC_READ(RESPONSE_R0_R1, *pTemp);
            pTemp++;
            NV_SDMMC_READ(RESPONSE_R2_R3, *pTemp);
            pTemp++;
            NV_SDMMC_READ(RESPONSE_R4_R5, *pTemp);
            pTemp++;
            NV_SDMMC_READ(RESPONSE_R6_R7, *pTemp);
            break;
        case SdmmcResponseType_NoResponse:
        default:
            *pTemp = 0;
    }
}

static NvBootError HwSdmmcWaitForDataLineReady(void)
{
    NvU32 PresentState;
    NvU32 DataLineActive;
    NvU32 TimeOut = s_SdmmcContext->ReadTimeOutInUs;
    
// jz
//printf ("%s: TimeOut: %d\n", __FUNCTION__, TimeOut);
    while (TimeOut)
    {
        NV_SDMMC_READ(PRESENT_STATE, PresentState);
        DataLineActive = NV_DRF_VAL(SDMMC, PRESENT_STATE, DAT_LINE_ACTIVE, 
                            PresentState);
        if (!DataLineActive)
            break;
        NvBootUtilWaitUS(1);
        TimeOut--;
        if (!TimeOut)
        {
            PRINT_SDMMC_ERRORS("DataLineActive is not set to 0 and timed out\n");
            return NvBootError_HwTimeOut;
        }
    }
    return NvBootError_Success;
}

static NvBootError HwSdmmcWaitForCmdInhibitData(void)
{
    NvU32 PresentState;
    NvU32 CmdInhibitData;
    NvU32 TimeOut = s_SdmmcContext->ReadTimeOutInUs;
// jz
//printf("%s: TimeOut %d\n", __FUNCTION__, TimeOut);    
    while (TimeOut)
    {
        NV_SDMMC_READ(PRESENT_STATE, PresentState);
        // This bit is set to zero after busy line is deasserted.
        // For response R1b, need to wait for this.
        CmdInhibitData = NV_DRF_VAL(SDMMC, PRESENT_STATE, CMD_INHIBIT_DAT, 
                            PresentState);
        if (!CmdInhibitData)
            break;
        NvBootUtilWaitUS(1);
        TimeOut--;
        if (!TimeOut)
        {
            PRINT_SDMMC_ERRORS("CmdInhibitData is not set to 0 and timed out. Int State: 0x%08x\n", PresentState);
            return NvBootError_HwTimeOut;
        }
    }
    return NvBootError_Success;
}

static NvBootError HwSdmmcWaitForCmdInhibitCmd(void)
{
    NvU32 PresentState;
    NvU32 CmdInhibitCmd;
    NvU32 TimeOut = SDMMC_COMMAND_TIMEOUT_IN_US;
    
    // jz
//    NV_SDMMC_WRITE(INTERRUPT_STATUS_ENABLE, 0x007f0001); 
      NV_SDMMC_READ(INTERRUPT_STATUS_ENABLE, PresentState);
    while (TimeOut)
    {
        NV_SDMMC_READ(PRESENT_STATE, PresentState);
        // This bit is set to zero after response is received. So, response 
        // registers should be read only after this bit is cleared.
        CmdInhibitCmd = NV_DRF_VAL(SDMMC, PRESENT_STATE, CMD_INHIBIT_CMD, 
                            PresentState);
        if (!CmdInhibitCmd)
            break;
        NvBootUtilWaitUS(1);
        TimeOut--;
        if (!TimeOut)
        {
            PRINT_SDMMC_ERRORS("CmdInhibitCmd is not set to 0 and timed out\n");
            return NvBootError_HwTimeOut;
        }
    }
    return NvBootError_Success;
}

static NvBootError HwSdmmcWaitForCommandComplete(NvU32 *Status)
{
    NvU32 CommandDone;
    NvU32 InterruptStatus;
    NvU32 TimeOutCounter = SDMMC_COMMAND_TIMEOUT_IN_US;
    NvU32 ErrorMask = NV_DRF_DEF(SDMMC, INTERRUPT_STATUS, COMMAND_INDEX_ERR,
                        ERR) |
                      NV_DRF_DEF(SDMMC, INTERRUPT_STATUS, COMMAND_END_BIT_ERR,
                        END_BIT_ERR_GENERATED) |
                      NV_DRF_DEF(SDMMC, INTERRUPT_STATUS, COMMAND_CRC_ERR,
                        CRC_ERR_GENERATED) |
                      NV_DRF_DEF(SDMMC, INTERRUPT_STATUS, COMMAND_TIMEOUT_ERR,
                        TIMEOUT);

    while (TimeOutCounter)
    {
        NV_SDMMC_READ(INTERRUPT_STATUS, InterruptStatus);
        *Status = InterruptStatus;
        CommandDone = NV_DRF_VAL(SDMMC, INTERRUPT_STATUS, CMD_COMPLETE,
                        InterruptStatus);
        if (InterruptStatus & ErrorMask)
        {
            PRINT_SDMMC_ERRORS("Errors in HwSdmmcWaitForCommandComplete, "
                "InterruptStatus = 0x%x\n", InterruptStatus);
            return NvBootError_DeviceError;
        }

        if (CommandDone)
            break;
        NvBootUtilWaitUS(1);
        TimeOutCounter--;
        if (!TimeOutCounter)
        {
            PRINT_SDMMC_ERRORS("Timed out in HwSdmmcWaitForCommandComplete\n");
            return NvBootError_HwTimeOut;
        }
    }
    return NvBootError_Success;
}

static NvBootError HwSdmmcIssueAbortCommand(void)
{
    NvBootError e;
    NvU32 retries = 2;
    NvU32 CommandXferMode;
    NvU32 InterruptStatus;
    NvU32* pSdmmcResponse = &s_SdmmcContext->SdmmcResponse[0];
    
    PRINT_SDMMC_MESSAGES("\n     Sending Abort CMD%d\n", 
        SdmmcCommand_StopTransmission);
    
    CommandXferMode = 
        NV_DRF_NUM(SDMMC, CMD_XFER_MODE, COMMAND_INDEX,
            SdmmcCommand_StopTransmission) |
        NV_DRF_DEF(SDMMC, CMD_XFER_MODE, COMMAND_TYPE, ABORT) |
        NV_DRF_DEF(SDMMC, CMD_XFER_MODE, DATA_PRESENT_SELECT, NO_DATA_TRANSFER) |
        NV_DRF_DEF(SDMMC, CMD_XFER_MODE, CMD_INDEX_CHECK_EN, ENABLE) |
        NV_DRF_DEF(SDMMC, CMD_XFER_MODE, CMD_CRC_CHECK_EN, ENABLE) |
        NV_DRF_DEF(SDMMC, CMD_XFER_MODE, RESP_TYPE_SELECT, RESP_LENGTH_48BUSY) | 
        NV_DRF_DEF(SDMMC, CMD_XFER_MODE, DATA_XFER_DIR_SEL, WRITE) |
        NV_DRF_DEF(SDMMC, CMD_XFER_MODE, BLOCK_COUNT_EN, DISABLE) |
        NV_DRF_DEF(SDMMC, CMD_XFER_MODE, DMA_EN, DISABLE);
    
    while (retries)
    {
        // Clear Status bits what ever is set.
        NV_SDMMC_READ(INTERRUPT_STATUS, InterruptStatus);
        NV_SDMMC_WRITE(INTERRUPT_STATUS, InterruptStatus);
        // This redundant read is for debug purpose.
        NV_SDMMC_READ(INTERRUPT_STATUS, InterruptStatus);
        NV_SDMMC_WRITE(ARGUMENT, 0);
        NV_SDMMC_WRITE(CMD_XFER_MODE, CommandXferMode);
        // Wait for the command to be sent out.if it fails, retry.
        e = HwSdmmcWaitForCommandComplete(&InterruptStatus);
        if (e == NvBootError_Success)
            break;
        HwSdmmcInitController();
        retries--;
    }
    if (retries)
    {
        // Wait till response is received from card.
        NV_BOOT_CHECK_ERROR(HwSdmmcWaitForCmdInhibitCmd());
        // Wait till busy line is deasserted by card. It is for R1b response.
        NV_BOOT_CHECK_ERROR(HwSdmmcWaitForCmdInhibitData());
        HwSdmmcReadResponse(SdmmcResponseType_R1B, pSdmmcResponse);
    }
    return e;
}

static NvBootError HwSdmmcRecoverControllerFromErrors(NvBool IsDataCmd)
{
    NvU32 StcReg;
    NvU32 PresentState;
    NvU32 ResetInProgress;
    NvU32 InterruptStatus;
    NvU32 TimeOut = SDMMC_TIME_OUT_IN_US;
    NvU32 CommandError = NV_DRF_DEF(SDMMC, INTERRUPT_STATUS, COMMAND_INDEX_ERR,
                            ERR) |
                         NV_DRF_DEF(SDMMC, INTERRUPT_STATUS, COMMAND_END_BIT_ERR,
                            END_BIT_ERR_GENERATED) |
                         NV_DRF_DEF(SDMMC, INTERRUPT_STATUS, COMMAND_CRC_ERR,
                            CRC_ERR_GENERATED) |
                         NV_DRF_DEF(SDMMC, INTERRUPT_STATUS, COMMAND_TIMEOUT_ERR,
                            TIMEOUT);
    NvU32 DataError = NV_DRF_DEF(SDMMC, INTERRUPT_STATUS, DATA_END_BIT_ERR,
                        ERR) |
                      NV_DRF_DEF(SDMMC, INTERRUPT_STATUS, DATA_CRC_ERR,
                        ERR) |
                      NV_DRF_DEF(SDMMC, INTERRUPT_STATUS, DATA_TIMEOUT_ERR,
                        TIMEOUT);
    NvU32 DataStateMask = NV_DRF_DEF(SDMMC, PRESENT_STATE, DAT_3_0_LINE_LEVEL, 
                            DEFAULT_MASK);
    
    NV_SDMMC_READ(INTERRUPT_STATUS, InterruptStatus);
    NV_SDMMC_READ(SW_RESET_TIMEOUT_CTRL_CLOCK_CONTROL, StcReg);
    if (InterruptStatus & CommandError)
    {
        // Reset Command line.
        StcReg |= NV_DRF_DEF(SDMMC, SW_RESET_TIMEOUT_CTRL_CLOCK_CONTROL,
                    SW_RESET_FOR_CMD_LINE, RESETED);
        NV_SDMMC_WRITE(SW_RESET_TIMEOUT_CTRL_CLOCK_CONTROL, StcReg);
        // Wait till Reset is completed.
        while (TimeOut)
        {
            NV_SDMMC_READ(SW_RESET_TIMEOUT_CTRL_CLOCK_CONTROL, StcReg);
            ResetInProgress = NV_DRF_VAL(SDMMC, SW_RESET_TIMEOUT_CTRL_CLOCK_CONTROL,
                                SW_RESET_FOR_CMD_LINE, StcReg);
            if (!ResetInProgress)
                break;
            NvBootUtilWaitUS(1);
            TimeOut--;
        }
        if (!TimeOut)
        {
            PRINT_SDMMC_ERRORS("Reset Command line timed out.\n");
            return NvBootError_HwTimeOut;
        }
    }
    if (InterruptStatus & DataError)
    {
        // Reset Data line.
        StcReg |= NV_DRF_DEF(SDMMC, SW_RESET_TIMEOUT_CTRL_CLOCK_CONTROL,
                    SW_RESET_FOR_DAT_LINE, RESETED);
        NV_SDMMC_WRITE(SW_RESET_TIMEOUT_CTRL_CLOCK_CONTROL, StcReg);
        // Wait till Reset is completed.
        while (TimeOut)
        {
            NV_SDMMC_READ(SW_RESET_TIMEOUT_CTRL_CLOCK_CONTROL, StcReg);
            ResetInProgress = NV_DRF_VAL(SDMMC, SW_RESET_TIMEOUT_CTRL_CLOCK_CONTROL,
                                SW_RESET_FOR_DAT_LINE, StcReg);
            if (!ResetInProgress)
                break;
            NvBootUtilWaitUS(1);
            TimeOut--;
        }
        if (!TimeOut)
        {
            PRINT_SDMMC_ERRORS("Reset Data line timed out.\n");
            return NvBootError_HwTimeOut;
        }
    }
    // Clear Interrupt Status
    NV_SDMMC_WRITE(INTERRUPT_STATUS, InterruptStatus);
    // Issue abort command.
    if (IsDataCmd)
        (void)HwSdmmcIssueAbortCommand();
    // Wait for 40us as per spec.
    NvBootUtilWaitUS(40);
    // Read Present State register.
    NV_SDMMC_READ(PRESENT_STATE, PresentState);
    if ( (PresentState & DataStateMask) != DataStateMask )
    {
        // Before give up, try full reset once.
        HwSdmmcInitController();
        NV_SDMMC_READ(PRESENT_STATE, PresentState);
        if ( (PresentState & DataStateMask) != DataStateMask)
        {
            PRINT_SDMMC_ERRORS("Error Recovery Failed.\n");
            return NvBootError_DeviceError;
        }
    }
    return NvBootError_Success;
}

static void HwSdmmcAbortDataRead(void)
{
    NvU32 StcReg;
    NvU32 PresentState;
    NvU32 ResetInProgress;
    NvU32 TimeOut = SDMMC_TIME_OUT_IN_US;
    NvU32 DataStateMask = NV_DRF_DEF(SDMMC, PRESENT_STATE, DAT_3_0_LINE_LEVEL, 
                            DEFAULT_MASK);
    
    NV_SDMMC_READ(SW_RESET_TIMEOUT_CTRL_CLOCK_CONTROL, StcReg);
    // Reset Data line.
    StcReg |= NV_DRF_DEF(SDMMC, SW_RESET_TIMEOUT_CTRL_CLOCK_CONTROL,
                SW_RESET_FOR_DAT_LINE, RESETED);
    NV_SDMMC_WRITE(SW_RESET_TIMEOUT_CTRL_CLOCK_CONTROL, StcReg);
    // Wait till Reset is completed.
    while (TimeOut)
    {
        NV_SDMMC_READ(SW_RESET_TIMEOUT_CTRL_CLOCK_CONTROL, StcReg);
        ResetInProgress = NV_DRF_VAL(SDMMC, SW_RESET_TIMEOUT_CTRL_CLOCK_CONTROL,
                            SW_RESET_FOR_DAT_LINE, StcReg);
        if (!ResetInProgress)
            break;
        NvBootUtilWaitUS(1);
        TimeOut--;
    }
    if (!TimeOut)
    {
        PRINT_SDMMC_ERRORS("AbortDataRead-Reset Data line timed out.\n");
    }
    // Read Present State register.
    NV_SDMMC_READ(PRESENT_STATE, PresentState);
    if ( (PresentState & DataStateMask) != DataStateMask )
    {
        // Before give up, try full reset once.
        HwSdmmcInitController();
        NV_SDMMC_READ(PRESENT_STATE, PresentState);
        if ( (PresentState & DataStateMask) != DataStateMask)
        {
            PRINT_SDMMC_ERRORS("Error Recovery Failed.\n");
        }
    }
}

static 
NvBootError 
EmmcVerifyResponse(
    SdmmcCommand command, 
    NvBool AfterCmdExecution)
{
    NvU32* pResp = &s_SdmmcContext->SdmmcResponse[0];
    NvU32 AddressOutOfRange = NV_DRF_VAL(SDMMC, CS, ADDRESS_OUT_OF_RANGE, pResp[0]);
    NvU32 AddressMisalign = NV_DRF_VAL(SDMMC, CS, ADDRESS_MISALIGN, pResp[0]);
    NvU32 BlockLengthError = NV_DRF_VAL(SDMMC, CS, BLOCK_LEN_ERROR, pResp[0]);
    NvU32 CommandCrcError = NV_DRF_VAL(SDMMC, CS, COM_CRC_ERROR, pResp[0]);
    // For illegal commands, card does not respond. It can
    // be known only through CMD13.
    NvU32 IllegalCommand = NV_DRF_VAL(SDMMC, CS, ILLEGAL_CMD, pResp[0]);
    NvU32 CardInternalError = NV_DRF_VAL(SDMMC, CS, CC_ERROR, pResp[0]);
    NvU32 CardEccError = NV_DRF_VAL(SDMMC, CS, CARD_ECC_FAILED, pResp[0]);
    NvU32 SwitchError = NV_DRF_VAL(SDMMC, CS, SWITCH_ERROR, pResp[0]);
    NvBool BeforeCommandExecution = (AfterCmdExecution ? NV_FALSE : NV_TRUE);
    
    if (command == SdmmcCommand_ReadSingle)
    {
        if (BeforeCommandExecution)
        {
            // This is during response time.
            if ( AddressOutOfRange || AddressMisalign || BlockLengthError || 
                 CardInternalError )
            {
                PRINT_SDMMC_ERRORS("ReadSingle Operation failed.\n");
                return NvBootError_DeviceResponseError;
            }
        }
        else if (CommandCrcError || IllegalCommand || CardEccError)
        {
            return NvBootError_DeviceReadError;
        }
    }
    else if (command == SdmmcCommand_SetBlockLength)
    {
        if ( BeforeCommandExecution && (BlockLengthError || CardInternalError) )
        {
            // Either the argument of a SET_BLOCKLEN command exceeds the 
            // maximum value allowed for the card, or the previously defined 
            // block length is illegal for the current command 
            PRINT_SDMMC_ERRORS("SetBlockLength Operation failed.\n");
            return NvBootError_DeviceResponseError;
        }
    }
    else if (command == SdmmcCommand_Switch)
    {
        if ( AfterCmdExecution && (SwitchError || CommandCrcError) )
        {
            // If set, the card did not switch to the expected mode as 
            // requested by the SWITCH command.
            PRINT_SDMMC_ERRORS("Switch Operation failed.\n");
            return NvBootError_DeviceResponseError;
        }
    }
    else if (command == SdmmcCommand_EmmcSendExtendedCsd)
    {
        if (BeforeCommandExecution && CardInternalError)
        {
            PRINT_SDMMC_ERRORS("Send Extneded CSD Operation failed.\n");
            return NvBootError_DeviceResponseError;
        }
    }
    else if (command == SdmmcCommand_EsdSelectPartition)
    {
        if (AfterCmdExecution && AddressOutOfRange)
        {
            PRINT_SDMMC_ERRORS("EsdSelectPartition Out of range error.\n");
            return NvBootError_DeviceResponseError;
        }
    }
    return NvBootError_Success;
}

// controller for various responses along with contstant arguments.
//
// EMMC does not have responses R6, R7. SD only has additional R6 and R7 
// responses. EMMC R4 needs command and crc checks in response. 
// SD R4 doesn't need command and crc checks in response.
// Do we use R4 Response at all? No, it isn't as of now in this driver.
//
// This array indicates what responses need Command Index check.
//  NR,R1,R2,R3,R4,R5,R6,R7,R1b
// {0, 1, 0, 0, 0, 1, 1, 1, 1};
// 
// This array indicates what responses need Crc check.
//  NR,R1,R2,R3,R4,R5,R6,R7,R1b
// {0, 1, 1, 0, 0, 1, 1, 1, 1};
#define RESPONSE_DATA(type, index_check, crc_check)                        \
    ( NV_DRF_DEF(SDMMC, CMD_XFER_MODE, COMMAND_TYPE,       NORMAL)      | \
      NV_DRF_DEF(SDMMC, CMD_XFER_MODE, DATA_XFER_DIR_SEL,  READ)        | \
      NV_DRF_DEF(SDMMC, CMD_XFER_MODE, RESP_TYPE_SELECT,   type)        | \
      NV_DRF_DEF(SDMMC, CMD_XFER_MODE, CMD_INDEX_CHECK_EN, index_check) | \
      NV_DRF_DEF(SDMMC, CMD_XFER_MODE, CMD_CRC_CHECK_EN,   crc_check) )

static const NvU32 s_ResponseDataArray[] =
{
    RESPONSE_DATA(NO_RESPONSE,        DISABLE, DISABLE), // None
    RESPONSE_DATA(RESP_LENGTH_48,     ENABLE,  ENABLE ), // R1
    RESPONSE_DATA(RESP_LENGTH_136,    DISABLE, ENABLE ), // R2
    RESPONSE_DATA(RESP_LENGTH_48,     DISABLE, DISABLE), // R3
    RESPONSE_DATA(RESP_LENGTH_48,     DISABLE, DISABLE), // R4
    RESPONSE_DATA(RESP_LENGTH_48,     ENABLE,  ENABLE ), // R5
    RESPONSE_DATA(RESP_LENGTH_48,     ENABLE,  ENABLE ), // R6
    RESPONSE_DATA(RESP_LENGTH_48,     ENABLE,  ENABLE ), // R7
    RESPONSE_DATA(RESP_LENGTH_48BUSY, ENABLE,  ENABLE )  // R1b
};

static NvBootError
HwSdmmcSendCommand(
    SdmmcCommand CommandIndex,
    NvU32 CommandArg,
    SdmmcResponseType ResponseType,
    NvBool IsDataCmd,
    NvBool IsWrite)
{
    NvBootError e;
    NvU32 retries = 3;
    NvU32 CommandXferMode;
    NvU32 InterruptStatus;
    NvU32* pSdmmcResponse = &s_SdmmcContext->SdmmcResponse[0];
    
    NV_ASSERT(ResponseType < SdmmcResponseType_Num);
    PRINT_SDMMC_MESSAGES("Sending CMD%d\n", CommandIndex);
    PRINT_SDMMC_MESSAGES("    Cmd Index=0x%x, Arg=0x%x, RespType=%d, data=%d\n",
        CommandIndex, CommandArg, ResponseType, IsDataCmd);
    // Wait till Controller is ready.
    NV_BOOT_CHECK_ERROR(HwSdmmcWaitForCmdInhibitCmd());
    
    CommandXferMode = 
        NV_DRF_NUM(SDMMC, CMD_XFER_MODE, COMMAND_INDEX, CommandIndex) |
        NV_DRF_NUM(SDMMC, CMD_XFER_MODE, DATA_PRESENT_SELECT, (IsDataCmd ? 1 : 0)) |
        s_ResponseDataArray[ResponseType] |
        NV_DRF_NUM(SDMMC, CMD_XFER_MODE, BLOCK_COUNT_EN, (IsDataCmd ? 1 : 0)) |
        NV_DRF_NUM(SDMMC, CMD_XFER_MODE, DMA_EN, (IsDataCmd ? 1 : 0));
    
    if (IsWrite) {
        CommandXferMode &= ~(NV_DRF_NUM(SDMMC, CMD_XFER_MODE, DATA_XFER_DIR_SEL, 1));
    }

    while (retries)
    {
        // Clear Status bits what ever is set.
        NV_SDMMC_READ(INTERRUPT_STATUS, InterruptStatus);
        NV_SDMMC_WRITE(INTERRUPT_STATUS, InterruptStatus);
        // This redundant read is for debug purpose.
        NV_SDMMC_READ(INTERRUPT_STATUS, InterruptStatus);
        NV_SDMMC_WRITE(ARGUMENT, CommandArg);
        NV_SDMMC_WRITE(CMD_XFER_MODE, CommandXferMode);
        // Wait for the command to be sent out. If it fails, retry.
        e = HwSdmmcWaitForCommandComplete(&InterruptStatus);
        if (e == NvBootError_Success)
            break;

        // Recover Controller from Errors.
        HwSdmmcRecoverControllerFromErrors(IsDataCmd);
        retries--;
// jz
printf("%s: retry %d\n", __FUNCTION__, retries);
    }
    if (retries)
    {
        // Wait till response is received from card.
        NV_BOOT_CHECK_ERROR(HwSdmmcWaitForCmdInhibitCmd());
        if (ResponseType == SdmmcResponseType_R1B)
            // Wait till busy line is deasserted by card.
            NV_BOOT_CHECK_ERROR(HwSdmmcWaitForCmdInhibitData());
        HwSdmmcReadResponse(ResponseType, pSdmmcResponse);
    }
    return e;
}

static NvBootError EmmcSendSwitchCommand(NvU32 CmdArg)
{
    NvBootError e;
    SdmmcResponseType Response = SdmmcResponseType_R1B;
    
    if (s_FuseInfo.CardType == NvBootSdmmcCardType_Esd)
        Response = SdmmcResponseType_R1;
    NV_BOOT_CHECK_ERROR(HwSdmmcSendCommand(SdmmcCommand_Switch,
        CmdArg, Response, NV_FALSE, NV_FALSE));
    NV_BOOT_CHECK_ERROR(HwSdmmcSendCommand(SdmmcCommand_SendStatus,
        s_SdmmcContext->CardRca, SdmmcResponseType_R1, NV_FALSE, NV_FALSE));
    NV_BOOT_CHECK_ERROR(EmmcVerifyResponse(SdmmcCommand_Switch, NV_TRUE));
    return e;
}

static NvBootError EmmcSelectAccessRegion(SdmmcAccessRegion region)
{
    NvU32 CmdArg;
    NvBootError e;
    NV_ASSERT(region < SdmmcAccessRegion_Num);
    
    CmdArg = s_SdmmcContext->BootConfig & (~EMMC_SWITCH_SELECT_PARTITION_MASK);
    CmdArg |= region;
    CmdArg <<= EMMC_SWITCH_SELECT_PARTITION_OFFSET;
    CmdArg |= EMMC_SWITCH_SELECT_PARTITION_ARG;
    NV_BOOT_CHECK_ERROR(EmmcSendSwitchCommand(CmdArg));
    s_SdmmcContext->CurrentAccessRegion = region;
    PRINT_SDMMC_MESSAGES("Selected Region=%d(1->BP1, 2->BP2, 0->User)\n", 
        region);
    return e;
}

static NvBootError SdmmcSelectAccessRegion(NvU32* Block, NvU32* Page)
{
    NvBootError e = NvBootError_Success;
    SdmmcAccessRegion region;
    NvU32 BlocksPerPartition = s_SdmmcContext->EmmcBootPartitionSize >> 
                               s_SdmmcContext->BlockSizeLog2;
    
    // If boot partition size is zero, then the card is either eSD or 
    // eMMC version is < 4.3.
    if (s_SdmmcContext->EmmcBootPartitionSize == 0)
    {
        s_SdmmcContext->CurrentAccessRegion = SdmmcAccessRegion_UserArea;
        return e;
    }
    // This will not work always, if the request is a multipage one.
    // But this driver never gets multipage requests.
    if ( (*Block) < BlocksPerPartition )
    {
        region = SdmmcAccessRegion_BootPartition1;
    }
    else if ( (*Block) < (BlocksPerPartition << 1) )
    {
        region = SdmmcAccessRegion_BootPartition2;
        *Block = (*Block) - BlocksPerPartition;
    }
    else
    {
        region = SdmmcAccessRegion_UserArea;
        *Block = (*Block) - (BlocksPerPartition << 1);
    }
    
    if (region != s_SdmmcContext->CurrentAccessRegion)
        NV_BOOT_CHECK_ERROR(EmmcSelectAccessRegion(region));
    return e;
}

NvBootError
NvBootSdmmcReadPage(
    const NvU32 Block,
    const NvU32 Page,
    NvU8 *pBuffer)
{
    NvBootError e;
    NvU32 CommandArg;
    NvU32 ActualBlockToRead;
    NvU32 Page2Access = Page;
    NvU32 Block2Access = Block;
    NvU32 PageSize = (1 << s_SdmmcContext->PageSizeLog2);
    
    NV_ASSERT(Page < (1 << s_SdmmcContext->PagesPerBlockLog2));
    NV_ASSERT(pBuffer != NULL);
    PRINT_SDMMC_MESSAGES("Read Block=%d, Page=%d\n", Block, Page);
#if NVRM
    s_SdmmcBitInfo->NumPagesRead++;
#endif
    if ( (s_IsBootModeDataValid == NV_TRUE) && (Block == 0) && (Page == 0) )
    {
        // the 0th page of 0th block will read in boot mode, if boot mode is
        // enabled. So, give it back from buffer.
        NvBootUtilMemcpy(pBuffer, &s_SdmmcContext->SdmmcBootModeBuffer[0], 
            PageSize);
        return NvBootError_Success;
    }
    
    // If data line ready times out, try to recover from errors.
    if (HwSdmmcWaitForDataLineReady() != NvBootError_Success)
        NV_BOOT_CHECK_ERROR(HwSdmmcRecoverControllerFromErrors(NV_TRUE));
    // Select access region.This will intern changes block and page addresses
    // based on the region the request falls in.
    NV_BOOT_CHECK_ERROR(SdmmcSelectAccessRegion(&Block2Access, &Page2Access));
    PRINT_SDMMC_MESSAGES("Region=%d(1->BP1, 2->BP2, 0->UP)Block2Access=%d, "
        "Page2Access=%d\n", s_SdmmcContext->CurrentAccessRegion, Block2Access, 
        Page2Access);
    // Send SET_BLOCKLEN(CMD16) Command.
    NV_BOOT_CHECK_ERROR(HwSdmmcSendCommand(SdmmcCommand_SetBlockLength,
        PageSize, SdmmcResponseType_R1, NV_FALSE, NV_FALSE));
    NV_BOOT_CHECK_ERROR(EmmcVerifyResponse(SdmmcCommand_SetBlockLength, 
        NV_FALSE));
    // Find out the Block to read from MoviNand.
    ActualBlockToRead = (Block2Access << s_SdmmcContext->PagesPerBlockLog2) + 
                        Page2Access;
    /*
     * If block to read is beyond card's capacity, then some Emmc cards are 
     * responding with error back and continue to work. Some are not responding 
     * for this and for subsequent valid operations also.
     */
    //if (ActualBlockToRead >= s_SdmmcContext->NumOfBlocks)
    //    return NvBootError_IllegalParameter;
    // Set number of blocks to read to 1.
    HwSdmmcSetNumOfBlocks(PageSize, 1);
    // Set up command arg.
    if (s_SdmmcContext->IsHighCapacityCard)
        CommandArg = ActualBlockToRead;
    else
        CommandArg = (ActualBlockToRead << s_SdmmcContext->PageSizeLog2);
    PRINT_SDMMC_MESSAGES("ActualBlockToRead=%d, CommandArg=%d\n", 
        ActualBlockToRead, CommandArg);
    // Setup Dma.
    HwSdmmcSetupDma(pBuffer, PageSize);
    // Send command to card.
// jz
//printf("%s: send read cmd %d\n", __FUNCTION__, (int)SdmmcCommand_ReadSingle);
    NV_BOOT_CHECK_ERROR(HwSdmmcSendCommand(SdmmcCommand_ReadSingle,
                            CommandArg, SdmmcResponseType_R1, NV_TRUE, NV_FALSE));
    // If response fails, return error. Nothing to clean up.
// jz
//printf("%s: verify cmd response from cmd %d\n", __FUNCTION__, (int)SdmmcCommand_ReadSingle);
    NV_BOOT_CHECK_ERROR_CLEANUP(EmmcVerifyResponse(SdmmcCommand_ReadSingle, 
        NV_FALSE));
    s_SdmmcContext->DeviceStatus = NvBootDeviceStatus_ReadInProgress;
    s_SdmmcContext->ReadStartTime = NvBootUtilGetTimeUS();
    return e;
fail:
    HwSdmmcAbortDataRead();
    return e;
}

NvBootDeviceStatus NvBootSdmmcQueryStatus(void)
{
    NvBootError e;
    NvU32 SdmaAddress;
    NvU32 TransferDone = 0;
    NvU32 InterruptStatusReg;
    NvU32 ErrorMask = NV_DRF_DEF(SDMMC, INTERRUPT_STATUS, DATA_END_BIT_ERR, ERR) |
                      NV_DRF_DEF(SDMMC, INTERRUPT_STATUS, DATA_CRC_ERR, ERR) |
                      NV_DRF_DEF(SDMMC, INTERRUPT_STATUS, DATA_TIMEOUT_ERR,
                        TIMEOUT) |
                      NV_DRF_DEF(SDMMC, INTERRUPT_STATUS, COMMAND_INDEX_ERR,
                        ERR) |
                      NV_DRF_DEF(SDMMC, INTERRUPT_STATUS, COMMAND_END_BIT_ERR,
                        END_BIT_ERR_GENERATED) |
                      NV_DRF_DEF(SDMMC, INTERRUPT_STATUS, COMMAND_CRC_ERR,
                        CRC_ERR_GENERATED) |
                      NV_DRF_DEF(SDMMC, INTERRUPT_STATUS, COMMAND_TIMEOUT_ERR,
                        TIMEOUT);
    NvU32 DmaBoundaryInterrupt = NV_DRF_DEF(SDMMC, INTERRUPT_STATUS, 
                                    DMA_INTERRUPT,GEN_INT);
    NvU32 DataTimeOutError = NV_DRF_DEF(SDMMC, INTERRUPT_STATUS, DATA_TIMEOUT_ERR,
                                TIMEOUT);
// jz    
//printf("%s\n", __FUNCTION__);
    if (s_SdmmcContext->DeviceStatus == NvBootDeviceStatus_ReadInProgress)
    {
        // Check whether Transfer is done.
        NV_SDMMC_READ(INTERRUPT_STATUS, InterruptStatusReg);
        TransferDone = NV_DRF_VAL(SDMMC, INTERRUPT_STATUS, XFER_COMPLETE,
                            InterruptStatusReg);
        // Check whether there are any errors.
        if (InterruptStatusReg & ErrorMask)
        {
            if ( (InterruptStatusReg & ErrorMask) == DataTimeOutError)
                s_SdmmcContext->DeviceStatus = NvBootDeviceStatus_DataTimeout;
            else
            {
                s_SdmmcContext->DeviceStatus = NvBootDeviceStatus_CrcFailure;
#if NVRM
                s_SdmmcBitInfo->NumCrcErrors++;
#endif
            }
            // Recover from errors here.
            (void)HwSdmmcRecoverControllerFromErrors(NV_TRUE);
        }
        else if (InterruptStatusReg & DmaBoundaryInterrupt)
        {
            // Need to clear this DMA boundary interrupt and write SDMA address
            // again. Otherwise controller doesn't go ahead.
            NV_SDMMC_WRITE(INTERRUPT_STATUS, DmaBoundaryInterrupt);
            NV_SDMMC_READ(SYSTEM_ADDRESS, SdmaAddress);
            NV_SDMMC_WRITE(SYSTEM_ADDRESS, SdmaAddress);
        }
        else if (TransferDone)
        {
            s_SdmmcContext->DeviceStatus = NvBootDeviceStatus_Idle;
            NV_SDMMC_WRITE(INTERRUPT_STATUS, InterruptStatusReg);
            if (s_SdmmcContext->BootModeReadInProgress == NV_FALSE)
            {
                // Check Whether there is any read ecc error.
                e = HwSdmmcSendCommand(SdmmcCommand_SendStatus,
                        s_SdmmcContext->CardRca, SdmmcResponseType_R1, NV_FALSE, NV_FALSE);
                if (e == NvBootError_Success)
                    e = EmmcVerifyResponse(SdmmcCommand_ReadSingle, NV_TRUE);
                if (e != NvBootError_Success)
                    s_SdmmcContext->DeviceStatus = NvBootDeviceStatus_ReadFailure;
            }
        }
        else if (NvBootUtilElapsedTimeUS(s_SdmmcContext->ReadStartTime) >
                 s_SdmmcContext->ReadTimeOutInUs)
        {
            s_SdmmcContext->DeviceStatus = NvBootDeviceStatus_ReadFailure;
        }
    }
    return s_SdmmcContext->DeviceStatus;
}

NvBootError SdmmcWritePage(const NvU32 Block, const NvU32 Page, NvU8 *pBuffer)
{
    NvU32 OrgStcReg;
    NvU32 ModStcReg;
    NvBootError e;
    NvU32 CommandArg;
    NvU32 ActualBlockToRead;
    NvU32 Page2Access = Page;
    NvBootDeviceStatus Status;
    NvU32 Block2Access = Block;
    NvU32 PageSize = (1 << s_SdmmcContext->PageSizeLog2);

    NV_ASSERT(Page < (1 << s_SdmmcContext->PagesPerBlockLog2));
    NV_ASSERT(pBuffer != NULL);
    PRINT_SDMMC_MESSAGES("Write Block=%d, Page=%d\n", Block, Page);

    NV_SDMMC_READ(SW_RESET_TIMEOUT_CTRL_CLOCK_CONTROL, OrgStcReg);
    ModStcReg = NV_FLD_SET_DRF_NUM(SDMMC, SW_RESET_TIMEOUT_CTRL_CLOCK_CONTROL,
                DATA_TIMEOUT_COUNTER_VALUE, 0xE, OrgStcReg);
    NV_SDMMC_WRITE(SW_RESET_TIMEOUT_CTRL_CLOCK_CONTROL, ModStcReg);

    // If data line ready times out, try to recover from errors.
    if (HwSdmmcWaitForDataLineReady() != NvBootError_Success)
        NV_BOOT_CHECK_ERROR(HwSdmmcRecoverControllerFromErrors(NV_TRUE));
    // Select access region.This will intern changes block and page addresses
    // based on the region the request falls in.
    NV_BOOT_CHECK_ERROR(SdmmcSelectAccessRegion(&Block2Access, &Page2Access));
    PRINT_SDMMC_MESSAGES("Region=%d(1->BP1, 2->BP2, 0->UP)Block2Access=%d, "
        "Page2Access=%d\n", s_SdmmcContext->CurrentAccessRegion, Block2Access,
        Page2Access);
     // Send SET_BLOCKLEN(CMD16) Command.
    NV_BOOT_CHECK_ERROR(HwSdmmcSendCommand(SdmmcCommand_SetBlockLength,
        (1 << s_SdmmcContext->PageSizeLog2), SdmmcResponseType_R1, NV_FALSE,
        NV_FALSE));
    NV_BOOT_CHECK_ERROR(EmmcVerifyResponse(SdmmcCommand_SetBlockLength,
        NV_FALSE));
    // Find out the Block to read from MoviNand.
    ActualBlockToRead = (Block2Access << s_SdmmcContext->PagesPerBlockLog2) +
                        Page2Access;
    /*
     * If block to write is beyond card's capacity, then some Emmc cards are 
     * responding with error back and continue to work. Some are not responding 
     * for this and for subsequent valid operations also.
     */
    //if (ActualBlockToRead >= s_SdmmcContext->NumOfBlocks)
    //    return NvBootError_IllegalParameter;
    // Set number of blocks to write to 1.
    HwSdmmcSetNumOfBlocks(PageSize, 1);
    // Set up command arg.
    if (s_SdmmcContext->IsHighCapacityCard)
        CommandArg = ActualBlockToRead;
    else
        CommandArg = (ActualBlockToRead << s_SdmmcContext->PageSizeLog2);
    PRINT_SDMMC_MESSAGES("ActualBlockToRead=%d, CommandArg=%d\n",
        ActualBlockToRead, CommandArg);
    // Setup Dma.
    HwSdmmcSetupDma(pBuffer, PageSize);
    // Send command to card.
    NV_BOOT_CHECK_ERROR(HwSdmmcSendCommand((SdmmcCommand)SdmmcCommand_WriteSingle,
                            CommandArg, SdmmcResponseType_R1, NV_TRUE, NV_TRUE));
    // If response fails, return error. Nothing to clean up.
    NV_BOOT_CHECK_ERROR_CLEANUP(EmmcVerifyResponse((SdmmcCommand)SdmmcCommand_WriteSingle,
        NV_FALSE));
    s_SdmmcContext->DeviceStatus = NvBootDeviceStatus_ReadInProgress;
    s_SdmmcContext->ReadStartTime = NvBootUtilGetTimeUS();

    do
    {
        Status = NvBootSdmmcQueryStatus();
        if (Status == NvBootDeviceStatus_Idle)
            break;
        else if ( (Status == NvBootDeviceStatus_ReadFailure) ||
                  (Status == NvBootDeviceStatus_DataTimeout) )
        {
            PRINT_SDMMC_ERRORS(" Write failed. Block=%d\n", Block);
            e = NvBootError_DeviceReadError;
            break;
        }
        else if (Status == NvBootDeviceStatus_CrcFailure)
        {
            PRINT_SDMMC_ERRORS(" Write crc error B=%d, P=%d\n", Block, Page);
        }
    } while (1);

    NV_SDMMC_WRITE(SW_RESET_TIMEOUT_CTRL_CLOCK_CONTROL, OrgStcReg);
    return e;
fail:
    HwSdmmcAbortDataRead();
    return e;
}

NvBootError SdmmcReadPage(const NvU32 Block, const NvU32 Page, NvU8 *pBuffer)
{
    NvBootError e;
    NvBootDeviceStatus Status;

    NV_BOOT_CHECK_ERROR(NvBootSdmmcReadPage(Block,Page, pBuffer));

    do
    {
        Status = NvBootSdmmcQueryStatus();
        if (Status == NvBootDeviceStatus_Idle)
            break;
        else if ( (Status == NvBootDeviceStatus_ReadFailure) ||
                  (Status == NvBootDeviceStatus_DataTimeout) )
        {
            PRINT_SDMMC_ERRORS (" Read failed. Block=%d\n", Block);
            e = NvBootError_DeviceReadError;
            break;
        }
    } while (1);
    return e;
}

static NvBool HwSdmmcIsCardPresent(void)
{
    NvU32 CardStable;
    NvU32 PresentState;
    NvU32 CardInserted = 0;
    NvU32 TimeOut = SDMMC_TIME_OUT_IN_US;
    
    while (TimeOut)
    {
        NV_SDMMC_READ(PRESENT_STATE, PresentState);
        CardStable = NV_DRF_VAL(SDMMC, PRESENT_STATE, CARD_STATE_STABLE, 
                        PresentState);
        if (CardStable)
        {
            CardInserted = NV_DRF_VAL(SDMMC, PRESENT_STATE, CARD_INSERTED, 
                            PresentState);
            break;
        }
        NvBootUtilWaitUS(1);
        TimeOut--;
    }
    PRINT_SDMMC_ERRORS("Card is %s stable\n", CardStable ? "":"not");
    PRINT_SDMMC_ERRORS("Card is %s present\n", CardInserted ? "":"not");
    return (CardInserted ? NV_TRUE : NV_FALSE);
}

void NvBootSdmmcShutdown(void)
{
    NvU32 StcReg;
    NvU32 PowerControlHostReg;
    
    PRINT_SDMMC_MESSAGES("SdmmcShutdown\n");

    // Stop the clock to SDMMC card.
    NV_SDMMC_READ(SW_RESET_TIMEOUT_CTRL_CLOCK_CONTROL, StcReg);
    StcReg = NV_FLD_SET_DRF_DEF(SDMMC, SW_RESET_TIMEOUT_CTRL_CLOCK_CONTROL,
                SD_CLOCK_EN, DISABLE, StcReg);
    NV_SDMMC_WRITE(SW_RESET_TIMEOUT_CTRL_CLOCK_CONTROL, StcReg);
    // Disable the bus power.
    NV_SDMMC_READ(POWER_CONTROL_HOST, PowerControlHostReg);
    PowerControlHostReg = NV_FLD_SET_DRF_DEF(SDMMC, POWER_CONTROL_HOST, 
                            SD_BUS_POWER, POWER_OFF, PowerControlHostReg);
    NV_SDMMC_WRITE(POWER_CONTROL_HOST, PowerControlHostReg);
    
    // Keep the controller in reset and disable the clock.
    NvBootResetSetEnable(NvBootResetDeviceId_SdmmcId, NV_TRUE);
    NvBootClocksSetEnable(NvBootClocksClockId_SdmmcId, NV_FALSE);
    s_SdmmcContext = NULL;
}

static NvBootError EmmcReadDataInBootMode(NvU8* pBuffer, NvU32 NumOfBlocks)
{
    NvBootError e;
    NvU32 BootControl;
    NvU32 CommandXferMode;
    NvU32 InterruptStatus;
    NvBootDeviceStatus DevStatus;
    NvU32 TimeOut = s_SdmmcContext->ReadTimeOutInUs;
    
    if (s_IsBootModeDataValid == NV_TRUE)
        return NvBootError_Success;
    HwSdmmcSetDataWidth(s_SdmmcContext->DataWidth);
    // Set card clock to 20MHz.
    HwSdmmcSetCardClock(NvBootSdmmcCardClock_20MHz);
    HwSdmmcSetNumOfBlocks(1 << SDMMC_MAX_PAGE_SIZE_LOG_2, NumOfBlocks);
    HwSdmmcSetupDma(pBuffer, NumOfBlocks << SDMMC_MAX_PAGE_SIZE_LOG_2);
    /*
     * Set Boot Ack and Data time outs 50msec and 1sec.
     * 20MHz --> 50ns cycle time.
     * 0x1F4240 * 50ns = 50msec.
     * 0x1312D00 * 50ns = 1sec.
     */
    NV_SDMMC_WRITE(VENDOR_BOOT_ACK_TIMEOUT, 0xF4240);
    NV_SDMMC_WRITE(VENDOR_BOOT_DAT_TIMEOUT, 0x1312D00);
    // Setup Command Xfer reg.
    CommandXferMode = NV_DRF_DEF(SDMMC, CMD_XFER_MODE, DATA_PRESENT_SELECT, 
                        DATA_TRANSFER) |
                      NV_DRF_DEF(SDMMC, CMD_XFER_MODE, DATA_XFER_DIR_SEL, READ) |
                      NV_DRF_DEF(SDMMC, CMD_XFER_MODE, BLOCK_COUNT_EN, ENABLE) |
                      NV_DRF_DEF(SDMMC, CMD_XFER_MODE, DMA_EN, ENABLE);
    NV_SDMMC_WRITE_08(CMD_XFER_MODE, 0, (CommandXferMode & 0xF));
    NV_SDMMC_WRITE_08(CMD_XFER_MODE, 1, ((CommandXferMode >> 8) & 0xF));
    NV_SDMMC_WRITE_08(CMD_XFER_MODE, 2, ((CommandXferMode >> 16) & 0xF));
    // Wait till Controller is ready.
    NV_BOOT_CHECK_ERROR(HwSdmmcWaitForCmdInhibitCmd());
    // Wait till busy line is deasserted.
    NV_BOOT_CHECK_ERROR(HwSdmmcWaitForCmdInhibitData());
    // Setup Boot Control reg.
    BootControl = NV_DRF_DEF(SDMMC, VENDOR_BOOT_CNTRL, BOOT_ACK, ENABLE) | 
                  NV_DRF_DEF(SDMMC, VENDOR_BOOT_CNTRL, BOOT, ENABLE);
    NV_SDMMC_WRITE(VENDOR_BOOT_CNTRL, BootControl);
    // Wait for data receive.
    s_SdmmcContext->DeviceStatus = NvBootDeviceStatus_ReadInProgress;
    s_SdmmcContext->ReadStartTime = NvBootUtilGetTimeUS();
    s_SdmmcContext->BootModeReadInProgress = NV_TRUE;
    do
    {
        DevStatus = NvBootSdmmcQueryStatus();
    } while ( (DevStatus != NvBootDeviceStatus_Idle) && 
              (DevStatus == NvBootDeviceStatus_ReadInProgress) );
    s_SdmmcContext->BootModeReadInProgress = NV_FALSE;
    if (DevStatus != NvBootDeviceStatus_Idle)
    {
        while (TimeOut)
        {
            // Disable Boot mode.
            BootControl = NV_DRF_DEF(SDMMC, VENDOR_BOOT_CNTRL, BOOT_ACK, 
                            DISABLE) |
                          NV_DRF_DEF(SDMMC, VENDOR_BOOT_CNTRL, BOOT, DISABLE);
            NV_SDMMC_WRITE(VENDOR_BOOT_CNTRL, BootControl);
            NV_SDMMC_READ(VENDOR_BOOT_CNTRL, BootControl);
            if (!BootControl)
                break;
            TimeOut--;
        }
        // Clear Status bits what ever is set.
        NV_SDMMC_READ(INTERRUPT_STATUS, InterruptStatus);
        NV_SDMMC_WRITE(INTERRUPT_STATUS, InterruptStatus);
        return NvBootError_DeviceError;
    }
    // Boot mode is succesful. Don't try to read in boot mode again.
    s_IsBootModeDataValid = NV_TRUE;
    return e;
}

static void HwSdmmcEnableHighSpeed(NvBool Enable)
{
    NvU32 PowerControlHostReg = 0;
    
    NV_SDMMC_READ(POWER_CONTROL_HOST, PowerControlHostReg);
    PowerControlHostReg = NV_FLD_SET_DRF_NUM(SDMMC, POWER_CONTROL_HOST,
                            HIGH_SPEED_EN, ((Enable == NV_TRUE) ? 1 : 0), 
                            PowerControlHostReg);
    NV_SDMMC_WRITE(POWER_CONTROL_HOST, PowerControlHostReg);
}

static void HwSdmmcCalculateCardClockDivisor(void)
{
    NvU32 TotalClockDivisor = s_SdmmcContext->ClockDivisor;
    
    s_SdmmcContext->CardClockDivisor = 1;
    s_SdmmcContext->HighSpeedMode = NV_FALSE;
    if ( (s_SdmmcContext->HostSupportsHighSpeedMode == NV_FALSE) || 
         (s_SdmmcContext->CardSupportsHighSpeedMode == NV_FALSE) || 
         (s_SdmmcContext->SpecVersion < 4) )
    {
        // Either card or host doesn't support high speed. So reduce the clock
        // frequency if required.
        if (QUOTIENT_CEILING(SDMMC_PLL_FREQ_IN_MHZ, s_SdmmcContext->ClockDivisor) > 
            s_SdmmcContext->TranSpeedInMHz)
            s_SdmmcContext->ClockDivisor = 
            QUOTIENT_CEILING(SDMMC_PLL_FREQ_IN_MHZ, s_SdmmcContext->TranSpeedInMHz);
    }
    else
    {
        while (QUOTIENT_CEILING(SDMMC_PLL_FREQ_IN_MHZ, TotalClockDivisor) > 
                SDMMC_MAX_CLOCK_FREQUENCY_IN_MHZ)
        {
            s_SdmmcContext->CardClockDivisor <<= 1;
            TotalClockDivisor <<= 1;
        }
        if (QUOTIENT_CEILING(SDMMC_PLL_FREQ_IN_MHZ, TotalClockDivisor) > 
            s_SdmmcContext->TranSpeedInMHz)
            s_SdmmcContext->HighSpeedMode = NV_TRUE;
    }
    PRINT_SDMMC_MESSAGES("ClockDivisor=%d, CardClockDivisor=%d, "
        "HighSpeedMode=%d\n", s_SdmmcContext->ClockDivisor, 
        s_SdmmcContext->CardClockDivisor, s_SdmmcContext->HighSpeedMode);
}

static NvBool SdmmcIsCardInTransferState(void)
{
    NvBootError e;
    NvU32 CardState;
    NvU32* pResp = &s_SdmmcContext->SdmmcResponse[0];
    
    // Send SEND_STATUS(CMD13) Command.
    NV_BOOT_CHECK_ERROR_CLEANUP(HwSdmmcSendCommand(SdmmcCommand_SendStatus,
        s_SdmmcContext->CardRca, SdmmcResponseType_R1, NV_FALSE, NV_FALSE));
    // Extract the Card State from the Response.
    CardState = NV_DRF_VAL(SDMMC, CS, CURRENT_STATE, pResp[0]);
    if (CardState == SdmmcState_Tran) {
        PRINT_SDMMC_MESSAGES("Indeed in Tran State\n");
        return NV_TRUE;
    }
fail:
    PRINT_SDMMC_MESSAGES("Not in Tran State\n");
    return NV_FALSE;
}

static NvBootError EmmcGetOpConditions(void)
{
    NvBootError e;
    NvU32 StartTime;
    NvU32 OCRRegister = 0;
    NvU32 ElapsedTime = 0;
    NvU32* pSdmmcResponse = &s_SdmmcContext->SdmmcResponse[0];
    NvU32 Cmd1Arg = s_OcrVoltageRange[s_FuseInfo.VoltageRange];
    
    if (Cmd1Arg != EmmcOcrVoltageRange_QueryVoltage)
        Cmd1Arg |= SDMMC_CARD_CAPACITY_MASK;
    // jz
    Cmd1Arg = 0x40ff8080;
    StartTime = NvBootUtilGetTimeUS();
    // Send SEND_OP_COND(CMD1) Command.
    while (ElapsedTime <= SDMMC_OP_COND_TIMEOUT_IN_US)
    {
        NV_BOOT_CHECK_ERROR(HwSdmmcSendCommand(
            SdmmcCommand_EmmcSendOperatingConditions, Cmd1Arg,
            SdmmcResponseType_R3, NV_FALSE, NV_FALSE));
        // Extract OCR from Response.
        OCRRegister = pSdmmcResponse[SDMMC_OCR_RESPONSE_WORD];
        // Check for Card Ready.
        if (OCRRegister & SDMMC_OCR_READY_MASK)
            break;
        if (Cmd1Arg == EmmcOcrVoltageRange_QueryVoltage)
        {
            if (OCRRegister & EmmcOcrVoltageRange_HighVoltage)
            {
                Cmd1Arg = EmmcOcrVoltageRange_HighVoltage;
                s_SdmmcContext->IsHighVoltageRange = NV_TRUE;
#if NVRM
                s_SdmmcBitInfo->DiscoveredVoltageRange = 
                    EmmcOcrVoltageRange_HighVoltage;
#endif
            }
            else if (OCRRegister & EmmcOcrVoltageRange_LowVoltage)
            {
                Cmd1Arg = EmmcOcrVoltageRange_LowVoltage;
                s_SdmmcContext->IsHighVoltageRange = NV_FALSE;
#if NVRM
                s_SdmmcBitInfo->DiscoveredVoltageRange = 
                    EmmcOcrVoltageRange_LowVoltage;
#endif
            }
            else
            {
                ElapsedTime = NvBootUtilElapsedTimeUS(StartTime);
                continue;
            }
            Cmd1Arg |= SDMMC_CARD_CAPACITY_MASK;
            StartTime = NvBootUtilGetTimeUS();
            continue;
        }
        #if DEBUG_SDMMC
        ElapsedTime += 10000;
        #else
        ElapsedTime = NvBootUtilElapsedTimeUS(StartTime);
        #endif
        // Wait for ten milliseconds between commands. This to avoid 
        // sending cmd1 too many times.
        NvBootUtilWaitUS(10000);
    }
    if (ElapsedTime > SDMMC_OP_COND_TIMEOUT_IN_US)
    {
        PRINT_SDMMC_ERRORS("Timeout during CMD1\n");
        return NvBootError_HwTimeOut;
    }
    PRINT_SDMMC_MESSAGES("TimeTaken for CMD1=%dus\n", ElapsedTime);
    s_SdmmcContext->IsHighCapacityCard = ( (OCRRegister & 
                                           SDMMC_CARD_CAPACITY_MASK) ? 
                                           NV_TRUE : NV_FALSE );
    return e;
}

static NvBootError EsdGetOpConditions(void)
{
    NvBootError e;
    NvU32 StartTime;
    NvU32 Cmd8Arg = 0;
    NvU32 OCRRegister = 0;
    NvU32 ElapsedTime = 0;
    NvU32 CardCapacityStatus = 0;
    NvU32 HighCapacitySupport = 0;
    NvU32* pSdmmcResponse = &s_SdmmcContext->SdmmcResponse[0];
    NvU32 Cmd1Arg = s_OcrVoltageRange[s_FuseInfo.VoltageRange];
    
    // Send SEND_IF_COND(CMD8) Command.
    if ( (s_FuseInfo.VoltageRange == NvBootSdmmcVoltageRange_QueryVoltage) || 
         (s_FuseInfo.VoltageRange == NvBootSdmmcVoltageRange_HighVoltage) )
        Cmd8Arg = ESD_HOST_HIGH_VOLTAGE_RANGE | ESD_HOST_CHECK_PATTERN;
    else if (s_FuseInfo.VoltageRange == NvBootSdmmcVoltageRange_LowVoltage)
        Cmd8Arg = ESD_HOST_LOW_VOLTAGE_RANGE | ESD_HOST_CHECK_PATTERN;
    else if (s_FuseInfo.VoltageRange == NvBootSdmmcVoltageRange_DualVoltage)
        Cmd8Arg = ESD_HOST_DUAL_VOLTAGE_RANGE | ESD_HOST_CHECK_PATTERN;
    e = HwSdmmcSendCommand(SdmmcCommand_EsdSendInterfaceCondition,
            Cmd8Arg, SdmmcResponseType_R7, NV_FALSE, NV_FALSE);
    
    if (e != NvBootError_Success)
    {
        // Ver2.00 or later SD Memory Card(voltage mismatch) 
        // Or Ver1.xx SD Memory Card or MultiMediaCard.
        PRINT_SDMMC_MESSAGES("V2.0 or later SD (vol mm) or V1 SD/MMC\n");
    }
    else if ( ((pSdmmcResponse[0] & ESD_CMD8_RESPONSE_CHECK_PATTERN_MASK) |
              (pSdmmcResponse[0] & ESD_CMD8_RESPONSE_VHS_MASK)) == Cmd8Arg )
    {
        // Ver2.xx SD Memory Card.
        HighCapacitySupport = 1;
        PRINT_SDMMC_MESSAGES("V2.xx SD Card\n");
    }
    else
    {
        // Unusable Card. Try to Continue with identification and see 
        // if it passes.
        PRINT_SDMMC_MESSAGES("Unusable Card\n");
    }
    
    if (Cmd1Arg != EmmcOcrVoltageRange_QueryVoltage)
        Cmd1Arg |= (HighCapacitySupport << ESD_ACMD41_HIGH_CAPACITY_BIT_OFFSET);
    StartTime = NvBootUtilGetTimeUS();
    while (ElapsedTime <= SDMMC_OP_COND_TIMEOUT_IN_US)
    {
        // Send ESD_SEND_OP_COND(ACMD41) Command.
        NV_BOOT_CHECK_ERROR(HwSdmmcSendCommand(SdmmcCommand_EsdAppCommand,
            0, SdmmcResponseType_R1, NV_FALSE, NV_FALSE));
        NV_BOOT_CHECK_ERROR(HwSdmmcSendCommand(
            SdmmcCommand_EsdAppSendOperatingCondition, Cmd1Arg,
            SdmmcResponseType_R3, NV_FALSE, NV_FALSE));
        // Extract OCR from Response.
        OCRRegister = pSdmmcResponse[SDMMC_OCR_RESPONSE_WORD];
        // Check for Card Ready.
        if (OCRRegister & SDMMC_OCR_READY_MASK)
            break;
        if (Cmd1Arg == EmmcOcrVoltageRange_QueryVoltage)
        {
            if (OCRRegister & EmmcOcrVoltageRange_HighVoltage)
            {
                Cmd1Arg = EmmcOcrVoltageRange_HighVoltage;
                s_SdmmcContext->IsHighVoltageRange = NV_TRUE;
#if NVRM
                s_SdmmcBitInfo->DiscoveredVoltageRange = 
                    EmmcOcrVoltageRange_HighVoltage;
#endif
            }
            else if (OCRRegister & EmmcOcrVoltageRange_LowVoltage)
            {
                Cmd1Arg = EmmcOcrVoltageRange_LowVoltage;
                s_SdmmcContext->IsHighVoltageRange = NV_FALSE;
#if NVRM
                s_SdmmcBitInfo->DiscoveredVoltageRange = 
                    EmmcOcrVoltageRange_LowVoltage;
#endif
            }
            else
            {
                ElapsedTime = NvBootUtilElapsedTimeUS(StartTime);
                continue;
            }
            Cmd1Arg |= (HighCapacitySupport << ESD_ACMD41_HIGH_CAPACITY_BIT_OFFSET);
            StartTime = NvBootUtilGetTimeUS();
            continue;
        }
        #if DEBUG_SDMMC
        ElapsedTime += 10000;
        #else
        ElapsedTime = NvBootUtilElapsedTimeUS(StartTime);
        #endif
        // Wait for ten milliseconds between commands. This to avoid 
        // sending cmd1 too many times.
        NvBootUtilWaitUS(10000);
    }
    if (ElapsedTime > SDMMC_OP_COND_TIMEOUT_IN_US)
    {
        PRINT_SDMMC_ERRORS("Timeout during ACMD41\n");
        return NvBootError_HwTimeOut;
    }
    PRINT_SDMMC_MESSAGES("TimeTaken for CMD1=%dus\n", ElapsedTime);
    CardCapacityStatus = (OCRRegister & SDMMC_CARD_CAPACITY_MASK);
    s_SdmmcContext->IsHighCapacityCard = 
    ( (HighCapacitySupport && CardCapacityStatus) ? NV_TRUE : NV_FALSE );
    return e;
}

static NvBootError SdmmcGetCsd(void)
{
    NvU32 Mult;
    NvU32 CSize;
    NvBootError e;
    NvU32 CSizeMulti;
    NvU32* pResp = &s_SdmmcContext->SdmmcResponse[0];
    
    // Send SEND_CSD(CMD9) Command.
    NV_BOOT_CHECK_ERROR(HwSdmmcSendCommand(SdmmcCommand_SendCsd,
        s_SdmmcContext->CardRca, SdmmcResponseType_R2, NV_FALSE, NV_FALSE));
    // Extract the page size log2 from Response data.
    s_SdmmcContext->PageSizeLog2 = NV_DRF_VAL(EMMC, CSD, READ_BL_LEN, pResp[2]);
    s_SdmmcContext->PageSizeLog2ForCapacity = s_SdmmcContext->PageSizeLog2;
    s_SdmmcContext->BlockSizeLog2 = NVBOOT_SDMMC_BLOCK_SIZE_LOG2;
    /*
     * The page size can be 512, 1024, 2048 or 4096. this size can only be used 
     * for Card capacity calculation. For Read/Write operations, We must use 
     * 512 byte page size only.
     */
    // Restrict the reads to (1 << EMMC_MAX_PAGE_SIZE_LOG_2) byte reads.
    if (s_SdmmcContext->PageSizeLog2 > SDMMC_MAX_PAGE_SIZE_LOG_2)
        s_SdmmcContext->PageSizeLog2 = SDMMC_MAX_PAGE_SIZE_LOG_2;
    if (s_SdmmcContext->PageSizeLog2 == 0)
        return NvBootError_DeviceError;
    s_SdmmcContext->PagesPerBlockLog2 = (s_SdmmcContext->BlockSizeLog2 -
                                           s_SdmmcContext->PageSizeLog2);
    // Extract the Spec Version from Response data.
    s_SdmmcContext->SpecVersion = NV_DRF_VAL(EMMC, CSD, SPEC_VERS, pResp[3]);
    s_SdmmcContext->taac = NV_DRF_VAL(EMMC, CSD, TAAC, pResp[3]);
    s_SdmmcContext->nsac = NV_DRF_VAL(EMMC, CSD, NSAC, pResp[3]);
    s_SdmmcContext->TranSpeed = NV_DRF_VAL(EMMC, CSD, TRAN_SPEED, pResp[2]);
    
    // For <= Emmc v4.0, v4.1, v4.2 and < Esd v1.10.
    s_SdmmcContext->TranSpeedInMHz = 20;
    // For Emmc v4.3 and Esd 1.10 onwards.
    if (s_SdmmcContext->TranSpeed == EMMC_CSD_V4_3_TRAN_SPEED)
    {
        // For Esd, it is 25MHz.
        s_SdmmcContext->TranSpeedInMHz = 25;
        if (s_FuseInfo.CardType == NvBootSdmmcCardType_Emmc)
            // For Emmc, it is 26MHz.
            s_SdmmcContext->TranSpeedInMHz = 26;
    }
    if (s_SdmmcContext->SpecVersion >= 4)
        s_SdmmcContext->CardSupportsHighSpeedMode = NV_TRUE;
    // Fund out number of blocks in card.
    CSize = NV_DRF_VAL(EMMC, CSD, C_SIZE_0, pResp[1]);
    CSize |= (NV_DRF_VAL(EMMC, CSD, C_SIZE_1, pResp[2]) << 
             EMMC_CSD_C_SIZE_1_LEFT_SHIFT_OFFSET);
    CSizeMulti = NV_DRF_VAL(EMMC, CSD, C_SIZE_MULTI, pResp[1]);
    if ( (CSize == EMMC_CSD_MAX_C_SIZE) && 
         (CSizeMulti == EMMC_CSD_MAX_C_SIZE_MULTI) )
    {
        // Capacity is > 2GB and should be calculated from ECSD fields, 
        // which is done in EmmcGetExtCSD() method.
        PRINT_SDMMC_MESSAGES("SdmmcGetCsd:Capacity is > 2GB\n")
    }
    else
    {
        Mult = 1 << (CSizeMulti + 2);
        s_SdmmcContext->NumOfBlocks = (CSize + 1) * Mult * 
                               (1 << (s_SdmmcContext->PageSizeLog2ForCapacity - 
                                s_SdmmcContext->PageSizeLog2));
        PRINT_SDMMC_MESSAGES("Csd NumOfBlocks=%d\n", 
            s_SdmmcContext->NumOfBlocks);
    }
    
    PRINT_SDMMC_MESSAGES("Page size from Card=0x%x, 0x%x\n", 
        s_SdmmcContext->PageSizeLog2, (1 << s_SdmmcContext->PageSizeLog2));
    PRINT_SDMMC_MESSAGES("Emmc SpecVersion=0x%x\n", s_SdmmcContext->SpecVersion);
    PRINT_SDMMC_MESSAGES("taac=0x%x\n", s_SdmmcContext->taac);
    PRINT_SDMMC_MESSAGES("nsac=0x%x\n", s_SdmmcContext->nsac);
    PRINT_SDMMC_MESSAGES("TranSpeed=0x%x\n", s_SdmmcContext->TranSpeed);
    PRINT_SDMMC_MESSAGES("TranSpeedInMHz=%d\n", s_SdmmcContext->TranSpeedInMHz);
    PRINT_SDMMC_MESSAGES("CardCommandClasses=0x%x\n", NV_DRF_VAL(EMMC, CSD, 
        CCC, pResp[2]));
    PRINT_SDMMC_MESSAGES("CSize=0x%x, CSizeMulti=0x%x\n", CSize, CSizeMulti);
    return e;
}

static NvBootError EsdGetScr(void)
{
    NvBootError e;
    NvBootDeviceStatus DevStatus;
    
    // Send SET_BLOCKLEN(CMD16) Command.
    NV_BOOT_CHECK_ERROR(HwSdmmcSendCommand(SdmmcCommand_SetBlockLength,
        ESD_SCR_DATA_LENGTH, SdmmcResponseType_R1, NV_FALSE, NV_FALSE));
    NV_BOOT_CHECK_ERROR(EmmcVerifyResponse(SdmmcCommand_SetBlockLength, 
        NV_FALSE));
    HwSdmmcSetNumOfBlocks(ESD_SCR_DATA_LENGTH, 1);
    // Setup Dma.
    HwSdmmcSetupDma((NvU8*)s_SdmmcContext->SdmmcInternalBuffer, 
        ESD_SCR_DATA_LENGTH);
    // Send SEND_SCR(ACMD51) to get SCR, which gives card's spec version.
    NV_BOOT_CHECK_ERROR(HwSdmmcSendCommand(SdmmcCommand_EsdAppCommand,
        s_SdmmcContext->CardRca, SdmmcResponseType_R1, NV_FALSE, NV_FALSE));
    NV_BOOT_CHECK_ERROR(HwSdmmcSendCommand(SdmmcCommand_EsdAppSendScr,
                            0, SdmmcResponseType_R1, NV_TRUE, NV_FALSE));
    // If response fails, return error. Nothing to clean up.
    NV_BOOT_CHECK_ERROR(EmmcVerifyResponse(SdmmcCommand_EsdAppSendScr, NV_FALSE));
    s_SdmmcContext->DeviceStatus = NvBootDeviceStatus_ReadInProgress;
    s_SdmmcContext->ReadStartTime = NvBootUtilGetTimeUS();
    do
    {
        DevStatus = NvBootSdmmcQueryStatus();
    } while ( (DevStatus != NvBootDeviceStatus_Idle) && 
              (DevStatus == NvBootDeviceStatus_ReadInProgress) );
    if (DevStatus != NvBootDeviceStatus_Idle)
        return NvBootError_DeviceError;
    // Extract the Sd Spec Version from Response data.
    s_SdmmcContext->SpecVersion = NV_DRF_VAL(ESD, SCR, SD_SPEC, 
        s_SdmmcContext->SdmmcInternalBuffer[ESD_SCR_SD_SPEC_BYTE_OFFSET]);
    PRINT_SDMMC_MESSAGES("Sd SpecVersion=0x%x\n", s_SdmmcContext->SpecVersion);
    if (s_SdmmcContext->SpecVersion >= 1)
        s_SdmmcContext->CardSupportsHighSpeedMode = NV_TRUE;
    return NvBootError_Success;
}

static void EsdEnableHighSpeedMode(void)
{
    NvBootError e;
    NvBootDeviceStatus DevStatus = NvBootDeviceStatus_Idle;
    
    // Clear controller's high speed bit.
    HwSdmmcEnableHighSpeed(NV_FALSE);
    if (s_SdmmcContext->HighSpeedMode)
    {
        // Boost clock frequency to 20MHz as enable high speed command involves 
        // reading 512 bytes from card.
        HwSdmmcSetCardClock(NvBootSdmmcCardClock_20MHz);
        HwSdmmcSetNumOfBlocks((1 << s_SdmmcContext->PageSizeLog2), 1);
        // Setup Dma.
        HwSdmmcSetupDma((NvU8*)s_SdmmcContext->SdmmcInternalBuffer, 
            (1 << s_SdmmcContext->PageSizeLog2));
        // Send SWITCH(CMD6) command to select High Speed Mode.
        NV_BOOT_CHECK_ERROR_CLEANUP(EmmcSendSwitchCommand(ESD_HIGHSPEED_SET));
        s_SdmmcContext->DeviceStatus = NvBootDeviceStatus_ReadInProgress;
        s_SdmmcContext->ReadStartTime = NvBootUtilGetTimeUS();
        do
        {
            DevStatus = NvBootSdmmcQueryStatus();
        } while ( (DevStatus != NvBootDeviceStatus_Idle) && 
                  (DevStatus == NvBootDeviceStatus_ReadInProgress) );
        if (DevStatus == NvBootDeviceStatus_Idle)
            return;
    fail:
        // High speed mode enable failed.
        s_SdmmcContext->CardSupportsHighSpeedMode = NV_FALSE;
        // Find out clock divider for card clock again.
        HwSdmmcCalculateCardClockDivisor();
    }
}

void EsdSelectbootPartition(void)
{
    NvBootError e;
    NvU32 CmdArg = ESD_BOOT_PARTITION_ID;
    
    NV_BOOT_CHECK_ERROR_CLEANUP(HwSdmmcSendCommand(
        SdmmcCommand_EsdSelectPartition, CmdArg, SdmmcResponseType_R1B, NV_FALSE, NV_FALSE));
    NV_BOOT_CHECK_ERROR_CLEANUP(HwSdmmcSendCommand(SdmmcCommand_SendStatus, 
        s_SdmmcContext->CardRca, SdmmcResponseType_R1, NV_FALSE, NV_FALSE));
    NV_BOOT_CHECK_ERROR_CLEANUP(EmmcVerifyResponse(
        SdmmcCommand_EsdSelectPartition, NV_TRUE));
    PRINT_SDMMC_MESSAGES("Boot Partition Select Successful.\n");
#if NVRM
    s_SdmmcBitInfo->BootFromBootPartition = 1;
#endif
    return;
fail:
#if NVRM
    s_SdmmcBitInfo->BootFromBootPartition = 0;
#endif
    PRINT_SDMMC_MESSAGES("Boot Partition Select Failed.\n");
}

static NvBootError EmmcGetExtCsd(void)
{
    NvBootError e;
    NvBootDeviceStatus DevStatus;
    NvU8* pBuffer = (NvU8*)&s_SdmmcContext->SdmmcInternalBuffer[0];
    
    // Set num of blocks to read to 1.
    HwSdmmcSetNumOfBlocks((1 << s_SdmmcContext->PageSizeLog2), 1);
    // Setup Dma.
    HwSdmmcSetupDma((NvU8*)s_SdmmcContext->SdmmcInternalBuffer, 
        (1 << s_SdmmcContext->PageSizeLog2));
    // Send SEND_EXT_CSD(CMD8) command to get boot partition size.
    NV_BOOT_CHECK_ERROR(HwSdmmcSendCommand(SdmmcCommand_EmmcSendExtendedCsd,
        0, SdmmcResponseType_R1, NV_TRUE, NV_FALSE));
    // If response fails, return error. Nothing to clean up.
    NV_BOOT_CHECK_ERROR(EmmcVerifyResponse(SdmmcCommand_EmmcSendExtendedCsd, 
        NV_FALSE));
    s_SdmmcContext->DeviceStatus = NvBootDeviceStatus_ReadInProgress;
    s_SdmmcContext->ReadStartTime = NvBootUtilGetTimeUS();
    do
    {
        DevStatus = NvBootSdmmcQueryStatus();
    } while ( (DevStatus != NvBootDeviceStatus_Idle) && 
              (DevStatus == NvBootDeviceStatus_ReadInProgress) );
    if (DevStatus != NvBootDeviceStatus_Idle)
        return NvBootError_DeviceError;
    s_SdmmcContext->EmmcBootPartitionSize = 
        // The partition size comes in 128KB units.
        // Left shift it by 17 to get it multiplied by 128KB.
        (pBuffer[EMMC_ECSD_BOOT_PARTITION_SIZE_OFFSET] << 17);
    s_SdmmcContext->PowerClass26MHz360V = pBuffer[EMMC_ECSD_POWER_CL_26_360_OFFSET];
    s_SdmmcContext->PowerClass52MHz360V = pBuffer[EMMC_ECSD_POWER_CL_52_360_OFFSET];
    s_SdmmcContext->PowerClass26MHz195V = pBuffer[EMMC_ECSD_POWER_CL_26_195_OFFSET];
    s_SdmmcContext->PowerClass52MHz195V = pBuffer[EMMC_ECSD_POWER_CL_52_195_OFFSET];
    s_SdmmcContext->BootConfig = pBuffer[EMMC_ECSD_BOOT_CONFIG_OFFSET];
    if (s_SdmmcContext->IsHighCapacityCard)
    {
        s_SdmmcContext->NumOfBlocks = (pBuffer[EMMC_ECSD_SECTOR_COUNT_0_OFFSET] | 
                                      (pBuffer[EMMC_ECSD_SECTOR_COUNT_1_OFFSET] << 8) |
                                      (pBuffer[EMMC_ECSD_SECTOR_COUNT_2_OFFSET] << 16) | 
                                      (pBuffer[EMMC_ECSD_SECTOR_COUNT_3_OFFSET] << 24));
        PRINT_SDMMC_MESSAGES("Ecsd NumOfBlocks=%d\n", s_SdmmcContext->NumOfBlocks);
    }
    
    PRINT_SDMMC_MESSAGES("BootPartition Size=%d\n", 
        s_SdmmcContext->EmmcBootPartitionSize);
    PRINT_SDMMC_MESSAGES("PowerClass26MHz360V=%d, PowerClass52MHz360V=%d, "
        "PowerClass26MHz195V=%d, PowerClass52MHz195V=%d\n", 
        s_SdmmcContext->PowerClass26MHz360V, s_SdmmcContext->PowerClass52MHz360V, 
        s_SdmmcContext->PowerClass26MHz195V, s_SdmmcContext->PowerClass52MHz195V);
    PRINT_SDMMC_MESSAGES("CurrentPowerClass=%d, CardType=%d\n", 
        pBuffer[EMMC_ECSD_POWER_CLASS_OFFSET], pBuffer[EMMC_ECSD_CARD_TYPE_OFFSET]);
    return e;
}

static NvU32 EmmcGetPowerClass(void)
{
    NvU32 PowerClass;
    
    if (s_SdmmcContext->IsHighVoltageRange)
        PowerClass = s_SdmmcContext->HighSpeedMode ? 
                     s_SdmmcContext->PowerClass52MHz360V : 
                     s_SdmmcContext->PowerClass26MHz360V;
    else
        PowerClass = s_SdmmcContext->HighSpeedMode ? 
                     s_SdmmcContext->PowerClass52MHz195V : 
                     s_SdmmcContext->PowerClass26MHz195V;
    /*
     * In the above power class, lower 4 bits give power class requirement for
     * for 4-bit data width and upper 4 bits give power class requirement for
     * for 8-bit data width.
     */
    if (s_SdmmcContext->DataWidth == NvBootSdmmcDataWidth_4Bit)
        PowerClass = (PowerClass >> EMMC_ECSD_POWER_CLASS_4_BIT_OFFSET) & 
                     EMMC_ECSD_POWER_CLASS_MASK;
    else if (s_SdmmcContext->DataWidth == NvBootSdmmcDataWidth_8Bit)
        PowerClass = (PowerClass >> EMMC_ECSD_POWER_CLASS_8_BIT_OFFSET) & 
                     EMMC_ECSD_POWER_CLASS_MASK;
    else //if (s_SdmmcContext->DataWidth == NvBootSdmmcDataWidth_1Bit)
        PowerClass = 0;
    return PowerClass;
}

static NvBootError EmmcSetPowerClass(void)
{
    NvU32 CmdArg;
    NvU32 PowerClassToSet;
    NvBootError e = NvBootError_Success;
    
    PowerClassToSet = EmmcGetPowerClass();
    // Select best possible configuration here.
    while (PowerClassToSet > s_SdmmcContext->MaxPowerClassSupported)
    {
        if (s_SdmmcContext->HighSpeedMode)
        {
            // Disable high speed and see, if it can be supported.
            s_SdmmcContext->CardSupportsHighSpeedMode = NV_FALSE;
            // Find out clock divider for card clock again for normal speed.
            HwSdmmcCalculateCardClockDivisor();
        }
        else if (s_SdmmcContext->DataWidth == NvBootSdmmcDataWidth_8Bit)
            s_SdmmcContext->DataWidth = NvBootSdmmcDataWidth_4Bit;
        else if (s_SdmmcContext->DataWidth == NvBootSdmmcDataWidth_4Bit)
            s_SdmmcContext->DataWidth = NvBootSdmmcDataWidth_1Bit;
        PowerClassToSet = EmmcGetPowerClass();
    }
    if (PowerClassToSet)
    {
        PRINT_SDMMC_MESSAGES("Set Power Class to %d\n", PowerClassToSet);
        CmdArg = EMMC_SWITCH_SELECT_POWER_CLASS_ARG | 
                 (PowerClassToSet << EMMC_SWITCH_SELECT_POWER_CLASS_OFFSET);
        NV_BOOT_CHECK_ERROR(EmmcSendSwitchCommand(CmdArg));
    }
#if NVRM
    s_SdmmcBitInfo->PowerClassUnderUse = PowerClassToSet;
#endif
    return e;
}

static void EmmcEnableHighSpeed(void)
{
    NvBootError e;
    NvU8* pBuffer = (NvU8*)&s_SdmmcContext->SdmmcInternalBuffer[0];
    
    // Clear controller's high speed bit.
    HwSdmmcEnableHighSpeed(NV_FALSE);
    // Enable the High Speed Mode, if required.
    if (s_SdmmcContext->HighSpeedMode)
    {
        PRINT_SDMMC_MESSAGES("Set High speed to %d\n", 
            s_SdmmcContext->HighSpeedMode);
        NV_BOOT_CHECK_ERROR_CLEANUP(EmmcSendSwitchCommand(
            EMMC_SWITCH_HIGH_SPEED_ENABLE_ARG));
        // Set the clock for data transfer.
        HwSdmmcSetCardClock(NvBootSdmmcCardClock_DataTransfer);
        // Validate high speed mode bit from card here.
        NV_BOOT_CHECK_ERROR_CLEANUP(EmmcGetExtCsd());
        if (pBuffer[EMMC_ECSD_HS_TIMING_OFFSET])
        {
            // As per Hw team, it should not be enabled. See Hw bug number
            // AP15#353684/AP20#478599.
            //HwSdmmcEnableHighSpeed(NV_TRUE);
            return;
        }
    fail:
        // If enable high speed fails, run in normal speed.
        PRINT_SDMMC_ERRORS("EmmcEnableHighSpeed Failed\n");
        s_SdmmcContext->CardSupportsHighSpeedMode = NV_FALSE;
        // Find out clock divider for card clock again.
        HwSdmmcCalculateCardClockDivisor();
    }
}

static NvBootError EmmcSetBusWidth(void)
{
    NvU32 CmdArg;
    NvBootError e = NvBootError_Success;
    
    // Send SWITCH(CMD6) Command to select bus width.
    PRINT_SDMMC_MESSAGES("Change Data width to %d(0->1bit, 1->4bit,"
        " 2->8-bit)\n", s_SdmmcContext->DataWidth);
    CmdArg = EMMC_SWITCH_BUS_WIDTH_ARG | 
             (s_SdmmcContext->DataWidth << EMMC_SWITCH_BUS_WIDTH_OFFSET);
    NV_BOOT_CHECK_ERROR(EmmcSendSwitchCommand(CmdArg));
    HwSdmmcSetDataWidth(s_SdmmcContext->DataWidth);
    return e;
}

static NvBootError EmmcIdentifyCard(void)
{
    NvBootError e = NvBootError_Success;
    
    PRINT_SDMMC_MESSAGES("\r\n%s\n", __FUNCTION__);
    // Set Clock rate to 375KHz for identification of card.
    HwSdmmcSetCardClock(NvBootSdmmcCardClock_Identification);
    // Send GO_IDLE_STATE(CMD0) Command.
    NV_BOOT_CHECK_ERROR(HwSdmmcSendCommand(SdmmcCommand_GoIdleState,
        0, SdmmcResponseType_NoResponse, NV_FALSE, NV_FALSE));
    // This sends SEND_OP_COND(CMD1) Command and finds out address mode and 
    // capacity status.
    NV_BOOT_CHECK_ERROR(EmmcGetOpConditions());
    // Send ALL_SEND_CID(CMD2) Command.
    NV_BOOT_CHECK_ERROR(HwSdmmcSendCommand(SdmmcCommand_AllSendCid,
        0, SdmmcResponseType_R2, NV_FALSE, NV_FALSE));
#if NVRM
    // Copy card identification data to sdmmc bit info.
    NvBootUtilMemcpy(s_SdmmcBitInfo->Cid, s_SdmmcContext->SdmmcResponse, 
        sizeof(s_SdmmcBitInfo->Cid));
#endif
    // Set RCA to Card Here. It should be greater than 1 as JEDSD spec.
    s_SdmmcContext->CardRca = (2 << SDMMC_RCA_OFFSET);
    // Send SET_RELATIVE_ADDR(CMD3) Command.
    NV_BOOT_CHECK_ERROR(HwSdmmcSendCommand(SdmmcCommand_EmmcSetRelativeAddress,
        s_SdmmcContext->CardRca, SdmmcResponseType_R1, NV_FALSE, NV_FALSE));
    // Get Card specific data. We can get it at this stage of identification.
    NV_BOOT_CHECK_ERROR(SdmmcGetCsd());
    // Send SELECT/DESELECT_CARD(CMD7) Command to place the card in tran state.
    NV_BOOT_CHECK_ERROR(HwSdmmcSendCommand(SdmmcCommand_SelectDeselectCard,
        s_SdmmcContext->CardRca, SdmmcResponseType_R1, NV_FALSE, NV_FALSE));
    // Card should be in transfer state now. Confirm it.
    if (SdmmcIsCardInTransferState() == NV_FALSE)
        return NvBootError_DeviceError;
    PRINT_SDMMC_MESSAGES("Card identification is Successful\n");
    PRINT_SDMMC_MESSAGES("Config Card\n");
    // Find out clock divider for card clock and high speed mode requirement.
    HwSdmmcCalculateCardClockDivisor();

#if 0 // the following code will cause mmc read failure. jz    
    if (s_SdmmcContext->SpecVersion >= 4) // v4.xx
    {
        // Bus width can only be changed to 4-bit/8-bit after required power class 
        // is set. To get Power class, we need to read Ext CSD. As we don't know 
        // the power class required for 4-bit/8-bit, we need to read Ext CSD 
        // with 1-bit data width.
        PRINT_SDMMC_MESSAGES("Set Data width to 1-bit for ECSD\n");
        HwSdmmcSetDataWidth(NvBootSdmmcDataWidth_1Bit);
        // Set data clock rate to 20MHz.
        HwSdmmcSetCardClock(NvBootSdmmcCardClock_20MHz);
        // It is valid for v4.xx and above cards only.
        // EmmcGetExtCsd() Finds out boot partition size also.
        NV_BOOT_CHECK_ERROR(EmmcGetExtCsd());
        // Select the power class now.
        NV_BOOT_CHECK_ERROR(EmmcSetPowerClass());
        // Enable the High Speed Mode, if required.
        EmmcEnableHighSpeed();
    }
#endif
    
    // Set the clock for data transfer.
    HwSdmmcSetCardClock(NvBootSdmmcCardClock_DataTransfer);
    // Set bus width.
    NV_BOOT_CHECK_ERROR(EmmcSetBusWidth());
    
    // Select boot partition1 here.
    if (s_SdmmcContext->EmmcBootPartitionSize != 0)
        NV_BOOT_CHECK_ERROR(EmmcSelectAccessRegion(
            SdmmcAccessRegion_BootPartition1));
#if NVRM
    s_SdmmcBitInfo->BootFromBootPartition = 
                                  s_SdmmcContext->EmmcBootPartitionSize ? 1 : 0;
#endif
    return e;
}

static NvBootError EsdIdentifyCard(void)
{
    NvBootError e;
    NvU32* pSdmmcResponse = &s_SdmmcContext->SdmmcResponse[0];
    
    PRINT_SDMMC_MESSAGES("\n%s\n", __FUNCTION__);
    // Set Clock rate to 375KHz for identification of card.
    HwSdmmcSetCardClock(NvBootSdmmcCardClock_Identification);
    // Send GO_IDLE_STATE(CMD0) Command.
PRINT_SDMMC_MESSAGES("\n%s: SendCmd0\n", __FUNCTION__);
    NV_BOOT_CHECK_ERROR(HwSdmmcSendCommand(SdmmcCommand_GoIdleState,
        0, SdmmcResponseType_NoResponse, NV_FALSE, NV_FALSE));

    // Sends SEND_IF_COND(CMD8) Command and ACMD41.
PRINT_SDMMC_MESSAGES("\n%s: SendCmd8\n", __FUNCTION__);
    NV_BOOT_CHECK_ERROR(EsdGetOpConditions());
    // Send ALL_SEND_CID(CMD2) Command.
PRINT_SDMMC_MESSAGES("\n%s: SendCmd2\n", __FUNCTION__);
    NV_BOOT_CHECK_ERROR(HwSdmmcSendCommand(SdmmcCommand_AllSendCid,
        0, SdmmcResponseType_R2, NV_FALSE, NV_FALSE));
#if NVRM
    // Copy card identification data to sdmmc bit info.
    NvBootUtilMemcpy(s_SdmmcBitInfo->Cid, s_SdmmcContext->SdmmcResponse, 
        sizeof(s_SdmmcBitInfo->Cid));
#endif
    // Get RCA Here.
    // Send SEND_RELATIVE_ADDR(CMD3) Command.
PRINT_SDMMC_MESSAGES("\n%s: SendCmd3\n", __FUNCTION__);
    NV_BOOT_CHECK_ERROR(HwSdmmcSendCommand(SdmmcCommand_EsdSendRelativeAddress,
        0, SdmmcResponseType_R6, NV_FALSE, NV_FALSE));
    s_SdmmcContext->CardRca = (pSdmmcResponse[0] >> 16);
    s_SdmmcContext->CardRca = (s_SdmmcContext->CardRca << SDMMC_RCA_OFFSET);
    // Get Card specific data. We can get it at this stage of identification.
    NV_BOOT_CHECK_ERROR(SdmmcGetCsd());
    // Send SELECT/DESELECT_CARD(CMD7) Command.
PRINT_SDMMC_MESSAGES("\n%s: SendCmd7\n", __FUNCTION__);
    NV_BOOT_CHECK_ERROR(HwSdmmcSendCommand(SdmmcCommand_SelectDeselectCard,
        s_SdmmcContext->CardRca, SdmmcResponseType_R1B, NV_FALSE, NV_FALSE));
    // Card should be in transfer state now. Verify it.
    if (SdmmcIsCardInTransferState() == NV_FALSE)
        return NvBootError_DeviceError;
    PRINT_SDMMC_MESSAGES("Card is identification is  Successful\n");
    PRINT_SDMMC_MESSAGES("Config Card\n");
    PRINT_SDMMC_MESSAGES("Set Data width to 1-bit for SCR\n");
    HwSdmmcSetDataWidth(NvBootSdmmcDataWidth_1Bit);
    // Get Sd card's SCR.
    NV_BOOT_CHECK_ERROR(EsdGetScr());
    // Send SWITCH(ACMD6) Command to select bus width.
    NV_BOOT_CHECK_ERROR(HwSdmmcSendCommand(SdmmcCommand_EsdAppCommand,
        s_SdmmcContext->CardRca, SdmmcResponseType_R1, NV_FALSE, NV_FALSE));
    NV_BOOT_CHECK_ERROR( EmmcSendSwitchCommand(
        ((s_SdmmcContext->DataWidth == NvBootSdmmcDataWidth_4Bit) ? 
        ESD_DATA_WIDTH_4BIT : ESD_DATA_WIDTH_1BIT)) );
    HwSdmmcSetDataWidth(s_SdmmcContext->DataWidth);
    // It finds out clock divider for card clock and high speed mode support.
    HwSdmmcCalculateCardClockDivisor();
    // Enable the High Speed Mode, if required.
    EsdEnableHighSpeedMode();
    // Set data clock rate.
    HwSdmmcSetCardClock(NvBootSdmmcCardClock_DataTransfer);
    s_SdmmcContext->EmmcBootPartitionSize = 0;
    // Select Boot Partition here, if it exists. If doesn't exist user area will
    // be accessed.
    EsdSelectbootPartition();
    return e;
}

NvBootError
NvBootSdmmcInit(
    const NvBootSdmmcParams *Params,
    NvBootSdmmcContext *Context)
{
    NvBootError e = NvBootError_Success;
    NV_ASSERT(Params != NULL);
    NV_ASSERT(Context != NULL);
    NV_ASSERT(Params->ClockDivider >= SDMMC_MIN_CLOCK_DIVIDER_SUPPORTED &&
        Params->ClockDivider <= SDMMC_MAX_CLOCK_DIVIDER_SUPPORTED);
    NV_ASSERT( (Params->DataWidth == NvBootSdmmcDataWidth_4Bit) ||
        (Params->DataWidth == NvBootSdmmcDataWidth_8Bit) );

// for debug only    
//    SdmmcGetParamsTest();
//    return e;


    // Stash the pointer to the context structure.
    s_SdmmcContext = Context;
    s_SdmmcContext->taac = 0;
    s_SdmmcContext->nsac = 0;
    s_SdmmcContext->ClockDivisor = Params->ClockDivider;
    s_SdmmcContext->DataWidth = Params->DataWidth;
    s_SdmmcContext->MaxPowerClassSupported = Params->MaxPowerClassSupported;
    s_SdmmcContext->CardSupportsHighSpeedMode = NV_FALSE;
    s_SdmmcContext->ReadTimeOutInUs = SDMMC_READ_TIMEOUT_IN_US;
    s_SdmmcContext->EmmcBootPartitionSize = 0;
    s_SdmmcContext->CurrentClockRate = NvBootSdmmcCardClock_Identification;
    s_SdmmcContext->CurrentAccessRegion = SdmmcAccessRegion_Unknown;
    s_SdmmcContext->BootModeReadInProgress = NV_FALSE;

    // Initialize the Hsmmc Hw controller.
    NV_BOOT_CHECK_ERROR(HwSdmmcInitController());

    // Check whether card is present. If not, return from here itself.
    if (HwSdmmcIsCardPresent() == NV_FALSE)
    {
        NvBootSdmmcShutdown();
        return NvBootError_DeviceError;
    }

    if (s_FuseInfo.DisableBootMode == NV_FALSE)
    {
        PRINT_SDMMC_MESSAGES("BootMode Enabled\n");

        e = EmmcReadDataInBootMode(&s_SdmmcContext->SdmmcBootModeBuffer[0], 1);

        if (e != NvBootError_Success)
        {
            // Reset data line.
            NV_BOOT_CHECK_ERROR(HwSdmmcInitController());
        }
    }

    if (s_FuseInfo.CardType == NvBootSdmmcCardType_Emmc)
    {
        PRINT_SDMMC_MESSAGES("Emmc Identify Card\n");
        e = EmmcIdentifyCard();
    }
    if ( (s_FuseInfo.CardType == NvBootSdmmcCardType_Esd) || 
         (e != NvBootError_Success) )
    {
        PRINT_SDMMC_MESSAGES("Esd Identify Card\n");
        s_SdmmcContext->DataWidth = NvBootSdmmcDataWidth_4Bit;
        s_FuseInfo.CardType = NvBootSdmmcCardType_Esd;
        NV_BOOT_CHECK_ERROR(EsdIdentifyCard());
    }
    s_SdmmcContext->DeviceStatus = NvBootDeviceStatus_Idle;

    PRINT_SDMMC_MESSAGES("\n%s: return %d\n", __FUNCTION__, e);
#ifdef NVRM
    s_SdmmcBitInfo->DiscoveredCardType = s_FuseInfo.CardType;
    s_SdmmcBitInfo->BootModeReadSuccessful = s_IsBootModeDataValid;
#endif
    return e;
}
