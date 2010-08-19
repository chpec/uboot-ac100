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
#include "ap20.h"

#define NV_ASSERT(p) \
{    if ((p) == 0) { \
         uart_post('C');  \
         uart_post('o');  \
         uart_post('l');  \
         uart_post('d');  \
    };               \
}

void NvBlAvpStallUs(NvU32 MicroSec)
{
    NvU32           Reg;            // Flow controller register
    NvU32           Delay;          // Microsecond delay time
    NvU32           MaxUs;          // Maximum flow controller delay

    // Get the maxium delay per loop.
    MaxUs = NV_DRF_NUM(FLOW_CTLR, HALT_COP_EVENTS, ZERO, 0xFFFFFFFF);

    while (MicroSec)
    {
        Delay     = (MicroSec > MaxUs) ? MaxUs : MicroSec;
        MicroSec -= Delay;

        Reg = NV_DRF_NUM(FLOW_CTLR, HALT_COP_EVENTS, ZERO, Delay)
            | NV_DRF_NUM(FLOW_CTLR, HALT_COP_EVENTS, uSEC, 1)
            | NV_DRF_NUM(FLOW_CTLR, HALT_COP_EVENTS, MODE, 2);

        NV_FLOW_REGW(FLOW_PA_BASE, HALT_COP_EVENTS, Reg);
    }
}


void NvBlAvpStallMs(NvU32 MilliSec)
{
    NvU32           Reg;            // Flow controller register
    NvU32           Delay;          // Millisecond delay time
    NvU32           MaxMs;          // Maximum flow controller delay

    // Get the maxium delay per loop.
    MaxMs = NV_DRF_NUM(FLOW_CTLR, HALT_COP_EVENTS, ZERO, 0xFFFFFFFF);

    while (MilliSec)
    {
        Delay     = (MilliSec > MaxMs) ? MaxMs : MilliSec;
        MilliSec -= Delay;

        Reg = NV_DRF_NUM(FLOW_CTLR, HALT_COP_EVENTS, ZERO, Delay)
            | NV_DRF_NUM(FLOW_CTLR, HALT_COP_EVENTS, MSEC, 1)
            | NV_DRF_NUM(FLOW_CTLR, HALT_COP_EVENTS, MODE, 2);

        NV_FLOW_REGW(FLOW_PA_BASE, HALT_COP_EVENTS, Reg);
    }
}

static NvU32  NvBlQueryBootCpuFrequency_NVAP( void )
{
    NvU32   frequency;

    #if     NVBL_PLL_BYPASS
        // In bypass mode we run at the oscillator frequency.
        frequency = NvBlQueryBootOscillatorFrequency_NVAP(ChipId);
    #else
        //frequency = (ChipId->SKU == FUSE_SKU_INFO_0_AP15L) ? 450000 : 600000;
        frequency = 600000;
    #endif

    return frequency;
}

