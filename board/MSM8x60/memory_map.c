/*
 * memory_map.c
 * Table that defines U-Boot memory map. It is used to build the MMU
 * page tables.
 *
 * Copyright (c) 2009, Code Aurora Forum. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Code Aurora Forum nor
 *       the names of its contributors may be used to endorse or promote
 *       products derived from this software without specific prior written
 *       permission.
 *
 * Alternatively, provided that this notice is retained in full, this software
 * may be relicensed by the recipient under the terms of the GNU General Public
 * License version 2 ("GPL") and only version 2, in which case the provisions of
 * the GPL apply INSTEAD OF those given above.  If the recipient relicenses the
 * software under the GPL, then the identification text in the MODULE_LICENSE
 * macro must be changed to reflect "GPLv2" instead of "Dual BSD/GPL".  Once a
 * recipient changes the license terms to the GPL, subsequent recipients shall
 * not relicense under alternate licensing terms, including the BSD or dual
 * BSD/GPL terms.  In addition, the following license statement immediately
 * below and between the words START and END shall also then apply when this
 * software is relicensed under the GPL:
 *
 * START
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License version 2 and only version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * END
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <common.h>
#include <asm/arch/memtypes.h>

#define ONE_MB    (1024 * 1024)

/*
 * U-Boot memory map.
 * Memory regions in the following table cannot overlap. All memory regions accessible by
 * U-Boot should be in this table EXCEPT the kernel SDRAM regions which will be added
 * to the page table by dram_reinit().
 */
mem_map_entry_t  memory_map[ ] =
{
    // Memory regions in SMI SDRAM
    { UBOOT_SDRAM_BASE,  UBOOT_SDRAM_SIZE,  MEM_TYPE_UBOOT,        0 },
    { SMEM_START,               SMEM_SIZE,  MEM_TYPE_SMEM,         0 },   // SMEM (shared memory)

#ifdef CONFIG_MSM8X60_LCDC
    { LCDC_FB_ADDR,          LCDC_FB_SIZE,  MEM_TYPE_FRAME_BUFFER, 0 },   // Frame buffer for LCDC
#endif

    // EBI2 attached peripherals
//    { EBI2CS7_BASE,              1*ONE_MB,  MEM_TYPE_PERIPHERAL,   0 },   // Ethernet controller on EBI2 CS7

    // Internal peripherals
    { 0xA0000000,               17*ONE_MB,  MEM_TYPE_PERIPHERAL,   0 },   // Fast Peripheral Bus
    { 0xA8000000,               48*ONE_MB,  MEM_TYPE_PERIPHERAL,   0 },   // Slow Peripheral Bus
    { 0xAC000000,               16*ONE_MB,  MEM_TYPE_PERIPHERAL,   0 },   // Scorpion Processor SubSystem (SPSS)

    // End of Table
    { END_OF_TABLE,          END_OF_TABLE,  END_OF_TABLE,          0 }
};
