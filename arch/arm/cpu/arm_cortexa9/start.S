/*
 * armboot - Startup Code for Tegra2 Cortex CPU-core
 *
 * Copyright (c) 2010  NVIDIA Corporation <www.nvidia.com>
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <config.h>
#include <version.h>

#define MODE_DISABLE_INTR   0xc0
#define MODE_SVC            0x13
#define MODE_SYS            0x1F

#define CP15_I              (1<<12) // instruction cache
#define CP15_Z              (1<<11) // branch prediction
#define CP15_C              (1<<2)  // data cache
#define CP15_M              (1<<0)  // MMU
#define CP15_V              (1<<13) // high vectors

.globl _start
_start: b	reset
	ldr	pc, _undefined_instruction
	ldr	pc, _software_interrupt
	ldr	pc, _prefetch_abort
	ldr	pc, _data_abort
	ldr	pc, _not_used
	ldr	pc, _irq
	ldr	pc, _fiq

_undefined_instruction: .word undefined_instruction
_software_interrupt:	.word software_interrupt
_prefetch_abort:	.word prefetch_abort
_data_abort:		.word data_abort
_not_used:		.word not_used
_irq:			.word irq
_fiq:			.word fiq
_pad:			.word 0x12345678 /* now 16*4=64 */
.global _end_vect
_end_vect:

	.balignl 16,0xdeadbeef
/*************************************************************************
 *
 * Startup Code (reset vector)
 *
 * do important init only if we don't start from memory!
 * setup Memory and board specific bits prior to relocation.
 * relocate armboot to ram
 * setup stack
 *
 *************************************************************************/

_TEXT_BASE:
	.word	TEXT_BASE

.globl _armboot_start
_armboot_start:
	.word _start

/*
 * These are defined in the board-specific linker script.
 */
.globl _bss_start
_bss_start:
	.word __bss_start

.globl _bss_end
_bss_end:
	.word _end

#ifdef CONFIG_USE_IRQ
/* IRQ stack memory (calculated at run-time) */
.globl IRQ_STACK_START
IRQ_STACK_START:
	.word	0x0badc0de

/* IRQ stack memory (calculated at run-time) */
.globl FIQ_STACK_START
FIQ_STACK_START:
	.word 0x0badc0de
#endif

/*
 * the actual reset code
 */

reset:
	/*
	 * set the cpu to SVC32 mode
	 */
	mrs	r0, cpsr
	bic	r0, r0, #MODE_SYS
	orr	r0, r0, #(MODE_SVC | MODE_DISABLE_INTR)
	msr	cpsr,r0

#if (CONFIG_OMAP34XX)
	/* Copy vectors to mask ROM indirect addr */
	adr	r0, _start		@ r0 <- current position of code
	add	r0, r0, #4		@ skip reset vector
	mov	r2, #64			@ r2 <- size to copy
	add	r2, r0, r2		@ r2 <- source end address
	mov	r1, #SRAM_OFFSET0	@ build vect addr
	mov	r3, #SRAM_OFFSET1
	add	r1, r1, r3
	mov	r3, #SRAM_OFFSET2
	add	r1, r1, r3
next:
	ldmia	r0!, {r3 - r10}		@ copy from source address [r0]
	stmia	r1!, {r3 - r10}		@ copy to   target address [r1]
	cmp	r0, r2			@ until source end address [r2]
	bne	next			@ loop until equal */
#if !defined(CONFIG_SYS_NAND_BOOT) && !defined(CONFIG_SYS_ONENAND_BOOT)
	/* No need to copy/exec the clock code - DPLL adjust already done
	 * in NAND/oneNAND Boot.
	 */
	bl	cpy_clk_code		@ put dpll adjust code behind vectors
#endif /* NAND Boot */
#endif
	/* the mask ROM code should have PLL and others stable */
#ifndef CONFIG_TEGRA2
#ifndef CONFIG_SKIP_LOWLEVEL_INIT
	bl	cpu_init_crit
#endif
#endif