static void InitPllX(void)
{
    NvU32               Reg;        // Scratch
    NvU32               Divm;       // Reference
    NvU32               Divn;       // Multiplier
    NvU32               Divp;       // Divider == Divp ^ 2
    NvBootClocksOscFreq OscFreq;    // Oscillator frequency

    // Is PLL-X already running?
    Reg = NV_CAR_REGR(CLK_RST_PA_BASE, PLLX_BASE);
    Reg = NV_DRF_VAL(CLK_RST_CONTROLLER, PLLX_BASE, PLLX_ENABLE, Reg);
    if (Reg == NV_DRF_DEF(CLK_RST_CONTROLLER, PLLX_BASE, PLLX_ENABLE, ENABLE))
    {
        return;
    }

    Reg = NV_DRF_NUM(CLK_RST_CONTROLLER, PLLX_MISC, PLLX_CPCON, 1)
        | NV_DRF_NUM(CLK_RST_CONTROLLER, PLLX_MISC, PLLX_LFCON, 0);
    NV_CAR_REGW(CLK_RST_PA_BASE, PLLX_MISC, Reg);

    {
        Divm = 1;
        Divp = 0;
        Divn = NvBlQueryBootCpuFrequency_NVAP() / 1000;

        // Operating below the 50% point of the divider's range?
        if (Divn <= (NV_DRF_VAL(CLK_RST_CONTROLLER, PLLX_BASE, PLLX_DIVN, ~0)/2))
        {
            // Yes, double the post divider and the feedback divider.
            Divp = 1;
            Divn <<= Divp;
        }
        // Operating above the range of the feedback divider?
        else if (Divn > NV_DRF_VAL(CLK_RST_CONTROLLER, PLLX_BASE, PLLX_DIVN, ~0))
        {
            // Yes, double the input divider and halve the feedback divider.
            Divn >>= 1;
            Divm = 2;
        }

        // Get the oscillator frequency.
        OscFreq  = ((NvBootInfoTable*)(AP20_BASE_PA_BOOT_INFO))->OscFrequency;

        // Program PLL-X.
        switch (OscFreq)
        {
            case NvBootClocksOscFreq_13:
                #if NVBL_PLL_BYPASS
                    Reg = 0x80000D0D;
                #else
                Divm = (Divm == 1) ? 13 : (13 / Divm);
                Reg = NV_DRF_DEF(CLK_RST_CONTROLLER, PLLX_BASE, PLLX_BYPASS, ENABLE)
                    | NV_DRF_DEF(CLK_RST_CONTROLLER, PLLX_BASE, PLLX_ENABLE, DISABLE)
                    | NV_DRF_DEF(CLK_RST_CONTROLLER, PLLX_BASE, PLLX_REF_DIS, REF_ENABLE)
                    | NV_DRF_NUM(CLK_RST_CONTROLLER, PLLX_BASE, PLLX_LOCK, 0x0)
                    | NV_DRF_NUM(CLK_RST_CONTROLLER, PLLX_BASE, PLLX_DIVP, Divp)
                    | NV_DRF_NUM(CLK_RST_CONTROLLER, PLLX_BASE, PLLX_DIVN, Divn)
                    | NV_DRF_NUM(CLK_RST_CONTROLLER, PLLX_BASE, PLLX_DIVM, Divm);
                #endif
                break;

            case NvBootClocksOscFreq_19_2:
                // NOTE: With a 19.2 MHz oscillator, the PLL will run 1.05% faster
                //       than the target frequency.
                #if NVBL_PLL_BYPASS
                    Reg = 0x80001313;
                #else
                Divm = (Divm == 1) ? 19 : (19 / Divm);
                Reg = NV_DRF_DEF(CLK_RST_CONTROLLER, PLLX_BASE, PLLX_BYPASS, ENABLE)
                    | NV_DRF_DEF(CLK_RST_CONTROLLER, PLLX_BASE, PLLX_ENABLE, DISABLE)
                    | NV_DRF_DEF(CLK_RST_CONTROLLER, PLLX_BASE, PLLX_REF_DIS, REF_ENABLE)
                    | NV_DRF_NUM(CLK_RST_CONTROLLER, PLLX_BASE, PLLX_LOCK, 0x0)
                    | NV_DRF_NUM(CLK_RST_CONTROLLER, PLLX_BASE, PLLX_DIVP, Divp)
                    | NV_DRF_NUM(CLK_RST_CONTROLLER, PLLX_BASE, PLLX_DIVN, Divn)
                    | NV_DRF_NUM(CLK_RST_CONTROLLER, PLLX_BASE, PLLX_DIVM, Divm);
                #endif
                break;

            case NvBootClocksOscFreq_12:
                #if NVBL_PLL_BYPASS
                    Reg = 0x80000C0C;
                #else
                Divm = (Divm == 1) ? 12 : (12 / Divm);
                Reg = NV_DRF_DEF(CLK_RST_CONTROLLER, PLLX_BASE, PLLX_BYPASS, ENABLE)
                    | NV_DRF_DEF(CLK_RST_CONTROLLER, PLLX_BASE, PLLX_ENABLE, DISABLE)
                    | NV_DRF_DEF(CLK_RST_CONTROLLER, PLLX_BASE, PLLX_REF_DIS, REF_ENABLE)
                    | NV_DRF_NUM(CLK_RST_CONTROLLER, PLLX_BASE, PLLX_LOCK, 0x0)
                    | NV_DRF_NUM(CLK_RST_CONTROLLER, PLLX_BASE, PLLX_DIVP, Divp)
                    | NV_DRF_NUM(CLK_RST_CONTROLLER, PLLX_BASE, PLLX_DIVN, Divn)
                    | NV_DRF_NUM(CLK_RST_CONTROLLER, PLLX_BASE, PLLX_DIVM, Divm);
                #endif
                break;

            case NvBootClocksOscFreq_26:
                #if NVBL_PLL_BYPASS
                    Reg = 0x80001A1A;
                #else
                Divm = (Divm == 1) ? 26 : (26 / Divm);
                Reg = NV_DRF_DEF(CLK_RST_CONTROLLER, PLLX_BASE, PLLX_BYPASS, ENABLE)
                    | NV_DRF_DEF(CLK_RST_CONTROLLER, PLLX_BASE, PLLX_ENABLE, DISABLE)
                    | NV_DRF_DEF(CLK_RST_CONTROLLER, PLLX_BASE, PLLX_REF_DIS, REF_ENABLE)
                    | NV_DRF_NUM(CLK_RST_CONTROLLER, PLLX_BASE, PLLX_LOCK, 0x0)
                    | NV_DRF_NUM(CLK_RST_CONTROLLER, PLLX_BASE, PLLX_DIVP, Divp)
                    | NV_DRF_NUM(CLK_RST_CONTROLLER, PLLX_BASE, PLLX_DIVN, Divn)
                    | NV_DRF_NUM(CLK_RST_CONTROLLER, PLLX_BASE, PLLX_DIVM, Divm);
                #endif
                break;

            default:
                NV_ASSERT(0);
        }
    }

    NV_CAR_REGW(CLK_RST_PA_BASE, PLLX_BASE, Reg);

    Reg = NV_FLD_SET_DRF_DEF(CLK_RST_CONTROLLER, PLLX_BASE, PLLX_ENABLE, ENABLE, Reg);
    NV_CAR_REGW(CLK_RST_PA_BASE, PLLX_BASE, Reg);

#if !NVBL_PLL_BYPASS
    Reg = NV_FLD_SET_DRF_DEF(CLK_RST_CONTROLLER, PLLX_BASE, PLLX_BYPASS, DISABLE, Reg);
    NV_CAR_REGW(CLK_RST_PA_BASE, PLLX_BASE, Reg);
#endif
}

