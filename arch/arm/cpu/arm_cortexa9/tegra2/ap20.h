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
#include "config.h"
#include <asm/arch/nvcommon.h>
#include <asm/arch/nv_drf.h>
#include <asm/arch/tegra2.h>
#include <asm/arch/nv_hardware_access.h>
#include <asm/arch/nvboot_bit.h>
#include <asm/arch/nvboot_osc.h>
#include <asm/arch/nvboot_clocks.h>
#include <asm/arch/nvbl_memmap_nvap.h>
#include <asm/arch/nvbl_arm_cpsr.h>
#include <asm/arch/nvbl_arm_cp15.h>
#include <asm/arch/nvboot_sdram_param.h>

#define _AND_ &

#define PG_UP_PA_BASE       0x60000000  // Base address for arpg.h registers
#define PMC_PA_BASE         0x7000E400  // Base address for arapbpm.h registers
#define CLK_RST_PA_BASE     0x60006000  // Base address for arclk_rst.h registers
#define TIMERUS_PA_BASE     0x60005010  // Base address for artimerus.h registers
#define FLOW_PA_BASE        0x60007000  // Base address for arflow_ctlr.h registers
#define EMC_PA_BASE         0x7000f400  // Base address for aremc.h registers
#define MC_PA_BASE          0x7000f000  // Base address for armc.h registers
#define MISC_PA_BASE        0x70000000  // Base address for arapb_misc.h registers
#define AHB_PA_BASE         0x6000C004  // Base address for arahb_arbc.h registers
#define EVP_PA_BASE         0x6000F000  // Base address for arevp.h registers
#define CSITE_PA_BASE       0x70040000  // Base address for arcsite.h registers
#define ARM_PREF_BASE       0x50040000

#define NV_PMC_REGR(pCar, reg)          NV_READ32( (((NvUPtr)(pCar)) + APBDEV_PMC_##reg##_0))
#define NV_PMC_REGW(pCar, reg, val)     NV_WRITE32((((NvUPtr)(pCar)) + APBDEV_PMC_##reg##_0), (val))
#define NV_FLOW_REGR(pFlow, reg)        NV_READ32((((NvUPtr)(pFlow)) + FLOW_CTLR_##reg##_0))
#define NV_FLOW_REGW(pFlow, reg, val)   NV_WRITE32((((NvUPtr)(pFlow)) + FLOW_CTLR_##reg##_0), (val))
#define NV_EVP_REGR(pEvp, reg)          NV_READ32( (((NvUPtr)(pEvp)) + EVP_##reg##_0))
#define NV_EVP_REGW(pEvp, reg, val)     NV_WRITE32((((NvUPtr)(pEvp)) + EVP_##reg##_0), (val))

#define USE_PLLC_OUT1               0       // 0 ==> PLLP_OUT4, 1 ==> PLLC_OUT1
#define NVBL_PLL_BYPASS 0
#define NVBL_PLLP_SUBCLOCKS_FIXED   (1)

#define NV_CAR_REGR(pCar, reg)              NV_READ32( (((NvUPtr)(pCar)) + CLK_RST_CONTROLLER_##reg##_0))
#define NV_CAR_REGW(pCar, reg, val)         NV_WRITE32((((NvUPtr)(pCar)) + CLK_RST_CONTROLLER_##reg##_0), (val))
#define NV_TIMERUS_REGR(pTimer, reg)        NV_READ32((((NvUPtr)(pTimer)) + TIMERUS_##reg##_0))
#define NV_TIMERUS_REGW(pTimer, reg, val)   NV_WRITE32((((NvUPtr)(pTimer)) + TIMERUS_##reg##_0), (val))

#define NV_MISC_REGR(pMisc, reg)                NV_READ32( (((NvUPtr)(pMisc)) + APB_MISC_##reg##_0))
#define NV_MISC_REGW(pMisc, reg, val)           NV_WRITE32((((NvUPtr)(pMisc)) + APB_MISC_##reg##_0), (val))

