/*
 *  cache_mmu.c - Functions to initialize cache and MMU related 
 *                resources
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
 */

#include <common.h>
#include <asm-armv7Scorpion/armv7Scorpion.h>
#include <asm-armv7Scorpion/mmu.h>

#define PVR0F0_MASK    0xFF00FC00      // bit fields for L1 I-cache timings
#define PVR2F0_MASK    0xFC000000      // bit fields for L1 D-cache timings
#define L2VR3F1_MASK   0x00FFFFFF      // bit fields for L2 cache timings

// L1 I-cache timing values for register PVR0F0
uint32_t L1_I_PVR0F0_val[8] =
{
   0x3800FC00,   // for TCSR_SPARE2[8,2,0] = 0, 0, 0
   0x3800FC00,   // for TCSR_SPARE2[8,2,0] = 0, 0, 1
   0x38007C00,   // for TCSR_SPARE2[8,2,0] = 0, 1, 0
   0x3800FC00,   // for TCSR_SPARE2[8,2,0] = 0, 1, 1
   0x38003C00,   // for TCSR_SPARE2[8,2,0] = 1, 0, 0
   0x38000400,   // for TCSR_SPARE2[8,2,0] = 1, 0, 1
   0x38000C00,   // for TCSR_SPARE2[8,2,0] = 1, 1, 0
   0x38001C00    // for TCSR_SPARE2[8,2,0] = 1, 1, 1
};

// L1 D-cache timing values for register PVR2F0
uint32_t L1_D_PVR2F0_val[8] =
{
   0xFC000000,   // for TCSR_SPARE2[8,2,0] = 0, 0, 0
   0xFC000000,   // for TCSR_SPARE2[8,2,0] = 0, 0, 1
   0x7C000000,   // for TCSR_SPARE2[8,2,0] = 0, 1, 0
   0xFC000000,   // for TCSR_SPARE2[8,2,0] = 0, 1, 1
   0x3C000000,   // for TCSR_SPARE2[8,2,0] = 1, 0, 0
   0x04000000,   // for TCSR_SPARE2[8,2,0] = 1, 0, 1
   0x0C000000,   // for TCSR_SPARE2[8,2,0] = 1, 1, 0
   0x1C000000    // for TCSR_SPARE2[8,2,0] = 1, 1, 1
};

// L2 timing values for register L2VR3F1
uint32_t L2_L2VR3F1_val[4] =
{
   0x00010102,   // for TCSR_SPARE2[3,1] = 0, 0
   0x00010102,   // for TCSR_SPARE2[3,1] = 0, 1
   0x00010101,   // for TCSR_SPARE2[3,1] = 1, 0
   0x00212102    // for TCSR_SPARE2[3,1] = 1, 1
};

extern mem_map_entry_t  memory_map[ ];
extern volatile uint32_t _pagetable[CONFIG_PAGETABLE_SIZE];

/*
 * set_cache_timing_parameters - function to set L1 I, L1 D, and L2 cache
 * timing parameters before enabling any of the caches.
 */
void set_cache_timing_parameters(void)
{
    static uint32_t spare2;
    static uint32_t L1_index;
    static uint32_t L2_index;
    static uint32_t reg;

    // NOTE: This function is called before the stack is set up. So all
    // variables above are defined as static.

    // Fuse values set during chip test are read by the ARM9 at boot and placed
    // in the TCSR_SPARE2 register.  The value of TCSR_SPARE2 determines what
    // values to should be written to the cache timing registers.
    // by the ARM9.
    spare2 = IO_READ32(TCSR_SPARE2);

    // The TCSR_SPARE2 value is valid if bits 15:12 (signature) are 0xA
    if ((spare2 & 0x0000F000) != 0x0000A000)
    {
        // Invalid TCSR_SPARE2 value. Nominal cache timing settings used.
        // Can't display an error message as console is not up yet.
        while(1); 
        
    }

    // TCSR_SPARE2 bits are interpreted as follows:
    //    15:12  Signature, should be equal to 0xA
    //        8  SCORPION_L1_ACC[2]
    //      7:4  Speed Bin 
    //        3  SCORPION_L2_ACC[1]
    //        2  SCORPION_L1_ACC[1]
    //        1  SCORPION_L2_ACC[0]
    //        0  SCORPION_L1_ACC[0]

    // Determine the index into the L1 table
    L1_index = (spare2 & 0x00000001) +
              ((spare2 & 0x00000004) >> 1) +
              ((spare2 & 0x00000100) >> 6);
 
    // Determine the index into the L2 table
    L2_index = ((spare2 & 0x00000002) >> 1) + 
               ((spare2 & 0x00000008) >> 2);

    // Set L1 I cache timings
    RCP15_PVR0F0(reg);
    reg &= (~PVR0F0_MASK);
    WCP15_PVR0F0(reg | L1_I_PVR0F0_val[L1_index]);

    // Set L1 D cache timings
    RCP15_PVR2F0(reg);
    reg &= (~PVR2F0_MASK);
    WCP15_PVR2F0(reg | L1_D_PVR2F0_val[L1_index]);

    // Set L2 cache timings
    RCP15_L2VR3F1(reg);
    reg &= (~L2VR3F1_MASK);
    WCP15_L2VR3F1(reg | L2_L2VR3F1_val[L2_index]);

    return;
}