static void NvBlAvpSetCpuResetVectorAp20(NvU32 reset)
{
    NV_EVP_REGW(EVP_PA_BASE, CPU_RESET_VECTOR, reset);
}

static void NvBlAvpEnableCpuClockAp20(NvBool enable)
{
    // !!!WARNING!!! THIS FUNCTION MUST NOT USE ANY GLOBAL VARIABLES
    // !!!WARNING!!! THIS FUNCTION MUST NOT USE THE RELOCATION TABLE
    // !!!WARNING!!! THIS FUNCTION MUST NOT BE CALLED FROM THE CPU
    // !!!WARNING!!! THIS FUNCTION MUST BE CALLED WITH A FLAT VIRTUAL ADDRESSING MAP

    NvU32   Reg;        // Scratch reg
    NvU32   Clk;        // Scratch reg

    //-------------------------------------------------------------------------
    // NOTE:  Regardless of whether the request is to enable or disable the CPU
    //        clock, every processor in the CPU complex except the master (CPU
    //        0) will have it's clock stopped because the AVP only talks to the
    //        master. The AVP, it does not know, nor does it need to know that
    //        there are multiple processors in the CPU complex.
    //-------------------------------------------------------------------------

    // Always halt CPU 1 at the flow controller so that in uni-processor
    // configurations the low-power trigger conditions will work properly.
    Reg = NV_DRF_DEF(FLOW_CTLR, HALT_CPU1_EVENTS, MODE, FLOW_MODE_STOP);
    //NV_FLOW_REGW(FLOW_PA_BASE, HALT_CPU1_EVENTS, Reg);    // BUG 557466

    // Need to initialize PLLX?
    if (enable)
    {
        // Initialize PLLX.
        InitPllX();

        // Wait until stable
        NvBlAvpStallUs(NVBOOT_CLOCKS_PLL_STABILIZATION_DELAY);

        //*((volatile long *)0x60006020) = 0x20008888 ;// CCLK_BURST_POLICY
        Reg = NV_DRF_NUM(CLK_RST_CONTROLLER, CCLK_BURST_POLICY, CPU_STATE, 0x2)
            | NV_DRF_NUM(CLK_RST_CONTROLLER, CCLK_BURST_POLICY, COP_AUTO_CWAKEUP_FROM_FIQ, 0x0)
            | NV_DRF_NUM(CLK_RST_CONTROLLER, CCLK_BURST_POLICY, CPU_AUTO_CWAKEUP_FROM_FIQ, 0x0)
            | NV_DRF_NUM(CLK_RST_CONTROLLER, CCLK_BURST_POLICY, COP_AUTO_CWAKEUP_FROM_IRQ, 0x0)
            | NV_DRF_NUM(CLK_RST_CONTROLLER, CCLK_BURST_POLICY, CPU_AUTO_CWAKEUP_FROM_IRQ, 0x0)
            | NV_DRF_DEF(CLK_RST_CONTROLLER, CCLK_BURST_POLICY, CWAKEUP_FIQ_SOURCE, PLLX_OUT0)
            | NV_DRF_DEF(CLK_RST_CONTROLLER, CCLK_BURST_POLICY, CWAKEUP_IRQ_SOURCE, PLLX_OUT0)
            | NV_DRF_DEF(CLK_RST_CONTROLLER, CCLK_BURST_POLICY, CWAKEUP_RUN_SOURCE, PLLX_OUT0)
            | NV_DRF_DEF(CLK_RST_CONTROLLER, CCLK_BURST_POLICY, CWAKEUP_IDLE_SOURCE, PLLX_OUT0);
        NV_CAR_REGW(CLK_RST_PA_BASE, CCLK_BURST_POLICY, Reg);

        //*((volatile long *)0x60006024) = 0x80000000 ;// SUPER_CCLK_DIVIDER
        Reg = NV_DRF_NUM(CLK_RST_CONTROLLER, SUPER_CCLK_DIVIDER, SUPER_CDIV_ENB, 0x1)
            | NV_DRF_NUM(CLK_RST_CONTROLLER, SUPER_CCLK_DIVIDER, SUPER_CDIV_DIS_FROM_COP_FIQ, 0x0)
            | NV_DRF_NUM(CLK_RST_CONTROLLER, SUPER_CCLK_DIVIDER, SUPER_CDIV_DIS_FROM_CPU_FIQ, 0x0)
            | NV_DRF_NUM(CLK_RST_CONTROLLER, SUPER_CCLK_DIVIDER, SUPER_CDIV_DIS_FROM_COP_IRQ, 0x0)
            | NV_DRF_NUM(CLK_RST_CONTROLLER, SUPER_CCLK_DIVIDER, SUPER_CDIV_DIS_FROM_CPU_IRQ, 0x0)
            | NV_DRF_NUM(CLK_RST_CONTROLLER, SUPER_CCLK_DIVIDER, SUPER_CDIV_DIVIDEND, 0x0)
            | NV_DRF_NUM(CLK_RST_CONTROLLER, SUPER_CCLK_DIVIDER, SUPER_CDIV_DIVISOR, 0x0);
        NV_CAR_REGW(CLK_RST_PA_BASE, SUPER_CCLK_DIVIDER, Reg);
    }

    // Read the register containing the main CPU complex clock enable.
    Reg = NV_CAR_REGR(CLK_RST_PA_BASE, CLK_OUT_ENB_L);

    // Read the register containing the individual CPU clock enables and
    // always stop the clock to CPU 1.
    Clk = NV_CAR_REGR(CLK_RST_PA_BASE, CLK_CPU_CMPLX);
    Clk = NV_FLD_SET_DRF_NUM(CLK_RST_CONTROLLER, CLK_CPU_CMPLX, CPU1_CLK_STP, 1, Clk);

    if (enable)
    {
        // Enable the CPU clock.
        Reg = NV_FLD_SET_DRF_DEF(CLK_RST_CONTROLLER, CLK_OUT_ENB_L, CLK_ENB_CPU, ENABLE, Reg);
        Clk  = NV_FLD_SET_DRF_NUM(CLK_RST_CONTROLLER, CLK_CPU_CMPLX, CPU0_CLK_STP, 0, Clk);
    }
    else
    {
        // Disable the CPU clock.
        Reg = NV_FLD_SET_DRF_DEF(CLK_RST_CONTROLLER, CLK_OUT_ENB_L, CLK_ENB_CPU, ENABLE, Reg);
        Clk = NV_FLD_SET_DRF_NUM(CLK_RST_CONTROLLER, CLK_CPU_CMPLX, CPU0_CLK_STP, 1, Clk);
    }
    NV_CAR_REGW(CLK_RST_PA_BASE, CLK_CPU_CMPLX, Clk);
    NV_CAR_REGW(CLK_RST_PA_BASE, CLK_OUT_ENB_L, Reg);
}