#define NV_MC_REGR(pMc, reg)                    NV_READ32( (((NvUPtr)(pMc)) + MC_##reg##_0))
#define NV_MC_REGW(pMc, reg, val)               NV_WRITE32((((NvUPtr)(pMc)) + MC_##reg##_0), (val))

#define NV_EMC_REGR(pEmc, reg)                  NV_READ32( (((NvUPtr)(pEmc)) + EMC_##reg##_0))
#define NV_EMC_REGW(pEmc, reg, val)             NV_WRITE32((((NvUPtr)(pEmc)) + EMC_##reg##_0), (val))

#define NV_AHB_ARBC_REGR(pArb, reg)             NV_READ32( (((NvUPtr)(pArb)) + AHB_ARBITRATION_##reg##_0))
#define NV_AHB_ARBC_REGW(pArb, reg, val)        NV_WRITE32((((NvUPtr)(pArb)) + AHB_ARBITRATION_##reg##_0), (val))

#define NV_MEMC_REGR(pMc, reg)                  NV_READ32( (((NvUPtr)(pMc)) + MEMC_##reg##_0))
#define NV_MEMC_REGW(pMc, reg, val)             NV_WRITE32((((NvUPtr)(pMc)) + MEMC_##reg##_0), (val))

#define NV_AHB_GIZMO_REGR(pAhbGizmo, reg)       NV_READ32( (((NvUPtr)(pAhbGizmo)) + AHB_GIZMO_##reg##_0))
#define NV_AHB_GIZMO_REGW(pAhbGizmo, reg, val)  NV_WRITE32((((NvUPtr)(pAhbGizmo)) + AHB_GIZMO_##reg##_0), (val))

#define NV_ARCSITE_REGR(pCsite, reg)        NV_READ32( (((NvUPtr)(pCsite)) + CSITE_##reg##_0))
#define NV_ARCSITE_REGW(pCsite, reg, val)   NV_WRITE32((((NvUPtr)(pCsite)) + CSITE_##reg##_0), (val))

#define NV_CEIL(time, clock)     (((time) + (clock) - 1)/(clock))

#define NVBL_PLLM_KHZ               167000
#define NVBL_PLLP_KHZ               (432000/2)
#define DISABLE_LOW_LATENCY_PATH 0

/// Calculate clock fractional divider value from reference and target frequencies
#define CLK_DIVIDER(REF, FREQ)  ((((REF) * 2) / FREQ) - 2)

/// Calculate clock frequency value from reference and clock divider value
#define CLK_FREQUENCY(REF, REG)  (((REF) * 2) / (REG + 2))

#define SCU_CONTROL_0                   _MK_ADDR_CONST(0x0)
#define SCU_CONTROL_0_SCU_ENABLE_RANGE                  0:0
#define SCU_INVALID_ALL_0                       _MK_ADDR_CONST(0xc)

#define NV_SCU_REGR(reg)      NV_READ32(ARM_PREF_BASE + SCU_##reg##_0)
#define NV_SCU_REGW(reg, val) NV_WRITE32((ARM_PREF_BASE + SCU_##reg##_0), (val))

//------------------------------------------------------------------------------
// Provide missing enumerators for spec files.
//------------------------------------------------------------------------------

#define NV_BIT_ADDRESS 0x40000000
#define NV3P_SIGNATURE 0x5AFEADD8

void NvBlStartCpu_AP20(NvU32 ResetVector);
void NvBlAvpHalt_AP20(void);
NV_NAKED void NvBlStartUpAvp_AP20( void );
NV_NAKED void ColdBoot_AP20( void );
void tegra2_start(void);
void uart_post(char c);
void NvBlUartInit(void);
void cpu_start(void);
void cpu_init_crit(void);
void PostZz(void);
void PostYy(void);
void PostXx(void);
void NvBlPrintU32(NvU32);

