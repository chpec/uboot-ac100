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
 * nvboot_error.h - error codes.
 */

#ifndef INCLUDED_NVBOOT_ERROR_H
#define INCLUDED_NVBOOT_ERROR_H

#if defined(__cplusplus)
extern "C"
{
#endif

/*
 * NvBootError: Enumerated error codes
 */
typedef enum
{
    NvBootError_None = 0,
    NvBootError_Success = 0,
    NvBootError_InvalidParameter,
    NvBootError_IllegalParameter,
    NvBootError_HwTimeOut,
    NvBootError_NotInitialized,
    NvBootError_DeviceNotResponding,
    NvBootError_DataCorrupted,
    NvBootError_DataUnderflow,
    NvBootError_DeviceError,
    NvBootError_DeviceReadError,
    NvBootError_DeviceUnsupported,
    NvBootError_DeviceResponseError,
    NvBootError_Unimplemented,
    NvBootError_ValidationFailure,
    NvBootError_EccDiscoveryFailed,
    NvBootError_EccFailureCorrected,
    NvBootError_EccFailureUncorrected,
    NvBootError_Busy,
    NvBootError_Idle,
    NvBootError_MemoryNotAllocated,
    NvBootError_MemoryNotAligned,
    NvBootError_BctNotFound,
    NvBootError_BootLoaderLoadFailure,
    NvBootError_BctBlockInfoMismatch,
    NvBootError_IdentificationFailed,
    NvBootError_HashMismatch,
    NvBootError_TxferFailed,
    NvBootError_WriteFailed,
    NvBootError_WarmBoot0_Failure,
    NvBootError_AccessDenied,
    NvBootError_InvalidOscFrequency,
    NvBootError_InvalidDevParams,
    NvBootError_InvalidBootDeviceEncoding,
    NvBootError_CableNotConnected,
    NvBootError_InvalidBlDst,

    NvBootError_Force32 = 0x7fffffff
} NvBootError;

#if defined(__cplusplus)
}
#endif

#endif /* #ifndef INCLUDED_NVBOOT_ERROR_H */