//----------------------------------------------------------------------------------------------
static NvBool NvBlAvpIsCpuPoweredAp20(void)
{
    NvU32   Reg;        // Scratch reg

    Reg = NV_PMC_REGR(PMC_PA_BASE, PWRGATE_STATUS);

    if (!NV_DRF_VAL(APBDEV_PMC, PWRGATE_STATUS, CPU, Reg))
    {
        return NV_FALSE;
    }

    return NV_TRUE;
}

static void NvBlAvpRemoveCpuIoClampsAp20(void)
{
    NvU32   Reg;        // Scratch reg

    // Remove the clamps on the CPU I/O signals.
    Reg = NV_DRF_DEF(APBDEV_PMC, REMOVE_CLAMPING_CMD, CPU, ENABLE);
    NV_PMC_REGW(PMC_PA_BASE, REMOVE_CLAMPING_CMD, Reg);

    // Give I/O signals time to stabilize.
    NvBlAvpStallMs(1);
}

static void  NvBlAvpPowerUpCpuAp20(void)
{
    NvU32   Reg;        // Scratch reg

    if (!NvBlAvpIsCpuPoweredAp20())
    {
        // Toggle the CPU power state (OFF -> ON).
        Reg = NV_DRF_DEF(APBDEV_PMC, PWRGATE_TOGGLE, PARTID, CP)
            | NV_DRF_DEF(APBDEV_PMC, PWRGATE_TOGGLE, START, ENABLE);
        NV_PMC_REGW(PMC_PA_BASE, PWRGATE_TOGGLE, Reg);

        // Wait for the power to come up.
        while (!NvBlAvpIsCpuPoweredAp20())
        {
            // Do nothing
        }

        // Remove the I/O clamps from CPU power partition.
        // Recommended only a Warm boot, if the CPU partition gets power gated.
        // Shouldn't cause any harm, when called after a cold boot, according to h/w.
        // probably just redundant.
        NvBlAvpRemoveCpuIoClampsAp20();
    }
}

