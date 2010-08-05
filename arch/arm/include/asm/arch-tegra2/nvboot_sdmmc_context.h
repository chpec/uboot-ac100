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

/**
 * nvboot_sdmmc_context.h - Definitions for the Sdmmc context structure.
 */

#ifndef INCLUDED_NVBOOT_SDMMC_CONTEXT_H
#define INCLUDED_NVBOOT_SDMMC_CONTEXT_H

#include "nvboot_sdmmc_param.h"

#if defined(__cplusplus)
extern "C"
{
#endif

/// Sdmmc Response buffer size.
#define NVBOOT_SDMMC_RESPONSE_BUFFER_SIZE_IN_BYTES 16
/// Buffer size for reading Extended CSD.
#define NVBOOT_SDMMC_ECSD_BUFFER_SIZE_IN_BYTES 512
/// Buffer size for reading data in boot mode.
#define NVBOOT_SDMMC_BOOT_MODE_BUFFER_SIZE_IN_BYTES 512

/// Defines Command Responses of Emmc/Esd.
typedef enum
{
    SdmmcResponseType_NoResponse = 0,
    SdmmcResponseType_R1 = 1,
    SdmmcResponseType_R2 =2,
    SdmmcResponseType_R3 =3,
    SdmmcResponseType_R4 = 4,
    SdmmcResponseType_R5 = 5,
    SdmmcResponseType_R6 = 6,
    SdmmcResponseType_R7 = 7,
    SdmmcResponseType_R1B = 8,
    SdmmcResponseType_Num,
    SdmmcResponseType_Force32 = 0x7FFFFFFF
} SdmmcResponseType;

/**
 * Defines Emmc/Esd Commands as per Emmc/Esd spec's.
 * Emmc specific Commands starts with prefix Emmc and .Esd specific Commands 
 * starts with prefix Esd. Common commands has no prefix.
 */
typedef enum
{
    SdmmcCommand_GoIdleState = 0,
    SdmmcCommand_EmmcSendOperatingConditions = 1,
    SdmmcCommand_AllSendCid = 2,
    SdmmcCommand_EmmcSetRelativeAddress = 3,
    SdmmcCommand_EsdSendRelativeAddress = 3,
    SdmmcCommand_Switch = 6,
    SdmmcCommand_SelectDeselectCard = 7,
    SdmmcCommand_EsdSendInterfaceCondition = 8,
    SdmmcCommand_EmmcSendExtendedCsd = 8,
    SdmmcCommand_SendCsd = 9,
    SdmmcCommand_StopTransmission =12,
    SdmmcCommand_SendStatus = 13,
    SdmmcCommand_SetBlockLength = 16,
    SdmmcCommand_ReadSingle = 17,
    SdmmcCommand_WriteSingle = 24,
    SdmmcCommand_EsdAppSendOperatingCondition = 41,
    SdmmcCommand_EsdSelectPartition = 43,
    SdmmcCommand_EsdAppSendScr = 51,
    SdmmcCommand_EsdAppCommand = 55,
    SdmmcCommand_Force32 = 0x7FFFFFFF
} SdmmcCommand;

/// Defines Emmc/Esd card states.
typedef enum
{
    SdmmcState_Idle = 0,
    SdmmcState_Ready,
    SdmmcState_Ident,
    SdmmcState_Stby,
    SdmmcState_Tran,
    SdmmcState_Data,
    SdmmcState_Rcv,
    SdmmcState_Prg,
    SdmmcState_Force32 = 0x7FFFFFFF
} SdmmcState;

/// Defines Emmc card partitions.
typedef enum
{
    SdmmcAccessRegion_UserArea = 0,
    SdmmcAccessRegion_BootPartition1,
    SdmmcAccessRegion_BootPartition2,
    SdmmcAccessRegion_Num,
    SdmmcAccessRegion_Unknown,
    SdmmcAccessRegion_Force32 = 0x7FFFFFFF
} SdmmcAccessRegion;

/// Defines various card types supported.
typedef enum
{
    NvBootSdmmcCardType_Emmc = 0,
    NvBootSdmmcCardType_Esd,
    NvBootSdmmcCardType_Num,
    NvBootSdmmcCardType_Force32 = 0x7FFFFFFF
} NvBootSdmmcCardType;

/// Defines various clock rates to use for accessing the Emmc/Esd card at.
typedef enum
{
    NvBootSdmmcCardClock_Identification = 0,
    NvBootSdmmcCardClock_DataTransfer,
    NvBootSdmmcCardClock_20MHz,
    NvBootSdmmcCardClock_Num,
    NvBootSdmmcCardClock_Force32 = 0x7FFFFFFF
} NvBootSdmmcCardClock;

/**
 * The context structure for the Sdmmc driver.
 * A pointer to this structure is passed into the driver's Init() routine.
 * This pointer is the only data that can be kept in a global variable for
 * later reference.
 */
typedef struct NvBootSdmmcContextRec
{
    /// Block size.
    NvU8 BlockSizeLog2;
    /// Page size.
    NvU8 PageSizeLog2;
    /// No of pages per block;
    NvU8 PagesPerBlockLog2;
    /**
     * Clock divisor for the Sdmmc Controller Clock Source. 
     * Sdmmc Controller gets PLL_P clock, which is 432MHz, as a clock source.
     * If it is set to 18, then clock Frequency at which Emmc controller runs 
     * is 432/18 = 24MHz.
     */
    NvU8 ClockDivisor;
    /// Card's Relative Card Address.
    NvU32 CardRca;
    /// Data bus width.
    NvBootSdmmcDataWidth DataWidth;
    /// Response buffer. Define it as NvU32 to make it word aligned.
    NvU32 SdmmcResponse[NVBOOT_SDMMC_RESPONSE_BUFFER_SIZE_IN_BYTES / sizeof(NvU32)];
    /// Device status.
    NvBootDeviceStatus DeviceStatus;
    /// Holds the Movi Nand Read start time.
    NvU32 ReadStartTime;
    /**
     * This sub divides the clock that goes to card from controller.
     * If the controller clock is 24 Mhz and if this set to 2, then
     * the clock that goes to card is 12MHz.
     */
    NvU8 CardClockDivisor;
    /// Indicates whether to access the card in High speed mode.
    NvBool HighSpeedMode;
    /// Indicates whether the card is high capacity card or not.
    NvBool IsHighCapacityCard;
    /// Spec version.
    NvU8 SpecVersion;
    /// Holds Emmc Boot Partition size.
    NvU32 EmmcBootPartitionSize;
    /// Holds the current access region.
    SdmmcAccessRegion CurrentAccessRegion;
    /// Holds the current clock rate.
    NvBootSdmmcCardClock CurrentClockRate;
    /// Buffer for selecting High Speed, reading extended CSD and SCR.
    NvU8 SdmmcInternalBuffer[NVBOOT_SDMMC_ECSD_BUFFER_SIZE_IN_BYTES];
    /// Read access time1.
    NvU8 taac;
    /// Read access time2.
    NvU8 nsac;
    /// The clock frequency when not in high speed mode.
    NvU8 TranSpeed;
    /// Transfer speed in MHz.
    NvU8 TranSpeedInMHz;
    /// Indicates whether host supports high speed mode.
    NvBool HostSupportsHighSpeedMode;
    /// Indicates whether card supports high speed mode.
    NvBool CardSupportsHighSpeedMode;
    /// Indicates the page size to use for card capacity calculation.
    NvU8 PageSizeLog2ForCapacity;
    /// Power class for 26MHz at 3.6V.
    NvU8 PowerClass26MHz360V;
    /// Power class for 52MHz at 3.6V.
    NvU8 PowerClass52MHz360V;
    /// Power class for 26MHz at 1.95V.
    NvU8 PowerClass26MHz195V;
    /// Power class for 52MHz at 1.95V.
    NvU8 PowerClass52MHz195V;
    /// Boot Config from ExtCSD.
    NvU8 BootConfig;
    /// Indicates whether high voltage range is used for Card identification.
    NvBool IsHighVoltageRange;
    /// Max Power class supported by target board.
    NvU8 MaxPowerClassSupported;
    /// Number of blocks present in card.
    NvU32 NumOfBlocks;
    /// Holds read time out at current card clock frequency.
    NvU32 ReadTimeOutInUs;
    /// Buffer for storing the data read in boot mode.
    NvU8 SdmmcBootModeBuffer[NVBOOT_SDMMC_BOOT_MODE_BUFFER_SIZE_IN_BYTES];
    /// Flag to indicate whether reading is boot mode.
    NvBool BootModeReadInProgress;
} NvBootSdmmcContext;

#if defined(__cplusplus)
}
#endif

#endif /* #ifndef INCLUDED_NVBOOT_SDMMC_CONTEXT_H */

