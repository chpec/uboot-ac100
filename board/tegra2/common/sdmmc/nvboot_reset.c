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
 * nvboot_reset.c - Implementation of Clocks support.
 */

#include <common.h>
#include <asm/io.h>
#include <asm/arch/nv_drf.h>
#include <asm/arch/nvcommon.h>
#include <asm/arch/tegra2.h>
#include <asm/arch/nv_hardware_access.h>

#include "nvboot_reset.h"
#include "nvboot_util.h"

void
NvBootResetSetEnable(const NvBootResetDeviceId DeviceId, const NvBool Enable) {

    NvU32 RegData ;
    NvU32 BitOffset ;
    NvU32 RegOffset ;

    NVBOOT_RESET_CHECK_ID(DeviceId) ;
    NV_ASSERT( ((int) Enable == 0) || 
              ((int) Enable == 1)   ) ;

    BitOffset = NVBOOT_RESET_BIT_OFFSET(DeviceId) ;
    RegOffset = NVBOOT_RESET_REG_OFFSET(DeviceId) ;
//    RegData = NV_READ32( NV_ADDRESS_MAP_CAR_BASE + CLK_RST_CONTROLLER_RST_DEVICES_L_0 + RegOffset) ;
    NV_READ32_(NV_ADDRESS_MAP_CAR_BASE + CLK_RST_CONTROLLER_RST_DEVICES_L_0 + RegOffset, RegData);
    /* no simple way to use the access macro in this case */
    if (Enable) {
        RegData |=  (1 << BitOffset) ;
    } else {
        RegData &= ~(1 << BitOffset) ;
    }
    NV_WRITE32(NV_ADDRESS_MAP_CAR_BASE + CLK_RST_CONTROLLER_RST_DEVICES_L_0 + RegOffset, RegData) ;

    // wait stabilization time (always)
    NvBootUtilWaitUS(NVBOOT_RESET_STABILIZATION_DELAY) ;
    
    return ;
}

void
NvBootResetFullChip(void) {
    NvU32 regData ;
    regData = NV_READ32(NV_ADDRESS_MAP_PMC_BASE + APBDEV_PMC_CNTRL_0) ;
    regData = NV_FLD_SET_DRF_DEF(APBDEV_PMC, CNTRL, MAIN_RST, ENABLE, regData) ;
    NV_WRITE32(NV_ADDRESS_MAP_PMC_BASE + APBDEV_PMC_CNTRL_0, regData) ;

    // wait is nto really needed, insure nothing dangerous is started
    NvBootUtilWaitUS(NVBOOT_RESET_STABILIZATION_DELAY) ;
}