static void NvBlAvpEnableCpuPowerRailAp20(void)
{
    NvU32   Reg;        // Scratch reg

    Reg = NV_PMC_REGR(PMC_PA_BASE, CNTRL);
    Reg = NV_FLD_SET_DRF_DEF(APBDEV_PMC, CNTRL, CPUPWRREQ_OE, ENABLE, Reg);
    NV_PMC_REGW(PMC_PA_BASE, CNTRL, Reg);
}

static void NvBlAvpResetCpuAp20(NvBool reset)
{
    NvU32   Reg;    // Scratch reg
    NvU32   Cpu;    // Scratch reg

    //-------------------------------------------------------------------------
    // NOTE:  Regardless of whether the request is to hold the CPU in reset or
    //        take it out of reset, every processor in the CPU complex except
    //        the master (CPU 0) will be held in reset because the AVP only
    //        talks to the master. The AVP does not know, nor does it need to
    //        know, that there are multiple processors in the CPU complex.
    //-------------------------------------------------------------------------

    // Hold CPU 1 in reset.
    Cpu = NV_DRF_NUM(CLK_RST_CONTROLLER, RST_CPU_CMPLX_SET, SET_CPURESET1, 1)
        | NV_DRF_NUM(CLK_RST_CONTROLLER, RST_CPU_CMPLX_SET, SET_DBGRESET1, 1)
        | NV_DRF_NUM(CLK_RST_CONTROLLER, RST_CPU_CMPLX_SET, SET_DERESET1,  1);
    NV_CAR_REGW(CLK_RST_PA_BASE, RST_CPU_CMPLX_SET, Cpu);

    Reg = NV_CAR_REGR(CLK_RST_PA_BASE, RST_DEVICES_L);
    if (reset)
    {
        // Place CPU0 into reset.
        Cpu = NV_DRF_NUM(CLK_RST_CONTROLLER, RST_CPU_CMPLX_SET, SET_CPURESET0, 1)
            | NV_DRF_NUM(CLK_RST_CONTROLLER, RST_CPU_CMPLX_SET, SET_DBGRESET0, 1)
            | NV_DRF_NUM(CLK_RST_CONTROLLER, RST_CPU_CMPLX_SET, SET_DERESET0,  1);
        NV_CAR_REGW(CLK_RST_PA_BASE, RST_CPU_CMPLX_SET, Cpu);

        // Enable master CPU reset.
        Reg = NV_FLD_SET_DRF_DEF(CLK_RST_CONTROLLER, RST_DEVICES_L, SWR_CPU_RST, ENABLE, Reg);
    }
    else
    {
        // Take CPU0 out of reset.
        Cpu = NV_DRF_NUM(CLK_RST_CONTROLLER, RST_CPU_CMPLX_CLR, CLR_CPURESET0, 1)
            | NV_DRF_NUM(CLK_RST_CONTROLLER, RST_CPU_CMPLX_CLR, CLR_DBGRESET0, 1)
            | NV_DRF_NUM(CLK_RST_CONTROLLER, RST_CPU_CMPLX_CLR, CLR_DERESET0,  1);
        NV_CAR_REGW(CLK_RST_PA_BASE, RST_CPU_CMPLX_CLR, Cpu);
        // Disable master CPU reset.
        Reg = NV_FLD_SET_DRF_DEF(CLK_RST_CONTROLLER, RST_DEVICES_L, SWR_CPU_RST, DISABLE, Reg);
    }
    NV_CAR_REGW(CLK_RST_PA_BASE, RST_DEVICES_L, Reg);
}

