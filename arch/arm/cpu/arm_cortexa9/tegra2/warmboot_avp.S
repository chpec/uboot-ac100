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

#include "asm/arch/nvbl_arm_cpsr.h"
#include "asm/arch/nvbl_memmap_nvap.h"
#include "asm/arch/tegra2.h"
#include "asm/arch/nv_drf_asm.h"

/* -----------------------------------
 * Compile-time code options
 * -----------------------------------
 */

/* CPU Clock Source -- DO NOT CHOOSE PLL-X */
#define CPU_CLOCK_SOURCE	CLK_RST_CONTROLLER_CCLK_BURST_POLICY_0_CWAKEUP_IDLE_SOURCE_PLLP_OUT0

/* Scratch map */
#define APBDEV_PMC_SCRATCH_FOR_LP_EXIT_TIME_0	APBDEV_PMC_SCRATCH1_0

/*Misc warmboot parameters */
#define AP20_WB0_RUN_ADDRESS	0x40020000

/*Apertures bases */
#define CSITE_PA_BASE		0x70040000	/* Base address for arcsite.h registers */
#define CLK_RST_PA_BASE		0x60006000	/* Base address for arclk_rst.h registers */
#define EVP_PA_BASE		0x6000F000	/* Base address for arevp.h registers */
#define FLOW_PA_BASE		0x60007000	/* Base address for arflow_ctlr.h registers */
#define PMC_PA_BASE		0x7000E400	/* Base address for arapbpm.h registers */
#define TIMERUS_PA_BASE		0x60005010	/* Base address for artimerus.h registers */
#define PG_UP_PA_BASE		0x60000000	/* Base address for arpg.h registers */
#define MISC_PA_BASE		0x70000000	/* Base address for arapb_misc.h registers */

/* -----------------------------------
 * Compile-time debug code enables
 * -----------------------------------
 */

#define DEBUG_DO_NOT_RESET_CORESIGHT	0	/* Set non-zero to skip resetting CoreSight */

	.section	.text
	.align		4

/* ------------------------------------------------------
 * Prototype:
 *   void wb_start(void)
 *
 * Input:
 *
 * Output:
 *   None
 *
 * Registers Used:
 *   ALL
 *
 * Description:
 *   This function restarts the CPU and then waits for the AVP driver to
 *   tell it where to transfer control to finish restoration of the AVP state.
 * -------------------------------------------------------------------------------
 */

	.globl	wb_start
wb_start:

	.word	0, 0, 0, 0
	.word	0, 0, 0, 0
	.word	0, 0, 0, 0
	.word	0, 0, 0, 0

