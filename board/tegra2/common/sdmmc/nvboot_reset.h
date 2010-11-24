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
 * nvboot_reset.h - Declarations for reset support.
 * 
 */

#ifndef INCLUDED_NVBOOT_RESET_H
#define INCLUDED_NVBOOT_RESET_H

#if defined(__cplusplus)
extern "C"
{
#endif

// Set of reset signals supported in the API */
// The corresponding enum embeds some information on the register structure
#define NVBOOT_RESET_DEVICE_BIT_OFFSET_MASK (0x1F)
#define NVBOOT_RESET_H_REG (0x20)
#define NVBOOT_RESET_U_REG (0x40)
#define NVBOOT_RESET_BIT_OFFSET(DeviceId)         (((NvU32) DeviceId) & NVBOOT_RESET_DEVICE_BIT_OFFSET_MASK)
#define NVBOOT_RESET_REG_OFFSET(DeviceId)        ((((NvU32) DeviceId) & (NVBOOT_RESET_H_REG | NVBOOT_RESET_U_REG)) >> 3 )
typedef enum 
{                                                                                      
    NvBootResetDeviceId_CpuId    = CLK_RST_CONTROLLER_RST_DEVICES_L_0_SWR_CPU_RST_SHIFT,
    NvBootResetDeviceId_CopId    = CLK_RST_CONTROLLER_RST_DEVICES_L_0_SWR_COP_RST_SHIFT,
    NvBootResetDeviceId_I2cpId   = CLK_RST_CONTROLLER_RST_DEVICES_H_0_SWR_DVC_I2C_RST_SHIFT	+ NVBOOT_RESET_H_REG,
    NvBootResetDeviceId_I2c1Id   = CLK_RST_CONTROLLER_RST_DEVICES_L_0_SWR_I2C1_RST_SHIFT,
    NvBootResetDeviceId_I2c2Id   = CLK_RST_CONTROLLER_RST_DEVICES_H_0_SWR_I2C2_RST_SHIFT	+ NVBOOT_RESET_H_REG,
    NvBootResetDeviceId_I2c3Id   = CLK_RST_CONTROLLER_RST_DEVICES_U_0_SWR_I2C3_RST_SHIFT	+ NVBOOT_RESET_U_REG,
    NvBootResetDeviceId_NandId   = CLK_RST_CONTROLLER_RST_DEVICES_L_0_SWR_NDFLASH_RST_SHIFT,
    NvBootResetDeviceId_Sdmmc1Id = CLK_RST_CONTROLLER_RST_DEVICES_L_0_SWR_SDMMC1_RST_SHIFT,
    NvBootResetDeviceId_Sdmmc2Id = CLK_RST_CONTROLLER_RST_DEVICES_L_0_SWR_SDMMC2_RST_SHIFT,
    NvBootResetDeviceId_Sdmmc3Id = CLK_RST_CONTROLLER_RST_DEVICES_U_0_SWR_SDMMC3_RST_SHIFT	+ NVBOOT_RESET_U_REG,
    NvBootResetDeviceId_Sdmmc4Id = CLK_RST_CONTROLLER_RST_DEVICES_L_0_SWR_SDMMC4_RST_SHIFT,
    NvBootResetDeviceId_UartaId  = CLK_RST_CONTROLLER_RST_DEVICES_L_0_SWR_UARTA_RST_SHIFT,
    NvBootResetDeviceId_UsbId    = CLK_RST_CONTROLLER_RST_DEVICES_L_0_SWR_USBD_RST_SHIFT,
    NvBootResetDeviceId_McId     = CLK_RST_CONTROLLER_RST_DEVICES_H_0_SWR_MEM_RST_SHIFT    + NVBOOT_RESET_H_REG,
    NvBootResetDeviceId_EmcId    = CLK_RST_CONTROLLER_RST_DEVICES_H_0_SWR_EMC_RST_SHIFT    + NVBOOT_RESET_H_REG,
    NvBootResetDeviceId_BseaId   = CLK_RST_CONTROLLER_RST_DEVICES_H_0_SWR_BSEA_RST_SHIFT   + NVBOOT_RESET_H_REG,
    NvBootResetDeviceId_BsevId   = CLK_RST_CONTROLLER_RST_DEVICES_H_0_SWR_BSEV_RST_SHIFT   + NVBOOT_RESET_H_REG,
    NvBootResetDeviceId_SpiId    = CLK_RST_CONTROLLER_RST_DEVICES_H_0_SWR_SPI1_RST_SHIFT   + NVBOOT_RESET_H_REG,
    NvBootResetDeviceId_VdeId    = CLK_RST_CONTROLLER_RST_DEVICES_H_0_SWR_VDE_RST_SHIFT    + NVBOOT_RESET_H_REG,
    NvBootResetDeviceId_ApbDmaId = CLK_RST_CONTROLLER_RST_DEVICES_H_0_SWR_APBDMA_RST_SHIFT + NVBOOT_RESET_H_REG,
    NvBootResetDeviceId_AhbDmaId = CLK_RST_CONTROLLER_RST_DEVICES_H_0_SWR_AHBDMA_RST_SHIFT + NVBOOT_RESET_H_REG,
    NvBootResetDeviceId_SnorId   = CLK_RST_CONTROLLER_RST_DEVICES_H_0_SWR_NOR_RST_SHIFT    + NVBOOT_RESET_H_REG,
    NvBootResetDeviceId_Usb3Id   = CLK_RST_CONTROLLER_RST_DEVICES_H_0_SWR_USB3_RST_SHIFT    + NVBOOT_RESET_H_REG,
   NvBootResetSignalId_Force32 = 0x7fffffff
} NvBootResetDeviceId;

#define NVBOOT_RESET_CHECK_ID(DeviceId) \
    NV_ASSERT( (DeviceId == NvBootResetDeviceId_CpuId)    || \
               (DeviceId == NvBootResetDeviceId_CopId)    || \
               (DeviceId == NvBootResetDeviceId_I2c1Id)   || \
               (DeviceId == NvBootResetDeviceId_NandId)   || \
               (DeviceId == NvBootResetDeviceId_Sdmmc1Id) || \
               (DeviceId == NvBootResetDeviceId_Sdmmc2Id) || \
               (DeviceId == NvBootResetDeviceId_Sdmmc3Id) || \
               (DeviceId == NvBootResetDeviceId_Sdmmc4Id) || \
               (DeviceId == NvBootResetDeviceId_UartaId)  || \
               (DeviceId == NvBootResetDeviceId_UsbId)    || \
               (DeviceId == NvBootResetDeviceId_Usb3Id)   || \
               (DeviceId == NvBootResetDeviceId_McId)     || \
               (DeviceId == NvBootResetDeviceId_EmcId)    || \
               (DeviceId == NvBootResetDeviceId_BseaId)   || \
               (DeviceId == NvBootResetDeviceId_BsevId)   || \
               (DeviceId == NvBootResetDeviceId_SpiId)    || \
               (DeviceId == NvBootResetDeviceId_VdeId)    || \
               (DeviceId == NvBootResetDeviceId_ApbDmaId) || \
               (DeviceId == NvBootResetDeviceId_AhbDmaId) || \
               (DeviceId == NvBootResetDeviceId_SnorId)      );


#define NVBOOT_RESET_STABILIZATION_DELAY (0x2)

/* 
 * NvBootResetSetEnable(): Control the reset level for the identified reset signal
 */
void
NvBootResetSetEnable(const NvBootResetDeviceId DeviceId, const NvBool Enable);

/* 
 * NvBootResetFullChip(): This is the highest possible reset, everything but AO is reset
 */
void
NvBootResetFullChip(void);

#if defined(__cplusplus)
}
#endif

#endif /* #ifndef INCLUDED_NVBOOT_RESET_H */
