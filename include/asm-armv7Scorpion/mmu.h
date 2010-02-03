
/*
 * mmu.h 
 * ARMv7 MMU definitions
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

#ifndef __MMU_H__
#define __MMU_H__


#define CONFIG_PAGETABLE_SIZE         4096   /* Page table is 16KB = 4K uint32 */

// ARMv7 first level page table entry field definitions
#define ARM_L1_PT_SECTION             0x00000002          // 1MB section descriptor 

#define ARM_L1_PT_XN                  0x00000010          // eXecute Never

#define ARM_L1_PT_ATTR_SO             0x00000000          // Strongly Ordered attribute
#define ARM_L1_PT_ATTR_DEVICE         0x00000004          // Shared Device attribute
#define ARM_L1_PT_ATTR_WT             0x00000008          // Write Thru cache attribute
#define ARM_L1_PT_ATTR_NONCACHE       0x00001000          // Non-cacheable attribute

#define ARM_L1_PT_AP_NONE             0x00000000          // No Access                                          
#define ARM_L1_PT_AP_PU_RW            0x00000C00          // Read/Write Access
#define ARM_L1_PT_AP_PU_RO            0x00008C00          // Read Only Access

#define ARM_L1_PT_SECT_ADDR_MASK      0xFFF00000          // 1MB
#define ARM_L1_PT_SECT_ADDR_SHFT      20
#define ARM_L1_PT_NUM_ENTRIES         4096


// An array of the following structure specifies the memory map for U-Boot.
// The last entry in the table should have base_addr and size_in_MB equal
// to END_OF_TABLE.

#define END_OF_TABLE    0xFFFFFFFF
typedef struct mem_map_entry 
{
    uint32_t base_addr;
    uint32_t size;
    uint32_t page_tbl_attrib;       // Level 1 page table attributes
    uint32_t reserved;
} mem_map_entry_t;


/* Functions to map and unmap memory regions in the MMU */
void mmu_map_region(uint32_t start_addr, uint32_t size, uint32_t attrib);
void mmu_unmap_region(uint32_t start_addr, uint32_t size);
int  mmu_is_mapped(uint32_t addr);


#endif /*__MMU_H__*/
