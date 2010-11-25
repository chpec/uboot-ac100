/*
 * (C) Copyright 2010
 * NVIDIA Corporation <www.nvidia.com>
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
 * Defines the SDRAM parameter structure.
 *
 * Note that PLLM is used by EMC.
 */

#ifndef INCLUDED_NVBOOT_SDRAM_PARAM_H
#define INCLUDED_NVBOOT_SDRAM_PARAM_H

#include "nvboot_config.h"

#if defined(__cplusplus)
extern "C"
{
#endif

typedef enum
{
    /// Specifies the memory type to be undefined
    NvBootMemoryType_None = 0,

    /// Specifies the memory type to be DDR SDRAM
    NvBootMemoryType_Ddr,

    /// Specifies the memory type to be LPDDR SDRAM
    NvBootMemoryType_LpDdr,

    /// Specifies the memory type to be DDR2 SDRAM
    NvBootMemoryType_Ddr2,

    /// Specifies the memory type to be LPDDR2 SDRAM
    NvBootMemoryType_LpDdr2,

    NvBootMemoryType_Num,
    NvBootMemoryType_Force32 = 0x7FFFFFF
} NvBootMemoryType;


/**
 * Defines the SDRAM parameter structure
 */
typedef struct NvBootSdramParamsRec
{
    /// Specifies the type of memory device
    NvBootMemoryType MemoryType;

    /// Specifies the CPCON value for PllM
    NvU32 PllMChargePumpSetupControl;
    /// Specifies the LPCON value for PllM
    NvU32 PllMLoopFilterSetupControl;
    /// Specifies the M value for PllM
    NvU32 PllMInputDivider;
    /// Specifies the N value for PllM
    NvU32 PllMFeedbackDivider;
    /// Specifies the P value for PllM
    NvU32 PllMPostDivider;
    /// Specifies the time to wait for PLLM to lock (in microseconds)
    NvU32 PllMStableTime;

    /// Specifies the divider for the EMC Clock Source
    NvU32 EmcClockDivider;

    ///
    /// Auto-calibration of EMC pads
    ///
    /// Specifies the value for EMC_AUTO_CAL_INTERVAL
    NvU32 EmcAutoCalInterval;
    /// Specifies the value for EMC_AUTO_CAL_CONFIG
    /// Note: Trigger bits are set by the SDRAM code.
    NvU32 EmcAutoCalConfig;
    /// Specifies the time for the calibration to stabilize (in microseconds)
    NvU32 EmcAutoCalWait;

    /// Specifies the time to wait after pin programming (in microseconds)
    /// Dram vendors require at least 200us.
    NvU32 EmcPinProgramWait;

    ///
    /// Timing parameters required for the SDRAM
    ///
    /// Specifies the value for EMC_RC
    NvU32 EmcRc;
    /// Specifies the value for EMC_RFC
    NvU32 EmcRfc;
    /// Specifies the value for EMC_RAS
    NvU32 EmcRas;
    /// Specifies the value for EMC_RP
    NvU32 EmcRp;
    /// Specifies the value for EMC_R2W
    NvU32 EmcR2w;
    /// Specifies the value for EMC_R2W
    NvU32 EmcW2r;
    /// Specifies the value for EMC_R2P
    NvU32 EmcR2p;
    /// Specifies the value for EMC_W2P
    NvU32 EmcW2p;
    /// Specifies the value for EMC_RD_RCD
    NvU32 EmcRdRcd;
    /// Specifies the value for EMC_WR_RCD
    NvU32 EmcWrRcd;
    /// Specifies the value for EMC_RRD
    NvU32 EmcRrd;
    /// Specifies the value for EMC_REXT
    NvU32 EmcRext;
    /// Specifies the value for EMC_WDV
    NvU32 EmcWdv;
    /// Specifies the value for EMC_QUSE
    NvU32 EmcQUse;
    /// Specifies the value for EMC_QRST
    NvU32 EmcQRst;
    /// Specifies the value for EMC_QSAFE
    NvU32 EmcQSafe;
    /// Specifies the value for EMC_RDV
    NvU32 EmcRdv;
    /// Specifies the value for EMC_REFRESH
    NvU32 EmcRefresh;
    /// Specifies the value for EMC_BURST_REFRESH_NUM
    NvU32 EmcBurstRefreshNum;
    /// Specifies the value for EMC_PDEX2WR
    NvU32 EmcPdEx2Wr;
    /// Specifies the value for EMC_PDEX2RD
    NvU32 EmcPdEx2Rd;
    /// Specifies the value for EMC_PCHG2PDEN
    NvU32 EmcPChg2Pden;
    /// Specifies the value for EMC_ACT2PDEN
    NvU32 EmcAct2Pden;
    /// Specifies the value for EMC_AR2PDEN
    NvU32 EmcAr2Pden;
    /// Specifies the value for EMC_RW2PDEN
    NvU32 EmcRw2Pden;
    /// Specifies the value for EMC_TXSR
    NvU32 EmcTxsr;
    /// Specifies the value for EMC_TCKE
    NvU32 EmcTcke;
    /// Specifies the value for EMC_TFAW
    NvU32 EmcTfaw;
    /// Specifies the value for EMC_TRPAB
    NvU32 EmcTrpab;
    /// Specifies the value for EMC_TCLKSTABLE
    NvU32 EmcTClkStable;
    /// Specifies the value for EMC_TCLKSTOP
    NvU32 EmcTClkStop;
    /// Specifies the value for EMC_TREFBW
    NvU32 EmcTRefBw;
    /// Specifies the value for EMC_QUSE_EXTRA
    NvU32 EmcQUseExtra;

    ///
    /// FBIO configuration values
    ///
    /// Specifies the value for EMC_FBIO_CFG1
    NvU32 EmcFbioCfg1;
    /// Specifies the value for EMC_FBIO_DQSIB_DLY
    NvU32 EmcFbioDqsibDly;
    /// Specifies the value for EMC_FBIO_DQSIB_DLY_MSB
    NvU32 EmcFbioDqsibDlyMsb;
    /// Specifies the value for EMC_FBIO_QUSE_DLY
    NvU32 EmcFbioQuseDly;
    /// Specifies the value for EMC_FBIO_QUSE_DLY_MSB
    NvU32 EmcFbioQuseDlyMsb;
    /// Specifies the value for EMC_FBIO_CFG5
    NvU32 EmcFbioCfg5;
    /// Specifies the value for EMC_FBIO_CFG6
    NvU32 EmcFbioCfg6;
    /// Specifies the value for EMC_FBIO_SPARE
    NvU32 EmcFbioSpare;

    ///
    /// MRS command values
    ///
    /// Specifies the value for EMC_MRS
    NvU32 EmcMrs;
    /// Specifies the value for EMC_EMRS
    NvU32 EmcEmrs;
    /// Specifies the first  of a sequence of three values for EMC_MRW
    NvU32 EmcMrw1;
    /// Specifies the second of a sequence of three values for EMC_MRW
    NvU32 EmcMrw2;
    /// Specifies the third  of a sequence of three values for EMC_MRW
    NvU32 EmcMrw3;

    /// Specifies the EMC_MRW reset command value
    NvU32 EmcMrwResetCommand;
    /// Specifies the EMC Reset wait time (in microseconds)
    NvU32 EmcMrwResetNInitWait;

    /// Specifies the value for EMC_ADR_CFG
    /// The same value is also used for MC_EMC_ADR_CFG
    NvU32 EmcAdrCfg;
    /// Specifies the value for EMC_ADR_CFG_1
    NvU32 EmcAdrCfg1;

    /// Specifies the value for MC_EMEM_CFG which holds the external memory
    /// size (in KBytes)
    /// EMEM_SIZE_KB must be <= (Device size in KB * Number of Devices)
    NvU32 McEmemCfg;

    /// Specifies the value for MC_LOWLATENCY_CONFIG
    /// Mainly for LL_DRAM_INTERLEAVE: Some DRAMs do not support interleave
    /// mode. If so, turn off this bit to get the correct low-latency path
    /// behavior. Reset is ENABLED.
    NvU32 McLowLatencyConfig;
    /// Specifies the value for EMC_CFG
    NvU32 EmcCfg;
    /// Specifies the value for EMC_CFG_2
    NvU32 EmcCfg2;
    /// Specifies the value for EMC_DBG
    NvU32 EmcDbg;

    /// Specifies the value for AHB_ARBITRATION_XBAR_CTRL.
    /// This is used to set the Memory Inid done
    NvU32 AhbArbitrationXbarCtrl;

    /// Specifies the value for EMC_CFG_DIG_DLL
    /// Note: Trigger bits are set by the SDRAM code.
    NvU32 EmcCfgDigDll;
    /// Specifies the value for EMC_DLL_XFORM_DQS
    NvU32 EmcDllXformDqs;
    /// Specifies the value for EMC_DLL_XFORM_QUSE
    NvU32 EmcDllXformQUse;

    /// Specifies the delay after prgramming the PIN/NOP register during a
    /// WarmBoot0 sequence (in microseconds)
    NvU32 WarmBootWait;

    /// Specifies the value for EMC_CTT_TERM_CTRL
    NvU32 EmcCttTermCtrl;

    /// Specifies the value for EMC_ODT_WRITE
    NvU32 EmcOdtWrite;
    /// Specifies the value for EMC_ODT_WRITE
    NvU32 EmcOdtRead;

    /// Specifies the value for EMC_ZCAL_REF_CNT
    /// Only meaningful for LPDDR2. Set to 0 for all other memory types.
    NvU32 EmcZcalRefCnt;
    /// Specifies the value for EMC_ZCAL_WAIT_CNT
    /// Only meaningful for LPDDR2. Set to 0 for all other memory types.
    NvU32 EmcZcalWaitCnt;
    /// Specifies the value for EMC_ZCAL_MRW_CMD
    /// Only meaningful for LPDDR2. Set to 0 for all other memory types.
    NvU32 EmcZcalMrwCmd;

    /// Specifies the MRS command value for initilizing the mode register.
    NvU32 EmcMrsResetDll;
    /// Specifies the MRW command for ZQ initialization of device 0
    NvU32 EmcMrwZqInitDev0;
    /// Specifies the MRW command for ZQ initialization of device 1
    NvU32 EmcMrwZqInitDev1;
    /// Specifies the wait time after programming a ZQ initialization command
    /// (in microseconds)
    NvU32 EmcMrwZqInitWait;
    /// Specifies the wait time after sending an MRS DLL reset command
    /// (in microseconds)
    NvU32 EmcMrsResetDllWait;
    /// Specifies the first of two EMRS commands to initialize mode registers
    NvU32 EmcEmrsEmr2;
    /// Specifies the second of two EMRS commands to initialize mode registers
    NvU32 EmcEmrsEmr3;
    /// Specifies the EMRS command to enable the DDR2 DLL
    NvU32 EmcEmrsDdr2DllEnable;
    /// Specifies the MRS command to reset the DDR2 DLL
    NvU32 EmcMrsDdr2DllReset;
    /// Specifies the EMRS command to set OCD calibration
    NvU32 EmcEmrsDdr2OcdCalib;
    /// Specifies the wait between initializing DDR and setting OCD
    /// calibration (in microseconds)
    NvU32 EmcDdr2Wait;

    ///
    /// Clock trimmers
    ///
    /// Specifies the value for EMC_CFG_CLKTRIM_0
    NvU32 EmcCfgClktrim0;
    /// Specifies the value for EMC_CFG_CLKTRIM_1
    NvU32 EmcCfgClktrim1;
    /// Specifies the value for EMC_CFG_CLKTRIM_2
    NvU32 EmcCfgClktrim2;

    ///
    /// Pad controls
    ///
    /// Specifies the value for PMC_DDR_PWR
    NvU32 PmcDdrPwr;
    /// Specifies the value for APB_MISC_GP_XM2CFGAPADCTRL
    NvU32 ApbMiscGpXm2CfgAPadCtrl;
    /// Specifies the value for APB_MISC_GP_XM2CFGCPADCTRL
    NvU32 ApbMiscGpXm2CfgCPadCtrl;
    /// Specifies the value for APB_MISC_GP_XM2CFGCPADCTRL2
    NvU32 ApbMiscGpXm2CfgCPadCtrl2;
    /// Specifies the value for APB_MISC_GP_XM2CFGDPADCTRL
    NvU32 ApbMiscGpXm2CfgDPadCtrl;
    /// Specifies the value for APB_MISC_GP_XM2CFGDPADCTRL2
    NvU32 ApbMiscGpXm2CfgDPadCtrl2;
    /// Specifies the value for APB_MISC_GP_XM2CLKCFGPADCTRL
    NvU32 ApbMiscGpXm2ClkCfgPadCtrl;
    /// Specifies the value for APB_MISC_GP_XM2COMPPADCTRL
    NvU32 ApbMiscGpXm2CompPadCtrl;
    /// Specifies the value for APB_MISC_GP_XM2VTTGENPADCTRL
    NvU32 ApbMiscGpXm2VttGenPadCtrl;

    /// Specifies storage for arbitration configuration registers
    /// Data passed through to the Bootloader but not used by the Boot ROM
    NvU32 ArbitrationConfig[NVBOOT_BCT_SDRAM_ARB_CONFIG_WORDS];
} NvBootSdramParams;

#if defined(__cplusplus)
}
#endif

#endif /* #ifndef INCLUDED_NVBOOT_SDRAM_PARAM_H */

