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
 * nvboot_sdmmc.h - Public definitions for using Sdmmc as the second
 * level boot device.
 */

#ifndef INCLUDED_NV_SDMMC_H
#define INCLUDED_NV_SDMMC_H

#include "nvcommon.h"
#include "nvboot_error.h"
#include "nvboot_device.h"
#include "nvboot_sdmmc_context.h"
#include "nvboot_sdmmc_param.h"

#if defined(__cplusplus)
extern "C"
{
#endif

//void FFAConfiguration(void);

/**
 * Returns a pointer to a device-specific structure of device parameters 
 * in the ROM.  Later, the init routine will use them to configure the device.
 *
 * @param ParamIndex Parma Index that comes from Fuse values.
 * @param Params double pointer to retunr Param info based on 
 *          the param index value.
 * 
 */
void NvBootSdmmcGetParams(const NvU32 ParamIndex, NvBootSdmmcParams **Params);

/**
 * Checks the contents of the parameter structure and returns NV_TRUE 
 * if the parameters are all legal, NV_FLASE otherwise.
 *
 *@param Params Pointer to Param info that needs validation.
 *
 * @retval NV_TRUE The parameters are valid.
 * @retval NV_FALSE The parameters would cause an error if used.
 */
NvBool NvBootSdmmcValidateParams(const NvBootSdmmcParams *Params);

/**
 * Queries the block and page sizes for the device in units of log2(bytes).
 * Thus, a 1KB block size is reported as 10.
 * 
 * @note NvBootSdmmcInit() must have beeen called at least once before 
 * calling this API and the call must have returned NvBootError_Success.
 * 
 * @param Params Pointer to Param info.
 * @param BlockSizeLog2 returns block size in log2 scale.
 * @param PageSizeLog2 returns page size in log2 scale.
 * 
 * @retval NvBootError_Success No Error
 */
void 
NvBootSdmmcGetBlockSizes(
    const NvBootSdmmcParams *Params,
    NvU32 *BlockSizeLog2,
    NvU32 *PageSizeLog2);

/**
 * Uses the data pointed to by DeviceParams to initialize
 * the device for reading.  Note that the routine will likely be called
 * multiple times - once for initially gaining access to the device,
 * and once to use better parameters stored in the device itself.
 *
 * DriverContext is provided as space for storing device-specific global state.
 * Drivers should keep this pointer around for reference during later calls.
 *
 * @param ParamData Pointer to Param info to initialize the Nand with.
 * @param Context Pointer to memory, where nand state is saved to.
 *
 * @retval NvBootError_Success Initialization is successful.
 * @retval NvBootError_HwTimeOut Device is not responding.
 * @retval NvBootError_DeviceResponseError Response Recevied from device 
 *          indicated that operation had failed.
 * @retval NvBootError_DeviceError Error in communicating with device. No need
 *          to retry. It is taken care internally.
 */
NvBootError 
NvBootSdmmcInit(
    const NvBootSdmmcParams *ParamData,
    NvBootSdmmcContext *Context);

/**
 * Initiate the reading of a page of data into Dest.buffer.
 * 
 * @param Block Block number to read from.
 * @param Page Page number in the block to read from. 
 *          valid range is 0 <= Page < PagesPerBlock.
 * @param Dest Buffer to rad the data into.
 *
 * @retval NvBootError_Success Read operation is launched successfully.
 * @retval NvBootError_HwTimeOut Device is not responding.
 * @retval NvBootError_DeviceResponseError Response Recevied from device 
 *          indicated that operation had failed.
 * @retval NvBootError_DeviceError Error in communicating with device. No need
 *          to retry. It is taken care internally.
 */
NvBootError 
SdmmcReadPage(const NvU32 Block, const NvU32 Page, NvU8 *pBuffer);

NvBootError 
SdmmcWritePage(const NvU32 Block, const NvU32 Page, NvU8 *pBuffer);

/**
 * Check the status of read operation that is launched with 
 *  API NvBootSdmmcReadPage, if it is pending.
 *
 * @retval NvBootDeviceStatus_ReadInProgress - Reading is in progress.
 * @retval NvBootDeviceStatus_CrcFailure - Data received is corrupted. Client 
 *          should try to read again and should be upto 3 times.
 * @retval NvBootDeviceStatus_DataTimeout Data is not received from device.
 * @retval NvBootDeviceStatus_ReadFailure - Read operation failed.
 * @retval NvBootDeviceStatus_Idle - Read operation is complete and successful.
 */
NvBootDeviceStatus NvBootSdmmcQueryStatus(void);

/**
 * Shutdowns device and cleanup the state.
 * 
 */
void NvBootSdmmcShutdown(void);

#if defined(__cplusplus)
}
#endif

#endif /* #ifndef INCLUDED_NV_SDMMC_H */