void NvBlAvpClockEnableCorsightAp20(NvBool enable)
{
    NvU32   Rst;        // Scratch register
    NvU32   Clk;        // Scratch register
    NvU32   Src;        // Scratch register

    Rst = NV_CAR_REGR(CLK_RST_PA_BASE, RST_DEVICES_U);
    Clk = NV_CAR_REGR(CLK_RST_PA_BASE, CLK_OUT_ENB_U);

    if (enable)
    {
        Rst = NV_FLD_SET_DRF_DEF(CLK_RST_CONTROLLER, RST_DEVICES_U, SWR_CSITE_RST, DISABLE, Rst);
        Clk = NV_FLD_SET_DRF_DEF(CLK_RST_CONTROLLER, CLK_OUT_ENB_U, CLK_ENB_CSITE, ENABLE, Clk);
    }
    else
    {
        Rst = NV_FLD_SET_DRF_DEF(CLK_RST_CONTROLLER, RST_DEVICES_U, SWR_CSITE_RST, ENABLE, Rst);
        Clk = NV_FLD_SET_DRF_DEF(CLK_RST_CONTROLLER, CLK_OUT_ENB_U, CLK_ENB_CSITE, DISABLE, Clk);
    }

    NV_CAR_REGW(CLK_RST_PA_BASE, CLK_OUT_ENB_U, Clk);
    NV_CAR_REGW(CLK_RST_PA_BASE, RST_DEVICES_U, Rst);

    if (enable)
    {
        // Put CoreSight on PLLP_OUT0 (216 MHz) and divide it down by 1.5
        // giving an effective frequency of 144MHz.
        {
            // Note that CoreSight has a fractional divider (LSB == .5).
            Src = NV_DRF_DEF(CLK_RST_CONTROLLER, CLK_SOURCE_CSITE, CSITE_CLK_SRC, PLLP_OUT0)
                | NV_DRF_NUM(CLK_RST_CONTROLLER, CLK_SOURCE_CSITE, CSITE_CLK_DIVISOR, CLK_DIVIDER(NVBL_PLLP_KHZ, 144000));
            NV_CAR_REGW(CLK_RST_PA_BASE, CLK_SOURCE_CSITE, Src);
        }

        // Unlock the CPU CoreSight interfaces.
        NV_ARCSITE_REGW(CSITE_PA_BASE, CPUDBG0_LAR, 0xC5ACCE55);
        NV_ARCSITE_REGW(CSITE_PA_BASE, CPUDBG1_LAR, 0xC5ACCE55);
    }
}

