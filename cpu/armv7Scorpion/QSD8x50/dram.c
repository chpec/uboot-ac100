/*
 * Copyright (c) 2009, Code Aurora Forum. All rights reserved.
 *
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
 *
 * (C) Copyright 2002
 * David Mueller, ELSOFT AG, <d.mueller@elsoft.ch>
 *
 * (C) Copyright 2003
 * Texas Instruments, <www.ti.com>
 * Kshitij Gupta <Kshitij@ti.com>
 *
 * (C) Copyright 2004
 * ARM Ltd.
 * Philippe Robin, <philippe.robin@arm.com>
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
#include <asm/arch/memtypes.h>

int dram_reinit (uint32_t start_addr, uint32_t size, int bank_num)
{
    DECLARE_GLOBAL_DATA_PTR;
    bd_t *bd = gd->bd;


    /*
     * First unmap old region from MMU.
     */
    mmu_unmap_region(bd->bi_dram[bank_num].start,
                     bd->bi_dram[bank_num].size);


    bd->bi_dram[bank_num].start = start_addr;
    bd->bi_dram[bank_num].size = size;

    /*
     * Map new region in MMU.
     */
    mmu_map_region(bd->bi_dram[bank_num].start,
                   bd->bi_dram[bank_num].size,
                   MEM_TYPE_KERNEL);

    return 0;
}

int dram_init (void)
{
    dram_reinit(PHYS_SDRAM_1, PHYS_SDRAM_1_SIZE, 0);
#if CONFIG_NR_DRAM_BANKS >= 2
    dram_reinit(PHYS_SDRAM_2, PHYS_SDRAM_2_SIZE, 1);
#endif
    return 0;
}

