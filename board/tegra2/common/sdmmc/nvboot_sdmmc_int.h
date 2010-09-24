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
 * nvboot_sdmmc_int.h - Internal definitions for using Sdmmc as a
 * second level boot device.
 */

#ifndef INCLUDED_NVBOOT_SDMMC_INT_H
#define INCLUDED_NVBOOT_SDMMC_INT_H

#if defined(__cplusplus)
extern "C"
{
#endif

#define SDMMC_PLL_FREQ_IN_MHZ 432

/// These defines are for Emmc operations time out.
#define SDMMC_COMMAND_TIMEOUT_IN_US 100000
#define SDMMC_OP_COND_TIMEOUT_IN_US 1000000
//#define SDMMC_READ_TIMEOUT_IN_US 200000	// origianl
#define SDMMC_READ_TIMEOUT_IN_US 300000
#define SDMMC_TIME_OUT_IN_US 100000	

/// The following divider limits are based on the requirement that
/// SD Host controler clock should be in between 10MHz to 63MHz.
/// Max valid clock divider for PLLP clock source(432MHz).
#define SDMMC_MAX_CLOCK_DIVIDER_SUPPORTED 43
/// Min valid clock divider for PLLP clock source(432MHz).
#define SDMMC_MIN_CLOCK_DIVIDER_SUPPORTED 1
/// Max Power class supported.by card.
#define SDMMC_MAX_POWER_CLASS_SUPPORTED 16
/// This needs to be more than the BCT struct info.
#define NVBOOT_SDMMC_BLOCK_SIZE_LOG2 14

/// These defines are as per Emmc Spec.
#define EMMC_SWITCH_BUS_WIDTH_ARG 0x03b70000
#define EMMC_SWITCH_BUS_WIDTH_OFFSET 8
#define EMMC_SWITCH_1BIT_BUS_WIDTH_ARG 0x03b70000
#define EMMC_SWITCH_4BIT_BUS_WIDTH_ARG 0x03b70100
#define EMMC_SWITCH_8BIT_BUS_WIDTH_ARG 0x03b70200

#define EMMC_SWITCH_HIGH_SPEED_ENABLE_ARG  0x03b90100
#define EMMC_SWITCH_HIGH_SPEED_DISABLE_ARG 0x03b90000

#define EMMC_SWITCH_SELECT_PARTITION_ARG   0x03b30000
#define EMMC_SWITCH_SELECT_PARTITION_MASK  0x7
#define EMMC_SWITCH_SELECT_PARTITION_OFFSET 0x8

#define EMMC_SWITCH_SELECT_POWER_CLASS_ARG 0x03bb0000
#define EMMC_SWITCH_SELECT_POWER_CLASS_OFFSET 8

/// Emmc Extended CSD fields.
#define EMMC_ECSD_SECTOR_COUNT_0_OFFSET  212
#define EMMC_ECSD_SECTOR_COUNT_1_OFFSET  213
#define EMMC_ECSD_SECTOR_COUNT_2_OFFSET  214
#define EMMC_ECSD_SECTOR_COUNT_3_OFFSET  215
#define EMMC_ECSD_POWER_CL_26_360_OFFSET 203
#define EMMC_ECSD_POWER_CL_52_360_OFFSET 202
#define EMMC_ECSD_POWER_CL_26_195_OFFSET 201
#define EMMC_ECSD_POWER_CL_52_195_OFFSET 200
#define EMMC_ECSD_CARD_TYPE_OFFSET       196
#define EMMC_ECSD_POWER_CLASS_OFFSET     187
#define EMMC_ECSD_HS_TIMING_OFFSET       185
#define EMMC_ECSD_BUS_WIDTH              183
#define EMMC_ECSD_BOOT_CONFIG_OFFSET     179
#define EMMC_ECSD_BOOT_PARTITION_SIZE_OFFSET 226
#define EMMC_ECSD_POWER_CLASS_MASK 0xF
#define EMMC_ECSD_POWER_CLASS_4_BIT_OFFSET 0
#define EMMC_ECSD_POWER_CLASS_8_BIT_OFFSET 4

/// Emmc CSD fields
#define EMMC_CSD_0_SPEC_VERS_RANGE 21:18 // 117:114 of response register.
#define EMMC_CSD_0_TAAC_RANGE 15:8 // 111:104 of response register.
#define EMMC_CSD_TAAC_TIME_UNIT_MASK 0x07
#define EMMC_CSD_TAAC_TIME_VALUE_OFFSET 3
#define EMMC_CSD_TAAC_TIME_VALUE_MASK 0x0F
#define EMMC_CSD_0_NSAC_RANGE 7:0 // 103:96 of response register.
#define EMMC_CSD_0_TRAN_SPEED_RANGE 31:24 // 95:88 of response register.
#define EMMC_CSD_0_CCC_RANGE 23:12 // 87:76 of response register.
#define EMMC_CSD_0_READ_BL_LEN_RANGE 11:8 // 75:72 of response register.
#define EMMC_CSD_V4_3_TRAN_SPEED 0x32
#define EMMC_CSD_0_C_SIZE_0_RANGE 31:22 // 63:54 of response register.
#define EMMC_CSD_0_C_SIZE_1_RANGE 1:0 // 65:64 of response register.
#define EMMC_CSD_C_SIZE_1_LEFT_SHIFT_OFFSET 10
#define EMMC_CSD_0_C_SIZE_MULTI_RANGE 9:7 // 41:39 of response register.
#define EMMC_CSD_MAX_C_SIZE 0xFFF
#define EMMC_CSD_MAX_C_SIZE_MULTI 0x7

/// Card status fields.
#define SDMMC_CS_0_ADDRESS_OUT_OF_RANGE_RANGE 31:31
#define SDMMC_CS_0_ADDRESS_MISALIGN_RANGE 30:30
#define SDMMC_CS_0_BLOCK_LEN_ERROR_RANGE 29:29
#define SDMMC_CS_0_COM_CRC_ERROR_RANGE 23:23
#define SDMMC_CS_0_ILLEGAL_CMD_RANGE 22:22
/// Card internal ECC was applied but failed to correct the data.
#define SDMMC_CS_0_CARD_ECC_FAILED_RANGE 21:21
/// A card error occurred, which is not related to the host command.
#define SDMMC_CS_0_CC_ERROR_RANGE 20:20
#define SDMMC_CS_0_CURRENT_STATE_RANGE 12:9
#define SDMMC_CS_0_SWITCH_ERROR_RANGE 7:7

/// Defines as per ESD spec.
#define ESD_HOST_HIGH_VOLTAGE_RANGE 0x100
#define ESD_HOST_DUAL_VOLTAGE_RANGE 0x300
#define ESD_HOST_LOW_VOLTAGE_RANGE 0x200
#define ESD_HOST_CHECK_PATTERN 0xA5
#define ESD_CARD_OCR_VALUE 0x00FF8000
#define ESD_CMD8_RESPONSE_VHS_MASK 0xF00
#define ESD_CMD8_RESPONSE_CHECK_PATTERN_MASK 0xFF
#define ESD_ACMD41_HIGH_CAPACITY_BIT_OFFSET 30
#define ESD_DATA_WIDTH_1BIT 0
#define ESD_DATA_WIDTH_4BIT 2
#define ESD_HIGHSPEED_SET 0x80FFFF01

#define ESD_SCR_SD_SPEC_BYTE_OFFSET 0
#define ESD_SCR_0_SD_SPEC_RANGE 3:0

#define ESD_BOOT_PARTITION_ID (1 << 24)
#define ESD_SCR_DATA_LENGTH 8

/// Defines common to ESD and EMMC.
#define SDMMC_RCA_OFFSET 16
#define SDMMC_MAX_PAGE_SIZE_LOG_2 9
#define SDMMC_OCR_READY_MASK 0x80000000
#define SDMMC_CARD_CAPACITY_MASK 0x40000000

#define SDMMC_OCR_RESPONSE_WORD 0
#define SDMMC_MAX_CLOCK_FREQUENCY_IN_MHZ 52

/**
 * These defines are as per fuse bits.
 */

/**
 * Fuse Bit 0: indicates the data bus width.
 *      = 0 means the data bus width is 4 lines.
 *      = 1 means the data bus width is 8 lines.
 */
#define SDMMC_DEVICE_CONFIG_0_DATA_WIDTH_RANGE 0:0
/**
 * Fuse Bit 1: indicates the inserted card type.
 *      = 0 means the card is EMMC card.
 *      = 1 means the card is SD card.
 */
#define SDMMC_DEVICE_CONFIG_0_CARD_TYPE_RANGE 1:1
/**
 * Fuse Bits 2,3: indicates the voltage range to use.
 *      = 00 means query voltage range from the card.
 *      = 01 means use high voltage range.
 *      = 10 means use dual voltage range.
 *      = 11 means use low voltage range.
 */
#define SDMMC_DEVICE_CONFIG_0_VOLTAGE_RANGE_RANGE 3:2
/**
 * Fuse Bit 4: indicates the Boot mode support.
 *      = 0 means the Boot mode is enabled.
 *      = 1 means the Boot mode is disabled.
 */
#define SDMMC_DEVICE_CONFIG_0_DISABLE_BOOT_MODE_RANGE 4:4
/**
 * Fuse Bit 5: indicates the pinmux to select.
 *      = 0 means, use primary pinmux.
 *      = 1 means, use secondary pinmux.
 */
#define SDMMC_DEVICE_CONFIG_0_PINMUX_SELECTION_RANGE 5:5

/// Defines operating voltages for voltage range validation during card
/// identification. The values of the enumerant map to device configuration
/// fuse values.
typedef enum
{
    NvBootSdmmcVoltageRange_QueryVoltage = 0,
    NvBootSdmmcVoltageRange_HighVoltage,
    NvBootSdmmcVoltageRange_DualVoltage,
    NvBootSdmmcVoltageRange_LowVoltage,
    NvBootSdmmcVoltageRange_Num,
    NvBootSdmmcVoltageRange_Force32 = 0x7FFFFFFF,
} NvBootSdmmcVoltageRange;

/// Defines Emmc Ocr register values to use for operating voltage
/// range validation during card identification.  Note that the
/// code will OR in the bit which indicates support for sector-based
/// addressing.
typedef enum
{
    /// Query the voltage supported.
    EmmcOcrVoltageRange_QueryVoltage = 0x00000000,
    /// High voltage only.
    EmmcOcrVoltageRange_HighVoltage = 0x00ff8000,
    ///  Both voltages.
    EmmcOcrVoltageRange_DualVoltage = 0x00ff8080,
    ///  Low voltage only.
    EmmcOcrVoltageRange_LowVoltage  = 0x00000080,
} EmmcOcrVoltageRange;

#if defined(__cplusplus)
}
#endif

#endif /* #ifndef INCLUDED_NVBOOT_SDMMC_INT_H */