void NvBlStartCpu_AP20(NvU32 ResetVector)
{

    uart_post('P');

    // Enable VDD_CPU
    NvBlAvpEnableCpuPowerRailAp20();
    uart_post('1');

    // Hold the CPUs in reset.
    NvBlAvpResetCpuAp20(NV_TRUE);
    uart_post('2');

    // Disable the CPU clock.
    NvBlAvpEnableCpuClockAp20(NV_FALSE);
    uart_post('3');

    // Enable CoreSight.
    NvBlAvpClockEnableCorsightAp20(NV_TRUE);
    uart_post('4');

    // Set the entry point for CPU execution from reset, if it's a non-zero value.
    if (ResetVector)
    {
        NvBlAvpSetCpuResetVectorAp20(ResetVector);
        uart_post('5');
    }

    // Enable the CPU clock.
    NvBlAvpEnableCpuClockAp20(NV_TRUE);
    uart_post('6');

    // Does the CPU already have power?
    if (!NvBlAvpIsCpuPoweredAp20())
    {
        // Power up the CPU.
        NvBlAvpPowerUpCpuAp20();
        uart_post('7');
    }
    uart_post('8');

    // Remove the I/O clamps from CPU power partition.
    //NvBlAvpRemoveCpuIoClampsAp20();

    // Take the CPU out of reset.
    NvBlAvpResetCpuAp20(NV_FALSE);
    uart_post('9');

}

void NvBlAvpHalt_AP20(void)
{
    NvU32   Reg;    // Scratch reg

    uart_post('H');
    for (;;)
    {
        Reg = NV_DRF_DEF(FLOW_CTLR, HALT_COP_EVENTS, MODE, FLOW_MODE_STOP)
            | NV_DRF_NUM(FLOW_CTLR, HALT_COP_EVENTS, JTAG, 1)
            | NV_DRF_NUM(FLOW_CTLR, HALT_COP_EVENTS, IRQ_1, 1)
            | NV_DRF_NUM(FLOW_CTLR, HALT_COP_EVENTS, FIQ_1, 1);
        NV_FLOW_REGW(FLOW_PA_BASE, HALT_COP_EVENTS, Reg);
    }
    //;------------------------------------------------------------------
    //; Should never get here.
    //;------------------------------------------------------------------
    NV_ASSERT(0);
    for (;;);
}

NV_NAKED void NvBlStartUpAvp_AP20( void )
{
    //;------------------------------------------------------------------
    //; Initialize the AVP, clocks, and memory controller.
    //;------------------------------------------------------------------

    asm volatile(
    //The SDRAM is guaranteed to be on at this point
    //in the nvml environment. Set r0 = 1.
#if 0 //  TODO: check back in case of warm boot
    "MOV     r0, #1                          \n"
    "BL      NvBlAvpInit_AP20                \n"
#endif

    //;------------------------------------------------------------------
    //; Start the CPU.
    //;------------------------------------------------------------------
    "LDR     r0, =ColdBoot_AP20              \n"//; R0 = reset vector for CPU
    "BL      NvBlStartCpu_AP20               \n"

    //;------------------------------------------------------------------
    //; Transfer control to the AVP code.
    //;------------------------------------------------------------------
    "BL      NvBlAvpHalt_AP20                \n"

    //;------------------------------------------------------------------
    //; Should never get here.
    //;------------------------------------------------------------------
    "B       .                               \n"
    );
}