start:

	ldr	r0, =MISC_PA_BASE		/* R0 = MISC PA base address */
	ldr	r1, =NV_DRF_DEF(APB_MISC_PP, CONFIG_CTL, JTAG, ENABLE) \
		_OR_ NV_DRF_DEF(APB_MISC_PP, CONFIG_CTL, TBE, ENABLE)
	str	r1, [r0, #NV_DRF_OFFSET(APB_MISC_PP, CONFIG_CTL)]

	/* -----------------------------------------------------------------
	 * Load up the base addresses for the register blocks.
	 * -----------------------------------------------------------------
	 */
	ldr	r5, =PMC_PA_BASE		/* R5 = PMC PA base address */
	ldr	r6, =FLOW_PA_BASE		/* R6 = FLOW PA base address */
	ldr	r7, =TIMERUS_PA_BASE		/* R7 = TIMERUS PA base address */
	ldr	r8, =CLK_RST_PA_BASE		/* R8 = CLK PA base address */
	ldr	r9, =EVP_PA_BASE		/* R9 = EVP PA base address */
	ldr	r10,=CSITE_PA_BASE		/* R10 = CSITE base address */

	/* -----------------------------------------------------------------
	 * Are we running where we're supposed to be?
	 * -----------------------------------------------------------------
	 */

	ldr	r0, =AP20_WB0_RUN_ADDRESS	/* R0 = expected load address */
	add	r1, pc, #here-(.+8)		/* R1 = &here */

here:
	sub	r1, r1, #(here-start)		/* R1 = actual load address */
	ldr	r2, =PG_UP_PA_BASE		/* R2 = PG PA base address */
	ldr	r3, [r2, #PG_UP_TAG_0]		/* R3 = processor tag */
	ldr	r2, =PG_UP_TAG_0_PID_COP	/* R2 = AVP processor tag */
	cmp	r0, r1				/* Addresses match? */
	cmpeq	r2, r3				/* Processor tags match? */
	bne	do_reset			/* No -- reset the chip */

	/* -----------------------------------------------------------------
	 * Get a snapshot of the Usec count. This is a good indicator of
	 * the overhead of BOOTROM after a wake-up event.
	 * -----------------------------------------------------------------
	 */

	ldr	r11, [r7, #TIMERUS_CNTR_1US_0]

	/* ==================================================================
	 * BEGIN CPU COMPLEX INITIALIZATON
	 * ==================================================================
	 */

#if	!DEBUG_DO_NOT_RESET_CORESIGHT

	/* ----------------------------------------------------------------
	 * Assert CoreSight reset.
	 * ----------------------------------------------------------------
	 */

	mov	r1, #NV_DRF_MASK(CLK_RST_CONTROLLER,RST_DEV_U_SET,SET_CSITE_RST)
	str	r1, [r8, #CLK_RST_CONTROLLER_RST_DEV_U_SET_0]

#endif /*!DEBUG_DO_NOT_RESET_CORESIGHT */

	/* Set the drive strength */
	ldr	r1, [r8, #CLK_RST_CONTROLLER_OSC_CTRL_0]
	ldr	r3, =NV_DRF_MASK(CLK_RST_CONTROLLER, OSC_CTRL, XOFS) \
		_OR_ NV_DRF_MASK(CLK_RST_CONTROLLER, OSC_CTRL, XOE)
	bic 	r1, r1, r3
	ldr	r3, =NV_DRF_NUM(CLK_RST_CONTROLLER, OSC_CTRL, XOFS, 0x4) \
		_OR_ NV_DRF_NUM(CLK_RST_CONTROLLER, OSC_CTRL, XOE, 0x1)
	orr 	r3, r1, r3
	str	r3, [r8, #CLK_RST_CONTROLLER_OSC_CTRL_0]

	/* -----------------------------------------------------------------
	 * Power up the CPU complex if necessary.
	 * -----------------------------------------------------------------
	 */

	ldr	r3, [r5, #APBDEV_PMC_PWRGATE_STATUS_0]
	tst	r3, #NV_DRF_MASK(APBDEV_PMC, PWRGATE_STATUS, CPU)

	ldreq	r2, =NV_DRF_DEF(APBDEV_PMC, PWRGATE_TOGGLE, PARTID, CP) \
		_OR_ NV_DRF_DEF(APBDEV_PMC, PWRGATE_TOGGLE, START, ENABLE)
	streq	r2, [r5, #APBDEV_PMC_PWRGATE_TOGGLE_0]

is_cpu_on:
	ldr	r3, [r5, #APBDEV_PMC_PWRGATE_STATUS_0]
	tst	r3, #NV_DRF_MASK(APBDEV_PMC, PWRGATE_STATUS, CPU)
	beq	is_cpu_on

	/* -----------------------------------------------------------------
	 * Remove the I/O clamps from the CPU power partition.
	 * -----------------------------------------------------------------
	 */

	mov	r3, #NV_DRF_DEF(APBDEV_PMC, REMOVE_CLAMPING_CMD, CPU, ENABLE)
	str	r3, [r5, #APBDEV_PMC_REMOVE_CLAMPING_CMD_0]

	ldr	r3, =NV_DRF_NUM(FLOW_CTLR, HALT_COP_EVENTS, ZERO, 0x14) \
		_OR_ NV_DRF_NUM(FLOW_CTLR, HALT_COP_EVENTS, MSEC, 1) \
		_OR_ NV_DRF_NUM(FLOW_CTLR, HALT_COP_EVENTS, MODE, 2)
	str	r3, [r6, #FLOW_CTLR_HALT_COP_EVENTS_0]

	/* ------------------------------------------------------------------
	 * Assert CPU complex reset.
	 * ------------------------------------------------------------------
	 */

	mov	r1, #NV_DRF_MASK(CLK_RST_CONTROLLER,RST_DEV_L_SET,SET_CPU_RST)
	str	r1, [r8, #CLK_RST_CONTROLLER_RST_DEV_L_SET_0]

	/* ------------------------------------------------------------------
	 * Hold both CPUs in reset.
	 * ------------------------------------------------------------------
	 */

	ldr	r3, =NV_DRF_NUM(CLK_RST_CONTROLLER, RST_CPU_CMPLX_SET, SET_CPURESET1, 1) \
		_OR_ NV_DRF_NUM(CLK_RST_CONTROLLER, RST_CPU_CMPLX_SET, SET_DBGRESET1, 1) \
		_OR_ NV_DRF_NUM(CLK_RST_CONTROLLER, RST_CPU_CMPLX_SET, SET_DERESET1,  1) \
		_OR_ NV_DRF_NUM(CLK_RST_CONTROLLER, RST_CPU_CMPLX_SET, SET_CPURESET0, 1) \
		_OR_ NV_DRF_NUM(CLK_RST_CONTROLLER, RST_CPU_CMPLX_SET, SET_DBGRESET0, 1) \
		_OR_ NV_DRF_NUM(CLK_RST_CONTROLLER, RST_CPU_CMPLX_SET, SET_DERESET0,  1)
	str	r3, [r8, #CLK_RST_CONTROLLER_RST_CPU_CMPLX_SET_0]

	/* ------------------------------------------------------------------
	 * Halt CPU1 at the flow controller for uni-processor configurations.
	 * ------------------------------------------------------------------
	 */

	mov	r3, #NV_DRF_DEF(FLOW_CTLR, HALT_CPU1_EVENTS, MODE, FLOW_MODE_STOP)
	str	r3, [r6, #FLOW_CTLR_HALT_CPU1_EVENTS_0]

	/* -----------------------------------------------------------------
	 * Set the CPU reset vector. SCRATCH41 contains the physical
	 * address of the CPU-side restoration code.
	 * -----------------------------------------------------------------
	 */

	ldr	r3, [r5, #APBDEV_PMC_SCRATCH41_0]
	str	r3, [r9, #EVP_CPU_RESET_VECTOR_0]

	/* ------------------------------------------------------------------
	 * Select CPU complex clock source.
	 * ------------------------------------------------------------------
	 */

	ldr	r3, =(CPU_CLOCK_SOURCE _SHL_ NV_DRF_SHIFT(CLK_RST_CONTROLLER, CCLK_BURST_POLICY, CWAKEUP_FIQ_SOURCE))  \
		_OR_ (CPU_CLOCK_SOURCE _SHL_ NV_DRF_SHIFT(CLK_RST_CONTROLLER, CCLK_BURST_POLICY, CWAKEUP_IRQ_SOURCE))  \
		_OR_ (CPU_CLOCK_SOURCE _SHL_ NV_DRF_SHIFT(CLK_RST_CONTROLLER, CCLK_BURST_POLICY, CWAKEUP_RUN_SOURCE))  \
		_OR_ (CPU_CLOCK_SOURCE _SHL_ NV_DRF_SHIFT(CLK_RST_CONTROLLER, CCLK_BURST_POLICY, CWAKEUP_IDLE_SOURCE)) \
		_OR_ NV_DRF_DEF(CLK_RST_CONTROLLER, CCLK_BURST_POLICY, CPU_STATE, RUN)
	str	r3, [r8, #CLK_RST_CONTROLLER_CCLK_BURST_POLICY_0]

	/* ------------------------------------------------------------------
	 * Start the CPU0 clock and stop the CPU1 clock.
	 * ------------------------------------------------------------------
	 */

	ldr	r3, =NV_DRF_DEF(CLK_RST_CONTROLLER, CLK_CPU_CMPLX, CPU_BRIDGE_CLKDIV, DEFAULT) \
		_OR_ NV_DRF_NUM(CLK_RST_CONTROLLER, CLK_CPU_CMPLX, CPU0_CLK_STP, 0) \
		_OR_ NV_DRF_NUM(CLK_RST_CONTROLLER, CLK_CPU_CMPLX, CPU1_CLK_STP, 1)
	str	r3, [r8, #CLK_RST_CONTROLLER_CLK_CPU_CMPLX_0]

	/* ------------------------------------------------------------------
	 * Enable the CPU complex clock.
	 * ------------------------------------------------------------------
	 */

	mov	r3, #NV_DRF_MASK(CLK_RST_CONTROLLER,CLK_ENB_L_SET,SET_CLK_ENB_CPU)
	str	r3, [r8, #CLK_RST_CONTROLLER_CLK_ENB_L_SET_0]

	/* -----------------------------------------------------------------
	 * Make sure the resets were held for at least 2 microseconds.
	 * -----------------------------------------------------------------
	 */

	add	r3, r11, #2

wait:
	ldr	r2, [r7, #TIMERUS_CNTR_1US_0]
	cmp	r2, r3
	ble	wait

#if	!DEBUG_DO_NOT_RESET_CORESIGHT

	/* -----------------------------------------------------------------
	 * De-assert CoreSight reset.
	 * NOTE: We're leaving the CoreSight clock on the oscillator for
	 *	now. It will be restored to its original clock source
	 *	when the CPU-side restoration code runs.
	 * -----------------------------------------------------------------
	 */

	mov	r1, #NV_DRF_MASK(CLK_RST_CONTROLLER,RST_DEV_U_CLR,CLR_CSITE_RST)
	str	r1, [r8, #CLK_RST_CONTROLLER_RST_DEV_U_CLR_0]

#endif /*!DEBUG_DO_NOT_RESET_CORESIGHT */

	ldr	r1, =0xC5ACCE55			/* R0 = CoreSight unlock value */
	ldr	r2, =CSITE_CPUDBG0_LAR_0	/* R1 = CPU0 lock offset */
	ldr	r3, =CSITE_CPUDBG1_LAR_0	/* R2 = CPU1 lock offset */
	str	r1, [r10, r2]			/* Unlock CPU0 */
	str	r1, [r10, r3]			/* Unlock CPU1 */

	/* -----------------------------------------------------------------
	 * Sample the microsecond timestamp again. This is the time we must
	 * use when returning from LP0 for PLL stabilization delays.
	 * ----------------------------------------------------------------
	 */

	ldr	r11, [r7, #TIMERUS_CNTR_1US_0]
	str	r11, [r5, #APBDEV_PMC_SCRATCH_FOR_LP_EXIT_TIME_0]

	/* -----------------------------------------------------------------
	 * Get the oscillator frequency. For 19.2 MHz, just use 19 to
	 * make the calculations easier.
	 * -----------------------------------------------------------------
	 */

	ldr	r4, [r7, #TIMERUS_USEC_CFG_0]
	and	r4, r4, #NV_DRF_MASK(TIMERUS, USEC_CFG, USEC_DIVISOR)
	add	r4, r4, #1
	cmp	r4, #26
	MOVGT	r4, #19

	/* PLLX_BASE.PLLX_DIVM */
	ldr	r0, [r5, #APBDEV_PMC_SCRATCH3_0]
	and	r2, r0, #APBDEV_PMC_SCRATCH3_0_CLK_RST_PLLX_BASE_PLLX_DIVM_DEFAULT_MASK
	cmp	r2, r4
	moveq	r4, #0
	movne	r4, #1

	/* PLLX_BASE.PLLX_DIVN */
	mov	r0, r0, ASR #(APBDEV_PMC_SCRATCH3_0_CLK_RST_PLLX_BASE_PLLX_DIVN_SHIFT - APBDEV_PMC_SCRATCH3_0_CLK_RST_PLLX_BASE_PLLX_DIVM_SHIFT)
	ldr	r3, =APBDEV_PMC_SCRATCH3_0_CLK_RST_PLLX_BASE_PLLX_DIVN_DEFAULT_MASK
	and	r1, r0, r3
	orr	r2, r2, r1, LSL #CLK_RST_CONTROLLER_PLLX_BASE_0_PLLX_DIVN_SHIFT
	mov	r4, r1, LSL r4

	/* PLLX_BASE.PLLX_DIVP */
	mov	r0, r0, ASR #(APBDEV_PMC_SCRATCH3_0_CLK_RST_PLLX_BASE_PLLX_DIVP_SHIFT - APBDEV_PMC_SCRATCH3_0_CLK_RST_PLLX_BASE_PLLX_DIVN_SHIFT)
	and	r1, r0, #APBDEV_PMC_SCRATCH3_0_CLK_RST_PLLX_BASE_PLLX_DIVP_DEFAULT_MASK
	orr	r2, r2, r1, LSL #CLK_RST_CONTROLLER_PLLX_BASE_0_PLLX_DIVP_SHIFT
	mov	r4, r4, ASR r1

	/* PLLX_BASE.PLLX_BYPASS_ENABLE | PLLX_BASE.PLLX_ENABLE_DISABLE | PLLX_BASE.PLLX_REF_DIS_REF_ENABLE */
	orr	r2, r2, #NV_DRF_DEF(CLK_RST_CONTROLLER, PLLX_BASE, PLLX_BYPASS, ENABLE) \
		_OR_ NV_DRF_DEF(CLK_RST_CONTROLLER, PLLX_BASE, PLLX_ENABLE, DISABLE) \
		_OR_ NV_DRF_DEF(CLK_RST_CONTROLLER, PLLX_BASE, PLLX_REF_DIS, REF_ENABLE)

	/* PLLX_MISC_DCCON must be set for frequencies > 600 MHz. */
	cmp	r4, #600
	movlt	r3, #0
	movge	r3, #NV_DRF_DEF(CLK_RST_CONTROLLER,PLLX_MISC,PLLX_DCCON,DEFAULT)

	/* PLLX_MISC_LFCON */
	mov	r0, r0, ASR #(APBDEV_PMC_SCRATCH3_0_CLK_RST_PLLX_MISC_LFCON_SHIFT - APBDEV_PMC_SCRATCH3_0_CLK_RST_PLLX_BASE_PLLX_DIVP_SHIFT)
	and	r1, r0, #APBDEV_PMC_SCRATCH3_0_CLK_RST_PLLX_MISC_LFCON_DEFAULT_MASK
	orr	r3, r3, r1, LSL #CLK_RST_CONTROLLER_PLLX_MISC_0_PLLX_LFCON_SHIFT

	/* PLLX_MISC_CPCON */
	mov	r0, r0, ASR #(APBDEV_PMC_SCRATCH3_0_CLK_RST_PLLX_MISC_CPCON_SHIFT - APBDEV_PMC_SCRATCH3_0_CLK_RST_PLLX_MISC_LFCON_SHIFT)
	and	r1, r0, #APBDEV_PMC_SCRATCH3_0_CLK_RST_PLLX_MISC_LFCON_DEFAULT_MASK
	orr	r3, r3, r1, LSL #CLK_RST_CONTROLLER_PLLX_MISC_0_PLLX_CPCON_SHIFT

	str	r3, [r8, #CLK_RST_CONTROLLER_PLLX_MISC_0]
	str	r2, [r8, #CLK_RST_CONTROLLER_PLLX_BASE_0]
	orr	r2, r2, #NV_DRF_DEF(CLK_RST_CONTROLLER, PLLX_BASE, PLLX_ENABLE, ENABLE)
	str	r2, [r8, #CLK_RST_CONTROLLER_PLLX_BASE_0]
	BIC	r2, r2, #NV_DRF_DEF(CLK_RST_CONTROLLER, PLLX_BASE, PLLX_BYPASS, ENABLE)
	str	r2, [r8, #CLK_RST_CONTROLLER_PLLX_BASE_0]

	mov	r3, #0
	str	r3, [r6, #FLOW_CTLR_HALT_CPU_EVENTS_0]

	ldr	r3, =NV_DRF_NUM(CLK_RST_CONTROLLER, RST_CPU_CMPLX_CLR, CLR_CPURESET0, 1) \
		_OR_ NV_DRF_NUM(CLK_RST_CONTROLLER, RST_CPU_CMPLX_CLR, CLR_DBGRESET0, 1) \
		_OR_ NV_DRF_NUM(CLK_RST_CONTROLLER, RST_CPU_CMPLX_CLR, CLR_DERESET0,  1)
	str	r3, [r8, #CLK_RST_CONTROLLER_RST_CPU_CMPLX_CLR_0]

	ldr	r1, = NV_DRF_DEF(CLK_RST_CONTROLLER, PLLM_OUT, PLLM_OUT1_RSTN, RESET_DISABLE) \
		_OR_ NV_DRF_DEF(CLK_RST_CONTROLLER, PLLM_OUT, PLLM_OUT1_CLKEN, ENABLE) \
		_OR_ NV_DRF_NUM(CLK_RST_CONTROLLER, PLLM_OUT, PLLM_OUT1_RATIO, 0x8)
	str	r1, [r8, #CLK_RST_CONTROLLER_PLLM_OUT_0]

	ldr	r2, =NV_DRF_DEF(CLK_RST_CONTROLLER, SCLK_BURST_POLICY, SWAKEUP_FIQ_SOURCE, PLLM_OUT1) \
		_OR_ NV_DRF_DEF(CLK_RST_CONTROLLER, SCLK_BURST_POLICY, SWAKEUP_IRQ_SOURCE, PLLM_OUT1) \
		_OR_ NV_DRF_DEF(CLK_RST_CONTROLLER, SCLK_BURST_POLICY, SWAKEUP_RUN_SOURCE, PLLM_OUT1) \
		_OR_ NV_DRF_DEF(CLK_RST_CONTROLLER, SCLK_BURST_POLICY, SWAKEUP_IDLE_SOURCE, PLLM_OUT1) \
		_OR_ NV_DRF_DEF(CLK_RST_CONTROLLER, SCLK_BURST_POLICY, SYS_STATE, IDLE)
	str	r2, [r8, #CLK_RST_CONTROLLER_SCLK_BURST_POLICY_0]
	b	avp_resume

	.ltorg
	.align	4

avp_resume:

	mov	r1, #NV_DRF_MASK(CLK_RST_CONTROLLER,RST_DEV_L_CLR,CLR_CPU_RST)
	str	r1, [r8, #CLK_RST_CONTROLLER_RST_DEV_L_CLR_0]

avp_halt:

	mov	r3, #NV_DRF_DEF(FLOW_CTLR, HALT_COP_EVENTS, MODE, FLOW_MODE_STOP)
	orr	r3, r3, #NV_DRF_NUM(FLOW_CTLR, HALT_COP_EVENTS, JTAG, 1)
	str	r3, [r6, #FLOW_CTLR_HALT_COP_EVENTS_0]
	b	avp_halt

/* -------------------------------------------------------------------------------
 * Prototype:
 *   do_reset
 *
 * Input:
 *   None
 *
 * Output:
 *   None
 *
 * Registers Used:
 *   All
 *
 * Description:
 *   Execution comes here it something goes wrong. The chip is reset and a
 *   cold boot is performed.
 * -------------------------------------------------------------------------------
 */

do_reset:

	mov	r0, #NV_DRF_DEF(CLK_RST_CONTROLLER, RST_DEVICES_L, SWR_TRIG_SYS_RST, ENABLE)
	str	r0, [r8, #CLK_RST_CONTROLLER_RST_DEVICES_L_0]
	b	.

	.ltorg

	.globl		wb_end
wb_end:

	.end

