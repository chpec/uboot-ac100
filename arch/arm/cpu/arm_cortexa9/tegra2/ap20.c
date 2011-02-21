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

#include <common.h>
#include "ap20.h"

#define NV_ASSERT(p) \
{    if ((p) == 0) { \
         uart_post('C');  \
         uart_post('o');  \
         uart_post('l');  \
         uart_post('d');  \
    };               \
}

#ifdef CONFIG_TEGRA2_LP0
/** Scratch register macros **/

/** NV_SF_NUM - define a new scratch register value.

	@param s scratch register name (APBDEV_PMC_s)
	@param f register field
	@param n defined value for the field
 */
#define NV_SF_NUM(s,f,n) \
	(((n)& NV_FIELD_MASK(APBDEV_PMC_##s##_0_##f##_RANGE)) << \
		NV_FIELD_SHIFT(APBDEV_PMC_##s##_0_##f##_RANGE))


/** NV_FLD_SET_SR_NUM - modify a scratch register field.

	@param s scratch register name (APBDEV_PMC_s)
	@param f register field
	@param n numeric field value
 */
#define NV_FLD_SET_SF_NUM(s,f,n) \
	((s & ~NV_FIELD_SHIFTMASK(APBDEV_PMC_##s##_0_##f##_RANGE))\
		| NV_SF_NUM(s,f,n))


/** NV_SDRF_NUM - define a new scratch register value.

	@param s scratch register name (APBDEV_PMC_s)
	@param d register domain (hardware block)
	@param r register name
	@param f register field
	@param n defined value for the field
 */
#define NV_SDRF_NUM(s,d,r,f,n) \
	(((n)& NV_FIELD_MASK(APBDEV_PMC_##s##_0_##d##_##r##_0_##f##_RANGE)) << \
		NV_FIELD_SHIFT(APBDEV_PMC_##s##_0_##d##_##r##_0_##f##_RANGE))


/** NV_FLD_SET_SDRF_NUM - modify a scratch register field.

	@param s scratch register name (APBDEV_PMC_s)
	@param d register domain (hardware block)
	@param r register name
	@param f register field
	@param n numeric field value
 */
#define NV_FLD_SET_SDRF_NUM(s,d,r,f,n) \
	((s & ~NV_FIELD_SHIFTMASK(APBDEV_PMC_##s##_0_##d##_##r##_0_##f##_RANGE))\
		| NV_SDRF_NUM(s,d,r,f,n))


/** SCRATCH_REGS() - PMC scratch registers (list of SCRATCH_REG() macros).
	SCRATCH_REG(s) - PMC scratch register name:

	@param s Scratch register name (APBDEV_PMC_s)
 */
#define SCRATCH_REGS() \
		SCRATCH_REG(SCRATCH2)   \
		SCRATCH_REG(SCRATCH4)   \
		SCRATCH_REG(SCRATCH24)  \
		/* End-of-List*/

#define SCRATCH_REG(s) static NvU32 s = 0;
SCRATCH_REGS()
#undef SCRATCH_REG

#define REGS() \
		/* CLK_RST Group */ \
		REG(SCRATCH2, CLK_RST_CONTROLLER, OSC_CTRL, XOBP) \
		REG(SCRATCH2, CLK_RST_CONTROLLER, PLLM_BASE, PLLM_DIVM) \
		REG(SCRATCH2, CLK_RST_CONTROLLER, PLLM_BASE, PLLM_DIVN) \
		REG(SCRATCH2, CLK_RST_CONTROLLER, PLLM_BASE, PLLM_DIVP) \
		REG(SCRATCH2, CLK_RST_CONTROLLER, PLLM_MISC, PLLM_CPCON) \
		REG(SCRATCH2, CLK_RST_CONTROLLER, PLLM_MISC, PLLM_LFCON) \
		/**/ \
		/* EMC Group */ \
		REG2(SCRATCH4, EMC, FBIO_SPARE, CFG_FBIO_SPARE_WB0) \
		/* APB_MISC Group */ \
		REG3(SCRATCH2, APB_MISC, GP_XM2CFGAPADCTRL, CFG2TMC_XM2CFGA_PREEMP_EN) \
		REG3(SCRATCH2, APB_MISC, GP_XM2CFGDPADCTRL, CFG2TMC_XM2CFGD_SCHMT_EN) \
		/**/ \
		/* BCT SdramParams Group*/ \
		RAM(SCRATCH2, MEMORY_TYPE, MemoryType) \
		/**/ \
		RAM(SCRATCH4, EMC_CLOCK_DIVIDER, EmcClockDivider) \
		CONSTANT(SCRATCH4, PLLM_STABLE_TIME, ~0) /* Stuff the maximum value */ \
		CONSTANT(SCRATCH4, PLLX_STABLE_TIME, ~0) /* Stuff the maximum value */ \
		/**/ \
		RAM(SCRATCH24, EMC_AUTO_CAL_WAIT, EmcAutoCalWait) \
		RAM(SCRATCH24, EMC_PIN_PROGRAM_WAIT, EmcPinProgramWait) \
		RAM(SCRATCH24, WARMBOOT_WAIT, WarmBootWait)

/*Correct names */
#define APBDEV_PMC_SCRATCH2_0_CLK_RST_CONTROLLER_OSC_CTRL_0_XOBP_RANGE\
	APBDEV_PMC_SCRATCH2_0_CLK_RST_OSC_CTRL_XOBP_RANGE
#define APBDEV_PMC_SCRATCH2_0_CLK_RST_CONTROLLER_PLLM_BASE_0_PLLM_DIVM_RANGE\
	APBDEV_PMC_SCRATCH2_0_CLK_RST_PLLM_BASE_PLLM_DIVM_RANGE
#define APBDEV_PMC_SCRATCH2_0_CLK_RST_CONTROLLER_PLLM_BASE_0_PLLM_DIVN_RANGE\
	APBDEV_PMC_SCRATCH2_0_CLK_RST_PLLM_BASE_PLLM_DIVN_RANGE
#define APBDEV_PMC_SCRATCH2_0_CLK_RST_CONTROLLER_PLLM_BASE_0_PLLM_DIVP_RANGE\
	APBDEV_PMC_SCRATCH2_0_CLK_RST_PLLM_BASE_PLLM_DIVP_RANGE
#define APBDEV_PMC_SCRATCH2_0_CLK_RST_CONTROLLER_PLLM_MISC_0_PLLM_CPCON_RANGE\
	APBDEV_PMC_SCRATCH2_0_CLK_RST_PLLM_MISC_CPCON_RANGE
#define APBDEV_PMC_SCRATCH2_0_CLK_RST_CONTROLLER_PLLM_MISC_0_PLLM_LFCON_RANGE\
	APBDEV_PMC_SCRATCH2_0_CLK_RST_PLLM_MISC_LFCON_RANGE

#define SDRAM_PARAMS_BASE_ADDR (0x40000000 + 0x100 + 0x88)

void NvBlSaveSdramParams(void)
{
	NvU32			reg;		/* Module register contents */
	NvU32			val;		/* Register field contents */
	NvBootSdramParams	sdram_params;

	memcpy (&sdram_params, (char *)SDRAM_PARAMS_BASE_ADDR,
				sizeof(NvBootSdramParams));

	/* REG(s,d,r,f)
	 *   s = destination Scratch register
	 *   d = Device name
	 *   r = Register name
	 *   f = register Field
	 */
	#define REG(s,d,r,f)  \
		reg = NV_CAR_REGR(CLK_RST_PA_BASE, r); \
		val = NV_DRF_VAL(d,r,f,reg); \
		s = NV_FLD_SET_SDRF_NUM(s,d,r,f,val);

	#define REG2(s,d,r,f)  \
		reg = NV_EMC_REGR(EMC_PA_BASE, r); \
		val = NV_DRF_VAL(d,r,f,reg); \
		s = NV_FLD_SET_SDRF_NUM(s,d,r,f,val);

	#define REG3(s,d,r,f)  \
		reg = NV_MISC_REGR(MISC_PA_BASE, r);\
		val = NV_DRF_VAL(d,r,f,reg); \
		s = NV_FLD_SET_SDRF_NUM(s,d,r,f,val);

	/* RAM(s,f,n)
	 *   s = destination Scratch register
	 *   f = register Field
	 *   v = bct Variable
	 */
	#define RAM(s,f,v) \
		s = NV_FLD_SET_SF_NUM(s,f,sdram_params.v);

	/* Define the transformation macro that will stuff a PMC scratch
	 * register with a constant value.
	 */

	/* CONSTANT(s,f,n)
	 *   s = destination Scratch register
	 *   f = register Field
	 *   v = constant Value
	 */
	#define CONSTANT(s,f,v) \
		s = NV_FLD_SET_SF_NUM(s,f,v);

	/*Instantiate all of the register transformations. */
	REGS()
	#undef RAM
	#undef CONSTANT

	/* Generate writes to the PMC scratch registers to copy the local
	 * variables to the actual registers.
	 */
	#define SCRATCH_REG(s)\
		NV_PMC_REGW(PMC_PA_BASE, s, s);
	SCRATCH_REGS()
	#undef SCRATCH_REG
}
#endif

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

    frequency = CONFIG_SYS_CPU_OSC_FREQUENCY;

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

    /* Timing between enabling of CPU_PWR_REQ and output of VDD_CPU,
     * based on PMU data sheet.
     */
    NvBlAvpStallUs(3750);
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

void enableScu(void)
{
    NvU32 reg;

    return;
    reg = NV_SCU_REGR(CONTROL);
    if (NV_DRF_VAL(SCU, CONTROL, SCU_ENABLE, reg) == 1)
    {
        /* SCU already enabled, return */
        return;
    }

    /* Invalidate all ways for all processors */
    NV_SCU_REGW(INVALID_ALL, 0xffff);

    /* Enable SCU - bit 0 */
    reg = NV_SCU_REGR(CONTROL);
    reg |= 0x1;
    NV_SCU_REGW(CONTROL, reg);

    return;
}

void disableScu(void)
{
    NvU32 reg;

    reg = NV_SCU_REGR(CONTROL);
    if (NV_DRF_VAL(SCU, CONTROL, SCU_ENABLE, reg) == 0)
    {
        //Return if scu already disabled
        return;
    }
 
    /* Invalidate all ways for all processors */
    NV_SCU_REGW(INVALID_ALL, 0xffff);

    /* Disable SCU - bit 0 */
    reg = NV_SCU_REGR(CONTROL);
    reg &= ~(NvU32)0x1;
    NV_SCU_REGW(CONTROL, reg);

    return;
}
 
NV_NAKED void NvBlCacheConfigure(void)
{
    asm volatile(
     "stmdb r13!,{r14}                              \n"
    //invalidate instruction cache
    "mov r1, #0                                     \n"
    "mcr p15, 0, r1, c7, c5, 0                      \n"

    //invalidate the i&d tlb entries
    "mcr p15, 0, r1, c8, c5, 0                      \n"
    "mcr p15, 0, r1, c8, c6, 0                      \n"

    //enable instruction cache
    "mrc  p15, 0, r1, c1, c0, 0                     \n"
    "orr  r1, r1, #(1<<12)                          \n"
    "mcr  p15, 0, r1, c1, c0, 0                     \n"

#if 0
    //read in the cpuid register.  If we are on ARMv7 then we have to
    //iterate over the data cache lines, there is no single invalidate for
    //the entire cache
    "mrc p15, 0, r0, c0, c0, 0                      \n"
    //Part number is bits 4 to 15
    "ldr r1, = 0xfff0                               \n"
    //Mask all bits except bits 4 to 15
    "and r0, r0, r1                                 \n"
    //0xc09 is the part number for Cortex A9
    "ldr r1, = (0xC09 << 4)                         \n"
    "cmp r0, r1                                     \n"
    "beq cortexA9                                   \n"

    //invalidate data cache
    "mov r0, #0                                     \n"
    "mcr p15, 0, r0, c7, c6, 0                      \n"// invalidate
    "mov r0, #0                                     \n"
    "mcr p15, 0, r0, c7, c10, 4                     \n"// data sync barrier
    "bl nvaos_L2Invalidate                          \n"
    "mcr p15, 0, r0, c7, c10, 4                     \n"// data sync barrier

    "b invalidate_done                              \n"

"cortexA9:                                          \n"
#endif

#if 0
    // We enable the SCU early only for AP20 because it is
    // required for PCIE. For all other A9-based chips, delay
    // SCU initialization until it is decided that we need it
    // for SMP to save power and reduce latency.
    "ldr r0, =0x70000000                            \n"// ldr r0, =AP15_APB_MISC_BASE
    "ldr r0, [r0, #0x804]                           \n"// ldr r0, [r0, #APB_MISC_GP_HIDREV_0]
    "mov r0, r0, asr #8                             \n"// mov r0, r0, asr #APB_MISC_GP_HIDREV_0_CHIPID_SHIFT
    "and r0, r0, #0xFF                              \n"// and r0, r0, #APB_MISC_GP_HIDREV_0_CHIPID_DEFAULT_MASK
    "cmp r0, #0x20                                  \n"

    //enable SCU - Snoop control unit of Cortex A9
    "bleq enableScu                                 \n"
#else
    "bl enableScu                                   \n"
#endif

    //enable SMP mode and FW for CPU0, by writing to Auxiliary Control
    //Register
    "mrc p15, 0, r0, c1, c0, 1                      \n"
    "orr r0, r0, #0x41                              \n"
    "mcr p15, 0, r0, c1, c0, 1                      \n"

    //Now flush the Dcache
    "mov r0, #0                                     \n"
    "mov r1, #256                                   \n"// 256 cache lines

"invalidate_loop:                                   \n"

    "add r1, r1, #-1                                \n"
    "mov r0, r1, lsl #5                             \n"
    // invalidate d-cache using line (way0)
    "mcr p15, 0, r0, c7, c6, 2                      \n"

    "orr r2, r0, #(1<<30)                           \n"
    // invalidate d-cache using line (way1)
    "mcr p15, 0, r2, c7, c6, 2                      \n"

    "orr r2, r0, #(2<<30)                           \n"
    // invalidate d-cache using line (way2)
    "mcr p15, 0, r2, c7, c6, 2                      \n"

    "orr r2, r0, #(3<<30)                           \n"
    // invalidate d-cache using line (way3)
    "mcr p15, 0, r2, c7, c6, 2                      \n"

    "cmp r1, #0                                     \n"
    "bne invalidate_loop                            \n"

    // FIXME: should have ap20's L2 disabled too
"invalidate_done:                                   \n"
    "ldmia r13!,{pc}                                \n"
    ".ltorg                                         \n"
    );
}

void NvBlInitPmcScratch(void)
{
    volatile NvU8 *pPmc;
    NvU32 i;
   
    pPmc = (volatile NvU8 *)(NV_ADDRESS_MAP_PMC_BASE);

    //  SCRATCH0 is initialized by the boot ROM and shouldn't be cleared
    for (i=APBDEV_PMC_SCRATCH1_0; i<=APBDEV_PMC_SCRATCH23_0; i+=4)
    {
        if (i==APBDEV_PMC_SCRATCH20_0)
            NV_WRITE32(pPmc+i, CONFIG_SYS_BOARD_ODMDATA);
        else
            NV_WRITE32(pPmc+i, 0);
    }

#ifdef CONFIG_TEGRA2_LP0
    // Save Sdram params to PMC 2, 4, and 24 for WB0
    NvBlSaveSdramParams();
#endif
}

NvU32 s_ChipId;
volatile NvU32 s_bFirstBoot = 1;

void cpu_start( void )
{
    NvU32 reg;

    // enable JTAG
    NV_MISC_REGW( MISC_PA_BASE, PP_CONFIG_CTL, 0xc0);

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
    if( s_bFirstBoot ) {
        /* Init Debug UART Port (115200 8n1)*/
        NvBlUartInit();

        /* post code 'Zz' */
        PostZz();

        /* Init PMC scratch memory */
        NvBlInitPmcScratch();
    }

#ifdef CONFIG_ENABLE_CORTEXA9
    /* take the mpcore out of reset.
     * if calling from the mpcore, do nothing.
     */
    cpu_start();

    /***********************************************************************/
    /* more cpu init */
    /***********************************************************************/
    NvBlCacheConfigure();

    /* post code 'Yy' */
    PostYy();
#endif

    /* post code 'Xx' */
    PostXx();
}