#ifndef CONFIG_SKIP_RELOCATE_UBOOT
relocate:				@ relocate U-Boot to RAM
	adr	r0, _start		@ r0 <- current position of code
	ldr	r1, _TEXT_BASE		@ test if we run from flash or RAM
	cmp	r0, r1			@ don't reloc during debug
	beq	stack_setup

	ldr	r2, _armboot_start
	ldr	r3, _bss_start
	sub	r2, r3, r2		@ r2 <- size of armboot
	add	r2, r0, r2		@ r2 <- source end address

copy_loop:				@ copy 32 bytes at a time
	ldmia	r0!, {r3 - r10}		@ copy from source address [r0]
	stmia	r1!, {r3 - r10}		@ copy to   target address [r1]
	cmp	r0, r2			@ until source end addreee [r2]
	ble	copy_loop
#endif	/* CONFIG_SKIP_RELOCATE_UBOOT */

	/* Set up the stack */
stack_setup:
	mov r0, #CONFIG_STACKBASE
	add sp, r0, #CONFIG_STACKSIZE

#ifdef CONFIG_TEGRA2
        /* Tegra2 specific Init */
        bl      tegra2_start   
#endif

	/* Clear BSS (if any). Is below tx (watch load addr - need space) */
clear_bss:
	ldr	r0, _bss_start		@ find start of bss segment
	ldr	r1, _bss_end		@ stop here
	mov	r2, #0x00000000		@ clear value
clbss_l:
	str	r2, [r0]		@ clear BSS location
	cmp	r0, r1			@ are we at the end yet
	add	r0, r0, #4		@ increment clear index pointer
	bne	clbss_l			@ keep clearing till at end

	ldr	pc, _start_armboot	@ jump to C code

_start_armboot: .word start_armboot


/*************************************************************************
 *
 * CPU_init_critical registers
 *
 * setup important registers
 * setup memory timing
 *
 *************************************************************************/
.globl cpu_init_crit
cpu_init_crit:
	mrc  p15, 0, r1, c1, c0, 0 /* Read cp15 control register */
	bic  r1, r1, #CP15_I	/* Disable instruction cache */
	bic  r1, r1, #CP15_Z	/* Disable branch predictor */
	bic  r1, r1, #CP15_C	/* Disable data cache */
	bic  r1, r1, #CP15_M	/* disable the mmu */
	orr  r1, r1, #CP15_V	/* set the 'V' bit, force high exceptions. */
	mcr  p15, 0, r1, c1, c0, 0 /* Write cp15 register */

	/* invalidate instruction cache */
	mov r1, #0
	mcr p15, 0, r1, c7, c5, 0

	/* invalidate the i&d tlb entries */
	mcr p15, 0, r1, c8, c5, 0
	mcr p15, 0, r1, c8, c6, 0

	/* enable instruction cache */
	mrc  p15, 0, r1, c1, c0, 0
	orr  r1, r1, #CP15_I
	mcr  p15, 0, r1, c1, c0, 0

	/* enable SMP mode and FW for CPU0, by writing to Auxiliary Control
	 * Register
	 */
	mrc p15, 0, r0, c1, c0, 1
	orr r0, r0, #0x41
	mcr p15, 0, r0, c1, c0, 1

	/* enable co-processor access (floating point will crash without this) */
	mrc p15, 0, r0, c1, c0, 2
	mov r0, #-1
	mcr p15, 0, r0, c1, c0, 2

	mrc  p15, 0, r1, c1, c0, 0 /* Read cp15 control register */
	orr  r1, r1, #CP15_Z	  /* Enable branch predictor */
	bic  r1, r1, #(1<<22)	  /* DO NOT allow unaligned loads/stores */
	orr  r1, r1, #(1<<1)	   /* Turn on strict alignment checking */
	mcr  p15, 0, r1, c1, c0, 0 /* Write cp15 register */

	/*
	 * Jump to board specific initialization...
	 * The Mask ROM will have already initialized
	 * basic memory. Go here to bump up clock rate and handle
	 * wake up conditions.
	 */
#ifndef CONFIG_TEGRA2
	mov	ip, lr			@ persevere link reg across call
	bl	lowlevel_init		@ go setup pll,mux,memory
	mov	lr, ip			@ restore link
