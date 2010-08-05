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
 * nvboot_driver.h - Definition of the driver interface for the second level
 *      boot devices.
 */

#ifndef INCLUDED_NVBOOT_DEVICE_H
#define INCLUDED_NVBOOT_DEVICE_H

#include "nvcommon.h"

#if defined(__cplusplus)
extern "C"
{
#endif

/*
 * Enumerated Constants
 */

/*
 * NvBootDeviceStatus: The current status of a read request.
 */
typedef enum
{
    NvBootDeviceStatus_None = 0,
    NvBootDeviceStatus_Idle,
    NvBootDeviceStatus_ReadInProgress,
    NvBootDeviceStatus_ReadFailure,
    NvBootDeviceStatus_EccFailure,
    NvBootDeviceStatus_CorrectedEccFailure,
    NvBootDeviceStatus_CrcFailure,
    NvBootDeviceStatus_DataTimeout,
    NvBootDeviceStatus_CorrectedReadFailure, /* Error occured but corrected */
    NvBootDeviceStatus_Max, /* Must appear after the last legal item */
    NvBootDeviceStatus_Force32 = 0x7fffffff
} NvBootDeviceStatus;

#if 0
/*
 * Entry points specified by each driver.
 */

/**
 * NvBootDeviceGetParams(): Returns a pointer to a device-specific 
 * structure of device parameters in the ROM.  Later, the init routine will
 * use them to configure the device.
 *
 * Note that the implementation of this function will declare its
 * second argument as a pointer to a specific structure type, not as a
 * void **.
 *
 * @param ParamIndex Device configuration index read from fuses.
 * @param Defaults Pointer into which the device-specific parameters will
 * be written.
 */
typedef void (*NvBootDeviceGetParams)(const NvU32 ParamIndex, void **Defaults);

/**
 * NvBootDeviceValidateParams(): Checks the contents of the parameter structure
 * and returns NV_TRUE if the parameters are all legal, NV_FLASE otherwise.
 *
 * Note that the implementation of this function will declare its
 * argument as a pointer to a specific structure type, not as a
 * void *.
 *
 * @param Params Pointer to a device-specific parameter structure.
 *
 * @retval NV_TRUE  The parameters are valid.
 * @retval NV_FALSE - The parameters would cause an error if used.
 */
typedef NvBool (*NvBootDeviceValidateParams)(const void *Params);

/**
 * NvBootDeviceGetBlockSizes(): Queries the block and page sizes for the
 * device in units of log2(bytes).  Thus, a 1KB block size is reported as
 * 10.
 * 
 * @param Params Pointer to the device-specific parameter structure
 * @param BlockSizeLog2 Size of the block in log2 number of bytes
 * @param PageSizeLog2 Size of the page in log2 number of bytes
 *
 * Note: The GetBlockSizes() call cannot occur prior to the Init() for the
 * device, as some devices query parameters from the device during Init().
 */
typedef void
(*NvBootDeviceGetBlockSizes)(
    const void *Params,
    NvU32 *BlockSizeLog2,
    NvU32 *PageSizeLog2);

/**
 * NvBootDeviceInit(): Use the data pointed to by DeviceParams to initialize
 * the device for reading.  Note that the routine will likely be called
 * multiple times - once for initially gaining access to the device,
 * and once to use better parameters stored in the device itself.
 *
 * DriverContext is provided as space for storing device-specific global state.
 * Drivers should keep this pointer around for reference during later calls.
 *
 * Note that the implementation of this function will declare its
 * arguments as pointers to specific structure types, not as void *.
 *
 * @param ParamData Pointer to a device-specific parameter structure.
 * @param DriverContext Pointer to storage for a device-specific context
 * structure
 *
 * @retval NvBootError_Success No Error
 * @retval NvBootError_InvalidParameter Illegal parameter value
 * @retval NvBootError_DeviceNotResponding
 * @retval NvBootError_DataCorrupted
 */
typedef NvError
(*NvBootDeviceInit)(const void *ParamData, void *DriverContext);

/**
 * NvBootDevReadPage(): Initiate the reading of a page of data into Dest.
 *
 * @param Block Number of the block from which to read
 * @param Page Number of the page within the block from which to read
 * @param Dest Storage for the data read from the device.
 *
 * @retval NvBootError_Success No Error
 * @retval NvBootError_InvalidParameter Illegal parameter value
 * @retval NvBootError_DeviceNotResponding
 * @retval NvBootError_DataCorrupted
 * @retval NvBootError_DataUnderflow
 * @retval NvBootError_DeviceReadError
 * @retval NvBootError_EccFailureCorrected
 * @retval NvBootError_EccFailureUncorrected
 */
typedef NvError
(*NvBootDeviceReadPage)(const NvU32 Block, const NvU32 Page, NvU8 *Dest);

/**
 * NvBootDeviceQueryStatus(): Check the status of pending operations.
 *
 * @retval NvBootDeviceStatus_Idle
 * @retval NvBootDeviceStatus_ReadInProgress
 * @retval NvBootDeviceStatus_ReadFailure
 * @retval NvBootDeviceStatus_EccFailure
 * @retval NvBootDeviceStatus_CorrectedReadFailure Error occured but corrected
 */
typedef NvBootDeviceStatus (*NvBootDeviceQueryStatus)(void);

/**
 * NvBootDeviceShutdown(): Shutdown device and cleanup state.
 */
typedef void (*NvBootDeviceShutdown)(void);

#endif

#if defined(__cplusplus)
}
#endif

#endif /* #ifndef INCLUDED_NVBOOT_DEVICE_H */