void mmu_init(void)
{
    mem_map_entry_t * mem_map_ptr = &memory_map[0];


    /* Initialize the entire page table to zero */
    memset((void *)_pagetable, 0x00, ARM_L1_PT_NUM_ENTRIES * sizeof(uint32_t));    
    
    /* Use memory map table to add valid first level page table entries */
    while (mem_map_ptr->base_addr != END_OF_TABLE)
    {
        mmu_map_region(mem_map_ptr->base_addr,
                       mem_map_ptr->size,
                       mem_map_ptr->page_tbl_attrib);

        /* Point to the next entry in the memory map table */
        mem_map_ptr++;
    }

    /* 
     * Set the value of TTBR0 to beginning of page table.
     * It is assumed to be 16KB aligned.
     */
    WCP15_TTBR0(_pagetable);
    WCP15_TTBCR(0);
    WCP15_DACR(0xFFFFFFFF);

}

/*
 * Maps requested region in the MMU page table
 */
void mmu_map_region(uint32_t start_addr, uint32_t size, uint32_t attrib)
{
    uint32_t size_in_MB;
    int i;
    uint32_t addr;
    int index;

    if (size == 0)
    {
        /* Nothing to do */
        return;
    }

    size_in_MB = size / (1024 * 1024);
    if (size % (1024 * 1024) != 0)
    {
        size_in_MB++;
    }

    for (i=0; i<size_in_MB; i++)
    {
        addr = (start_addr & ARM_L1_PT_SECT_ADDR_MASK) + (i*1024*1024);
        index = addr >> ARM_L1_PT_SECT_ADDR_SHFT;
        _pagetable[index] = addr | attrib;
    }

    /* Invalidate the TLB */
    WCP15_UTLBIALL(0);
}

/*
 * Unmaps requested region in the MMU page table
 */
void mmu_unmap_region(uint32_t start_addr, uint32_t size)
{
    uint32_t size_in_MB;
    int i;
    uint32_t addr;
    int index;

    if (size == 0)
    {
        /* Nothing to do */
        return;
    }

    size_in_MB = size / (1024 * 1024);
    if (size % (1024 * 1024) != 0)
    {
        size_in_MB++;
    }

    for (i=0; i<size_in_MB; i++)
    {
        addr = (start_addr & ARM_L1_PT_SECT_ADDR_MASK) + (i*1024*1024);
        index = addr >> ARM_L1_PT_SECT_ADDR_SHFT;
        _pagetable[index] = 0;
    }

    /* Invalidate the TLB */
    WCP15_UTLBIALL(0);
}

/*
 * Checks if address is currently mapped.
 */
int mmu_is_mapped(uint32_t addr)
{
    int index;
    uint32_t pt_entry;

    index = addr >> ARM_L1_PT_SECT_ADDR_SHFT;
    pt_entry = _pagetable[index];
    if ((pt_entry & ARM_L1_PT_SECT_ADDR_MASK) == 
        (addr & ARM_L1_PT_SECT_ADDR_MASK))
    {
        return 1;    /* yes */
    }
    else
    {
        return 0;    /* no */
    }
}
