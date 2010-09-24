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

#include <common.h>
#include <asm/io.h>
#include <asm/arch/nv_drf.h>
#include <asm/arch/nvcommon.h>
#include <asm/arch/tegra2.h>
#include <asm/arch/nv_hardware_access.h>

#include "nvboot_pads.h"
#include "nvboot_pinmux_local.h"
#include "nvboot_util.h"

extern const NvU32* g_Ap20MuxControllers[];
//#define PAD_DEBUG
#undef PAD_DEBUG

#ifdef PAD_DEBUG
#define SET_DRIVER_STRENGTH(set, voltage)                                  \
    RegData = NV_READ32(NV_ADDRESS_MAP_APB_MISC_BASE +                     \
                        APB_MISC_GP_##set##PADCTRL_0);                     \
    printf("%s: R reg: 0x%08x, val: 0x%08x\n", __FUNCTION__,               \
    NV_ADDRESS_MAP_APB_MISC_BASE + APB_MISC_GP_##set##PADCTRL_0, RegData); \
    RegData = NV_FLD_SET_DRF_NUM(APB_MISC_GP,                              \
                                 set##PADCTRL,                             \
                                 CFG2TMC_##set##_CAL_DRVDN,                \
                                 *pDrvStrengthDn_##voltage,             \
                                 RegData);                                 \
    RegData = NV_FLD_SET_DRF_NUM(APB_MISC_GP,                              \
                                 set##PADCTRL,                             \
                                 CFG2TMC_##set##_CAL_DRVUP,                \
                                 *pDrvStrengthUp_##voltage,               \
                                 RegData);                                 \
    NV_WRITE32(NV_ADDRESS_MAP_APB_MISC_BASE +                              \
               APB_MISC_GP_##set##PADCTRL_0, RegData);			   \
    printf("%s: W reg: 0x%08x, val: 0x%08x\n", __FUNCTION__,               \
      NV_ADDRESS_MAP_APB_MISC_BASE + APB_MISC_GP_##set##PADCTRL_0, RegData);

#else // endof PAD_DEBUG

#define SET_DRIVER_STRENGTH(set, voltage)                                  \
    RegData = NV_READ32(NV_ADDRESS_MAP_APB_MISC_BASE +                     \
                        APB_MISC_GP_##set##PADCTRL_0);                     \
    RegData = NV_FLD_SET_DRF_NUM(APB_MISC_GP,                              \
                                 set##PADCTRL,                             \
                                 CFG2TMC_##set##_CAL_DRVDN,                \
                                 *pDrvStrengthDn_##voltage,             \
                                 RegData);                                 \
    RegData = NV_FLD_SET_DRF_NUM(APB_MISC_GP,                              \
                                 set##PADCTRL,                             \
                                 CFG2TMC_##set##_CAL_DRVUP,                \
                                 *pDrvStrengthUp_##voltage,               \
                                 RegData);                                 \
    NV_WRITE32(NV_ADDRESS_MAP_APB_MISC_BASE +                              \
               APB_MISC_GP_##set##PADCTRL_0, RegData);
#endif

/*
 * Note: To ease ECOs, the driver strength values are written to local
 *       variables which are referenced from volatile constants.  This
 *       forces the compiler to generate code with the constants used in
 *       exactly one place.
 */
NvBootError
NvBootPadsConfigForBootDevice(NvBootFuseBootDevice BootDevice, NvU32 Config)
{
    NvU32 RegData;
    const NvU32 *BootData;
    NvU32 DriverStrengthDown_1_8V;
    NvU32 DriverStrengthUp_1_8V;
    volatile NvU32* pDrvStrengthDn_1_8V = &DriverStrengthDown_1_8V;
    volatile NvU32* pDrvStrengthUp_1_8V = &DriverStrengthUp_1_8V;

    NV_ASSERT(BootDevice < NvBootFuseBootDevice_Max);

    // Set the pinmmuxes and tristates for the device.
    BootData = g_Ap20MuxControllers[BootDevice];
    NvBootPadSetPinMuxCtl(BootData, Config);
    NvBootPadSetTriStates(BootData, Config, NV_FALSE);

    *pDrvStrengthDn_1_8V = NVBOOT_PADS_DN_DRIVER_STRENGTH_1_8V;
    *pDrvStrengthUp_1_8V = NVBOOT_PADS_UP_DRIVER_STRENGTH_1_8V;

    // Set the driver strength controls as needed
    switch (BootDevice)
    {
        case NvBootFuseBootDevice_MuxOneNand:
            /*
             * Put driver strength to 1.8V (stronger) as MuxOneNAND
             * could be 1.8V
             * see ap20/package/AP20_PinOutSpec.xls
             */
            // Common for all configs
            SET_DRIVER_STRENGTH(ATCFG1,   1_8V);
            SET_DRIVER_STRENGTH(ATCFG2,   1_8V);
            SET_DRIVER_STRENGTH(DAP2CFG,  1_8V);
            SET_DRIVER_STRENGTH(DAP4CFG,  1_8V);
            SET_DRIVER_STRENGTH(GMBCFG,   1_8V);
            SET_DRIVER_STRENGTH(GMCCFG,   1_8V);
            SET_DRIVER_STRENGTH(GMDCFG,   1_8V);
            SET_DRIVER_STRENGTH(SPICFG,   1_8V);

            // Additions for non-muxed address lines
            if (Config == NvBootPinmuxConfig_OneNand);
            {
                SET_DRIVER_STRENGTH(DBGCFG,   1_8V);
                SET_DRIVER_STRENGTH(UART2CFG, 1_8V);
                SET_DRIVER_STRENGTH(UART3CFG, 1_8V);
            }
            break;

        case NvBootFuseBootDevice_MobileLbaNand:
        case NvBootFuseBootDevice_NandFlash:
            /*
             * Put driver strength to 1.8V (stronger) as NAND could be 1.8V
             * see ap20/package/AP20_PinOutSpec.xls
             */
            if (Config == NvBootPinmuxConfig_Nand_Std_x8  ||
                Config == NvBootPinmuxConfig_Nand_Std_x16 ||
                Config == NvBootPinmuxConfig_MobileLbaNand)
            {
                SET_DRIVER_STRENGTH(ATCFG1, 1_8V);
                SET_DRIVER_STRENGTH(ATCFG2, 1_8V);
            }
            else if (Config == NvBootPinmuxConfig_Nand_Alt)
            {
                SET_DRIVER_STRENGTH(AOCFG1, 1_8V);
                SET_DRIVER_STRENGTH(AOCFG2, 1_8V);
            }
            break;

        case NvBootFuseBootDevice_Sdmmc:
            /*
             * Put driver strength to 1.8V (stronger) as eMMC/eSD could be 1.8V
             * see ap20/package/AP20_PinOutSpec.xls
             */

            // Common for all configs
            SET_DRIVER_STRENGTH(ATCFG1, 1_8V);
            SET_DRIVER_STRENGTH(ATCFG2, 1_8V);

            if (Config == NvBootPinmuxConfig_Sdmmc_Std_x4 ||
                Config == NvBootPinmuxConfig_Sdmmc_Std_x8)
            {
                SET_DRIVER_STRENGTH(GMACFG, 1_8V);
            }
            break;

        case NvBootFuseBootDevice_SnorFlash:
            /*
             * Put driver strength to 1.8V (stronger) as SNOR could be 1.8V
             * see ap20/package/AP20_PinOutSpec.xls
             */

            // Common for all configs
            SET_DRIVER_STRENGTH(ATCFG1,   1_8V);
            SET_DRIVER_STRENGTH(ATCFG2,   1_8V);
            SET_DRIVER_STRENGTH(DAP2CFG,  1_8V);
            SET_DRIVER_STRENGTH(DAP4CFG,  1_8V);
            SET_DRIVER_STRENGTH(GMBCFG,   1_8V);
            SET_DRIVER_STRENGTH(GMDCFG,   1_8V);
            SET_DRIVER_STRENGTH(SPICFG,   1_8V);

            // Additions for non-muxed address lines
            if (Config == NvBootPinmuxConfig_Snor_NonMux_x16 ||
                Config == NvBootPinmuxConfig_Snor_NonMux_x32)
            {
                SET_DRIVER_STRENGTH(DBGCFG,   1_8V);
                SET_DRIVER_STRENGTH(UART2CFG, 1_8V);
                SET_DRIVER_STRENGTH(UART3CFG, 1_8V);
            }

            // Additions for 32-bit data bus
            if (Config == NvBootPinmuxConfig_Snor_Mux_x32 ||
                Config == NvBootPinmuxConfig_Snor_NonMux_x32)
            {
                SET_DRIVER_STRENGTH(DAP1CFG,  1_8V);
                SET_DRIVER_STRENGTH(GMACFG,   1_8V);
                SET_DRIVER_STRENGTH(GMCCFG,   1_8V);
                SET_DRIVER_STRENGTH(GMECFG,   1_8V);
            }

            break;

        case NvBootFuseBootDevice_SpiFlash:
            /*
             * Put driver strength to 1.8V (stronger) as SPI could be 1.8V
             * see ap20/package/AP20_PinOutSpec.xls
             */
            SET_DRIVER_STRENGTH(GMCCFG, 1_8V);
            SET_DRIVER_STRENGTH(GMDCFG, 1_8V);

        default:
            break;
    }

    return NvBootError_Success;
}

void
NvBootPadsExitDeepPowerDown()
{
    NvU32 regVal;

    // Clear the DPD Sample and DPD Enable fields in the PMC HW

    // Note: The order of these operations must be preserved!

    // Disable the DPD_SAMPLE field of APBDEV_PMC_DPD_SAMPLE_0
    NV_READ32_(NV_ADDRESS_MAP_PMC_BASE + APBDEV_PMC_DPD_SAMPLE_0, regVal);
    regVal = NV_FLD_SET_DRF_NUM(APBDEV_PMC,
                                DPD_SAMPLE,
                                ON,
                                APBDEV_PMC_DPD_SAMPLE_0_ON_DISABLE,
                                regVal);
    NV_WRITE32(NV_ADDRESS_MAP_PMC_BASE + APBDEV_PMC_DPD_SAMPLE_0, regVal);

    // Disable the DPD_ENABLE field of APBDEV_PMC_DPD_ENABLE_0
    NV_READ32_(NV_ADDRESS_MAP_PMC_BASE + APBDEV_PMC_DPD_ENABLE_0, regVal);
    regVal = NV_FLD_SET_DRF_NUM(APBDEV_PMC,
                                DPD_ENABLE,
                                ON,
                                APBDEV_PMC_DPD_ENABLE_0_ON_DISABLE,
                                regVal);
    NV_WRITE32(NV_ADDRESS_MAP_PMC_BASE + APBDEV_PMC_DPD_ENABLE_0, regVal);

}