// we're hard coding the entry point for all AOS images
NvU32 cpu_boot_stack = NVAP_LIMIT_PA_IRAM_CPU_EARLY_BOOT_STACK;
extern NvU32 _armboot_start;
NvU32 proc_tag = PG_UP_TAG_0_PID_CPU _AND_ 0xFF;
NvU32 avp_boot_stack = NVAP_LIMIT_PA_IRAM_AVP_EARLY_BOOT_STACK;
NvU32 deadbeef = 0xdeadbeef;

NV_NAKED void ColdBoot_AP20( void )
{
    uart_post('C');
    asm volatile(
    "MSR     CPSR_c, #0xd3                                          \n"
    //;------------------------------------------------------------------
    //; Check current processor: CPU or AVP?
    //; If AVP, go to AVP boot code, else continue on.
    //;------------------------------------------------------------------
    "MOV     r0, %0                                                 \n"
    "LDRB    r2, [r0, %1]                                           \n"
     //;are we the CPU?
    "CMP     r2, %2                                                 \n"
    "MOV     sp, %3           \n"
    // leave in some symbols for release debugging
    "MOV     r3, %6                                                 \n"
    "STR     r3, [sp, #-4]!                                         \n"
    "STR     r3, [sp, #-4]!                                         \n"
    //; yep, we are the CPU
    "BXEQ     %4                                                    \n"
    //;==================================================================
    //; AVP Initialization follows this path
    //;==================================================================
     "MOV     sp, %5                                                 \n"
    // leave in some symbols for release debugging
    "MOV     r3, %6                                                 \n"
    "STR     r3, [sp, #-4]!                                         \n"
    "STR     r3, [sp, #-4]!                                         \n"
    //;------------------------------------------------------------------
    //; Init UART A
    //;------------------------------------------------------------------
//    "BL      NvBlAvpClockInit                                       \n"
//    "BL      NvBlAvpUartInit                                        \n"
    //;------------------------------------------------------------------
    //; Init and Start CPU  
    //;------------------------------------------------------------------
    "B       NvBlStartUpAvp_AP20                                    \n"
    :
    :"I"(PG_UP_PA_BASE),
     "I"(PG_UP_TAG_0),
     "r"(proc_tag),
     "r"(cpu_boot_stack),
     "r"(_armboot_start),
     "r"(avp_boot_stack),
     "r"(deadbeef)
    : "r0", "r2", "r3", "cc", "lr"
    );
}

NvU32 s_ChipId;
volatile NvU32 s_bFirstBoot = 1;

void cpu_start( void )
{
    volatile NvU32 *jtagReg = (NvU32*)0x70000024;
    NvU32 reg;

    // enable JTAG
    *jtagReg = 192;

    reg = NV_MISC_REGR( MISC_PA_BASE, GP_HIDREV );

    // DRF macros generate too-complicated code for the arm7
    s_ChipId = reg >> 8;
    s_ChipId &= 0xff;
    if( s_bFirstBoot )
    {
        uart_post('i');
        /* need to set this before cold-booting, otherwise we'll end up in
         * an infinite loop.
         */
        s_bFirstBoot = 0;

        switch( s_ChipId ) {
        case 0x20:
            ColdBoot_AP20();
            break;
        default:
//            NV_ASSERT( !"unknown chipid" );
            break;
        }
    }

    uart_post('x');
    return;
}

void tegra2_start()
{
        /* Init Debug UART Port (115200 8n1)*/
        NvBlUartInitA();

        /* post code 'Zz' */
        PostZz();

#ifdef CONFIG_ENABLE_CORTEXA9
        /* take the mpcore out of reset.
         * if calling from the mpcore, do nothing.
         */
        cpu_start();

        /***********************************************************************/
        /* more cpu init */
        /***********************************************************************/
        cpu_init_crit();
#endif

        /* Init UART PortD (115200 8n1)*/
        NvBlUartInitD();

        /* post code 'Yy' */
        PostYy();
}
