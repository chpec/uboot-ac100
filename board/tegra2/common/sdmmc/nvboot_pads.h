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
 * @file nvboot_pads.h
 *
 * Pads and other low level power function for NvBoot
 *
 * NvBootPads is NVIDIA's interface for control of IO pads and power related 
 * functions, including pin mux control.
 *
 */

#ifndef INCLUDED_NVBOOT_PADS_H
#define INCLUDED_NVBOOT_PADS_H

#include <asm/arch/nvcommon.h>
#include <asm/arch/nvboot_error.h>
#include "nvboot_fuse.h"

#define NVBOOT_PADS_PWR_DET_DELAY (6)

#if defined(__cplusplus)
extern "C"
{
#endif

#define NVBOOT_PADS_UP_DRIVER_STRENGTH_1_8V (22)
#define NVBOOT_PADS_DN_DRIVER_STRENGTH_1_8V (18)

// Symbolic constants for Config arguments to
// NvBootPadsConfigureForBootPeripheral() 
typedef enum
{
    NvBootPinmuxConfig_None = 0,

    NvBootPinmuxConfig_MobileLbaNand   = 1, // MobileLBA NAND

    NvBootPinmuxConfig_MuxOneNand      = 5, // MuxOneNAND (SNOR config 5)
    NvBootPinmuxConfig_OneNand         = 6, // OneNAND    (SNOR config 6)

    NvBootPinmuxConfig_Nand_Std_x8     = 2, // NAND x8,     standard  pins
    NvBootPinmuxConfig_Nand_Std_x16    = 1, // NAND x16,    standard  pins
    NvBootPinmuxConfig_Nand_Alt        = 3, // NAND x8,x16, alternate pins

    NvBootPinmuxConfig_Sdmmc_Std_x4    = 3, // eMMC/eSD x4,    standard  pins
    NvBootPinmuxConfig_Sdmmc_Std_x8    = 2, // eMMC/eSD x8,    standard  pins
    NvBootPinmuxConfig_Sdmmc_Alt       = 1, // eMMC/eSD x4,x8, alternate pins

    NvBootPinmuxConfig_Snor_Mux_x16    = 3, // SNOR Muxed x16
    NvBootPinmuxConfig_Snor_NonMux_x16 = 4, // SNOR NonMuxed x16
    NvBootPinmuxConfig_Snor_Mux_x32    = 2, // SNOR Muxed x32
    NvBootPinmuxConfig_Snor_NonMux_x32 = 1, // SNOR NonMuxed x32

    NvBootPinmuxConfig_Spi             = 1, // SPI Flash
    
    NvBootPinmuxConfig_Force32 = 0x7fffffff
} NvBootPinmuxConfig;

/*
 * Set up of correct path between a controller and external world
 * Also set the correct IO driver strength for interfaces that may operate 
 * at 1.8 V (NAND and HSMMC)
 *
 * @param identification of the boot device
 * @param configuration of the boot device
 *
 * @retval NvBootError_ValidationFailure of passing an incorrect device
 * @retval NvBootError_Success otherwise
 */ 
NvBootError
NvBootPadsConfigForBootDevice(
    NvBootFuseBootDevice BootDevice,
    NvU32 Config);


/*
 * Bring the pads out of Deep Power Down operation.
 *
 * @retval none
 */ 
void
NvBootPadsExitDeepPowerDown(void);

#if defined(__cplusplus)
}
#endif

#endif // INCLUDED_NVBOOT_PADS_H
