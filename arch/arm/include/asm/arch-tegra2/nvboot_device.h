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

#if defined(__cplusplus)
}
#endif

#endif /* #ifndef INCLUDED_NVBOOT_DEVICE_H */
