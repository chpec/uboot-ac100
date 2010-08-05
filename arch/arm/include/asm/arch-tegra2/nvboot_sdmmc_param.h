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
 * Defines the parameters and data structure for eMMC and eSD devices, which
 * attach to the same SDMMC controller.
 */

#ifndef INCLUDED_NVBOOT_SDMMC_PARAM_H
#define INCLUDED_NVBOOT_SDMMC_PARAM_H

#if defined(__cplusplus)
extern "C"
{
#endif

/// Defines various data widths supported.
typedef enum
{
    /**
     * Specifies a 1 bit interface to eMMC.
     * Note that 1-bit data width is only for the driver's internal use.
     * Fuses doesn't provide option to select 1-bit data width.
     * The driver selects 1-bit internally based on need.
     * It is used for reading Extended CSD and when the power class
     * requirements of a card for 4-bit or 8-bit transfers are not
     * supported by the target board.
     */
    NvBootSdmmcDataWidth_1Bit = 0,

    /// Specifies a 4 bit interface to eMMC.
    NvBootSdmmcDataWidth_4Bit = 1,

    /// Specifies a 8 bit interface to eMMC.
    NvBootSdmmcDataWidth_8Bit = 2,

    NvBootSdmmcDataWidth_Num,
    NvBootSdmmcDataWidth_Force32 = 0x7FFFFFFF
} NvBootSdmmcDataWidth;

/// Defines the parameters that can be changed after BCT is read.
typedef struct NvBootSdmmcParamsRec
{
    /**
     * Specifies the clock divider for the SDMMC controller's clock source,
     * which is PLLP running at 432MHz.  If it is set to 18, then the SDMMC
     * controller runs at 432/18 = 24MHz.
     */
    NvU8 ClockDivider;

    /// Specifies the data bus width. Supported data widths are 4- and 8-bits.
    NvBootSdmmcDataWidth DataWidth;

    /** 
     * Max Power class supported by the target board.
     * The driver determines the best data width and clock frequency
     * supported within the power class range (0 to Max) if the selected
     * data width cannot be used at the chosen clock frequency.
     */
    NvU8 MaxPowerClassSupported;
} NvBootSdmmcParams;

#if defined(__cplusplus)
}
#endif

#endif /* #ifndef INCLUDED_NVBOOT_SDMMC_PARAM_H */