#endif
	mov	pc, lr			@ back to my caller

/*
 *************************************************************************
 *
 * Interrupt handling
 *
 *************************************************************************
 */
@
@ IRQ stack frame.
@
#define S_FRAME_SIZE	72

#define S_OLD_R0	68
#define S_PSR		64
#define S_PC		60
#define S_LR		56
#define S_SP		52

#define S_IP		48
#define S_FP		44
#define S_R10		40
#define S_R9		36
#define S_R8		32
#define S_R7		28
#define S_R6		24
#define S_R5		20
#define S_R4		16
#define S_R3		12
#define S_R2		8
#define S_R1		4
#define S_R0		0

#define MODE_SVC 0x13
#define I_BIT	 0x80

/*
 * use bad_save_user_regs for abort/prefetch/undef/swi ...
 * use irq_save_user_regs / irq_restore_user_regs for IRQ/FIQ handling
 */

	.macro	bad_save_user_regs
	sub	sp, sp, #S_FRAME_SIZE		@ carve out a frame on current
						@ user stack
	stmia	sp, {r0 - r12}			@ Save user registers (now in
						@ svc mode) r0-r12

	ldr	r2, _armboot_start
	sub	r2, r2, #(CONFIG_SYS_MALLOC_LEN)
	sub	r2, r2, #(CONFIG_SYS_GBL_DATA_SIZE + 8)	@ set base 2 words into abort
						@ stack
	ldmia	r2, {r2 - r3}			@ get values for "aborted" pc
						@ and cpsr (into parm regs)
	add	r0, sp, #S_FRAME_SIZE		@ grab pointer to old stack

	add	r5, sp, #S_SP
	mov	r1, lr
	stmia	r5, {r0 - r3}			@ save sp_SVC, lr_SVC, pc, cpsr
	mov	r0, sp				@ save current stack into r0
						@ (param register)
	.endm

	.macro	irq_save_user_regs
	sub	sp, sp, #S_FRAME_SIZE
	stmia	sp, {r0 - r12}			@ Calling r0-r12
	add	r8, sp, #S_PC			@ !! R8 NEEDS to be saved !!
						@ a reserved stack spot would
						@ be good.
	stmdb	r8, {sp, lr}^			@ Calling SP, LR
	str	lr, [r8, #0]			@ Save calling PC
	mrs	r6, spsr
	str	r6, [r8, #4]			@ Save CPSR
	str	r0, [r8, #8]			@ Save OLD_R0
	mov	r0, sp
	.endm

	.macro	irq_restore_user_regs
	ldmia	sp, {r0 - lr}^			@ Calling r0 - lr
	mov	r0, r0
	ldr	lr, [sp, #S_PC]			@ Get PC
	add	sp, sp, #S_FRAME_SIZE
	subs	pc, lr, #4			@ return & move spsr_svc into
						@ cpsr
	.endm

	.macro get_bad_stack
	ldr	r13, _armboot_start		@ setup our mode stack (enter
						@ in banked mode)
	sub	r13, r13, #(CONFIG_SYS_MALLOC_LEN)	@ move past malloc pool
	sub	r13, r13, #(CONFIG_SYS_GBL_DATA_SIZE + 8) @ move to reserved a couple
						@ spots for abort stack

	str	lr, [r13]			@ save caller lr in position 0
						@ of saved stack
	mrs	lr, spsr			@ get the spsr
	str	lr, [r13, #4]			@ save spsr in position 1 of
						@ saved stack

	mov	r13, #MODE_SVC			@ prepare SVC-Mode
	@ msr	spsr_c, r13
	msr	spsr, r13			@ switch modes, make sure
						@ moves will execute
	mov	lr, pc				@ capture return pc
	movs	pc, lr				@ jump to next instruction &
						@ switch modes.
	.endm

	.macro get_bad_stack_swi
	sub	r13, r13, #4			@ space on current stack for
						@ scratch reg.
	str	r0, [r13]			@ save R0's value.
	ldr	r0, _armboot_start		@ get data regions start
	sub	r0, r0, #(CONFIG_SYS_MALLOC_LEN)	@ move past malloc pool
	sub	r0, r0, #(CONFIG_SYS_GBL_DATA_SIZE + 8)	@ move past gbl and a couple
						@ spots for abort stack
	str	lr, [r0]			@ save caller lr in position 0
						@ of saved stack
	mrs	r0, spsr			@ get the spsr
	str	lr, [r0, #4]			@ save spsr in position 1 of
						@ saved stack
	ldr	r0, [r13]			@ restore r0
	add	r13, r13, #4			@ pop stack entry
	.endm

	.macro get_irq_stack			@ setup IRQ stack
	ldr	sp, IRQ_STACK_START
	.endm

	.macro get_fiq_stack			@ setup FIQ stack
	ldr	sp, FIQ_STACK_START
	.endm

/*
 * exception handlers
 */
	.align	5
undefined_instruction:
	get_bad_stack
	bad_save_user_regs
	bl	do_undefined_instruction

	.align	5
software_interrupt:
	get_bad_stack_swi
	bad_save_user_regs
	bl	do_software_interrupt

	.align	5
prefetch_abort:
	get_bad_stack
	bad_save_user_regs
	bl	do_prefetch_abort

	.align	5
data_abort:
	get_bad_stack
	bad_save_user_regs
	bl	do_data_abort

	.align	5
not_used:
	get_bad_stack
	bad_save_user_regs
	bl	do_not_used

#ifdef CONFIG_USE_IRQ

	.align	5
irq:
	get_irq_stack
	irq_save_user_regs
	bl	do_irq
	irq_restore_user_regs

	.align	5
fiq:
	get_fiq_stack
	/* someone ought to write a more effective fiq_save_user_regs */
	irq_save_user_regs
	bl	do_fiq
	irq_restore_user_regs

#else

	.align	5
irq:
	get_bad_stack
	bad_save_user_regs
	bl	do_irq

	.align	5
fiq:
	get_bad_stack
	bad_save_user_regs
	bl	do_fiq

#endif

	.align 5
.global prepare_jump_to_kernel
prepare_jump_to_kernel:

	stmfd	r13!, {r0 - r5, r7, r9 - r12, r14}

	// disable the MMU and L1-caches
	mov r4, #0
	mcr p15, 0, r4, c7, c10, 4
	mrc p15, 0, r4, c1, c0, 0
	bic r4, r4, #(1<<2)
	bic r4, r4, #(1<<11)
	bic r4, r4, #(1<<12)
	bic r4, r4, #(1<<0)
	bic r4, r4, #(1<<29)
	bic r4, r4, #(1<<23)
	mcr p15, 0, r4, c1, c0, 0

	// disable the SMP bit, if it was ever enabled
	mrc p15, 0, r4, c1, c0, 1
	bic r4, r4, #(1<<6)
	bic r4, r4, #(1<<1)
	bic r4, r4, #(1<<2)
	mcr p15, 0, r4, c1, c0, 1

	mov r4, #0
	mcr p15, 0, r4, c7, c10, 4

	// invalidate the instruction and data caches, just to be on the
	// extra-safe side
	mcr p15, 0, r4, c7, c5, 0   // instruction cache invalidate
	mcr p15, 0, r4, c7, c5, 4   // flush prefetch buffer

	mcr p15, 0, r4, c8, c7, 0   // invalidate TLB
	mcr p15, 0, r4, c7, c10, 4
	mcr p15, 0, r11, c7, c10, 4

	ldmfd	r13!, {r0 - r5, r7, r9 - r12, pc}

.global invalidate_dcache
invalidate_dcache:
	mov pc, lr


	.align	5
.global reset_cpu
reset_cpu:
	ldr	r1, rstctl			@ get addr for global reset
						@ reg
	ldr	r3, [r1]
	orr	r3, r3, #0x10
	str	r3, [r1]			@ force reset
	mov	r0, r0
_loop_forever:
	b	_loop_forever
rstctl:
	.word	PRM_RSTCTRL

