/*
 * Copyright (c) 2009, Code Aurora Forum. All rights reserved.
 * 
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
 *
 * (C) Copyright 2002
 * Gary Jennejohn, DENX Software Engineering, <gj@denx.de>
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
 * CPU specific code
 */

#include <common.h>
#include <command.h>
#include <asm-armv7Scorpion/armv7Scorpion.h>


/* SCTLR bits, See also ARM Ref. Man. */
#define C1_MMU		(1<<0)		/* mmu off/on */
#define C1_ALIGN	(1<<1)		/* alignment faults off/on */
#define C1_DC		(1<<2)		/* dcache off/on */
#define C1_WB		(1<<3)		/* merging write buffer on/off */
#define C1_BIG_ENDIAN	(1<<7)	/* big endian off/on */
#define C1_SYS_PROT	(1<<8)		/* system protection */
#define C1_ROM_PROT	(1<<9)		/* ROM protection */
#define C1_Z        (1<<11)     /* Branch prediction off/on */
#define C1_IC		(1<<12)		/* icache off/on */
#define C1_HIGH_VECTORS	(1<<13)	/* location of vectors: low/high addresses */
#define RESERVED_1	(0xf << 3)	/* must be 111b for R/W */

/* Scorpion ACTLR bits, See also Scorpion PRM */
#define ACTLR_MMUDRE  (1<<21)


int cleanup_platform_before_linux(void);
void mmu_init(void);

int arch_cpu_init (void)
{
    uint32_t reg;

    /*
     * Set up MMU page tables
     */
    mmu_init();

    /*
     * Enable I-cache (I), D-cache (C), MMU (M), and branch prediction (Z).
     */
    RCP15_SCTLR(reg);
    reg |= (C1_IC | C1_Z);        // Enable L1 I-cache and Branch prediction
    reg |= C1_MMU;                // Enable MMU
#ifdef CONFIG_DCACHE
    reg |= C1_DC;                 // C bit will enable L1 D and L2 caches
#endif
    WCP15_SCTLR(reg);

	/*
	 * setup up stacks if necessary
	 */
#ifdef CONFIG_USE_IRQ
	DECLARE_GLOBAL_DATA_PTR;

	IRQ_STACK_START = _armboot_start - CFG_MALLOC_LEN - CFG_GBL_DATA_SIZE - 4;
	FIQ_STACK_START = IRQ_STACK_START - CONFIG_STACKSIZE_IRQ;
#endif
	return 0;
}

int cleanup_before_linux (void)
{
	/*
	 * this function is called just before we call linux
	 * it prepares the processor for linux
	 *
	 * we turn off caches etc ...
	 */

	unsigned long i;

	disable_interrupts ();

    /*
     * Call SOC/board specific code to cleanup
     */
    cleanup_platform_before_linux();

    /* 
     * Turn off the MMU disabled remap feature. It is assumed
     * that no memory was copy-back cachable in either L1 or L2.
     * if it was, a flush would be required.
     */
    DSB;
    RCP15_ACTLR(i);
    i &= ~ACTLR_MMUDRE;
    WCP15_ACTLR(i);
    ISB;

    /* turn off I/D-cache, MMU, and branch prediction */
    DSB;
    RCP15_SCTLR(i);
    i &= ~(C1_DC | C1_IC | C1_Z | C1_MMU);
    WCP15_SCTLR(i);
    ISB;

    /* Invalidate all 4 banks of the L2 */
    WCP15_DCIALL(0x0002);
    WCP15_DCIALL(0x4002);
    WCP15_DCIALL(0x8002);
    WCP15_DCIALL(0xC002);

	/* Invalidate L1 D-cache */
	WCP15_DCIALL(0);

    /* Invalidate L1 I-cache */
    WCP15_ICIALLU(0);
    DSB;

    /* Invalidate the TLB */
    WCP15_UTLBIALL(0);
    DSB;

	return (0);
}

int do_reset (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	extern void reset_cpu (ulong addr);

	disable_interrupts ();
	reset_cpu (0);
	/*NOTREACHED*/
	return (0);
}

void icache_enable (void)
{
	ulong reg;

    RCP15_SCTLR(reg);
    WCP15_SCTLR(reg | C1_IC);
    /* Pre-fetch Buffer Flush */
    ISB;
}

void icache_disable (void)
{
	ulong reg;

    RCP15_SCTLR(reg);
	WCP15_SCTLR(reg & ~C1_IC);
    /* Pre-fetch Buffer Flush */
    ISB;
}

int icache_status (void)
{
    ulong reg;

    RCP15_SCTLR(reg);
	return (reg & C1_IC) != 0;
}

void dcache_enable (void)
{
	ulong reg;

    DSB;
    RCP15_SCTLR(reg);
    WCP15_SCTLR(reg | C1_DC);
}
