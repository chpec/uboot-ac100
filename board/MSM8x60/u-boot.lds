/*
 * Copyright (c) 2009-2010, Code Aurora Forum. All rights reserved.
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

OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(_start)

/* Cannot access any C symbols, macros in linker script, harcode.
 * To save memory from being wasted due to 16K alignement,
 * pagetable size (16KB) is subtracted from smi end (0xE0100000)
 * i.e 0xE0100000 - 0x4000 = 0xE00FC000.
 * if we just used ALIGN(16384) we could waste upto 16383 bytes.
 */

pagetable_base = 0x401FC000;
/* UBOOT uses SDRAM 0x40100000 to 0x40200000, linker must place everything in this */
MEMORY
{
  smi (wx)  : ORIGIN = 0x40100000, LENGTH = 1M
}

SECTIONS
{
	. = 0x40100000;
	. = ALIGN(4);
	.text	:
	{
	  cpu/armv7Scorpion/start.o	(.text)
	  *(.text)
	} >smi
	.rodata : { *(.rodata) } >smi
	. = ALIGN(4);
	.data : { *(.data) } >smi
	. = ALIGN(4);
	.got : { *(.got) } >smi

	__u_boot_cmd_start = .;
	.u_boot_cmd : { *(.u_boot_cmd) } >smi
	__u_boot_cmd_end = .;

	. = ALIGN(4);
	__bss_start = .;
	.bss : { *(.bss) } >smi
        __bss_end = .;

	/*
	 * Following sections are used for compile-time check
         * on the max size of stack and heap.
         * These should just be use for calculation only
	 * and not bloat binary size, use (NOLOAD).
         */
        . = ALIGN(4);
	.stack (NOLOAD) :
	{
 	  cpu/armv7Scorpion/stack_heap.o (.stack)
	} >smi

        . = ALIGN(4);
	.heap (NOLOAD) :
	{
	  cpu/armv7Scorpion/stack_heap.o (.heap)
	} >smi

        __smi_stack_base = .;

        .pagetable pagetable_base (NOLOAD) :
	{
	  cpu/armv7Scorpion/stack_heap.o (.pagetable)
	} >smi

	_end = . ;
}
