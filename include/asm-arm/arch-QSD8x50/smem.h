/*
 *  smem.h - Shared memory definitions
 *
 * Copyright (c) 2009-2010, Code Aurora Forum. All rights reserved.
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

#ifndef __QC_SMEM_H
#define __QC_SMEM_H

#include <asm/arch/proc_comm.h>

#define SMEM_SUCCESS	0
#define SMEM_ERROR	-1


#define SMEM_FLASH_ERASE_SIZE			0x20000

/* Flash partition table magic numbers */
#define SMEM_FLASH_PARTITION_TBL_MAGIC1		0x55EE73AA
#define SMEM_FLASH_PARTITION_TBL_MAGIC2		0xE35EBDDB

/* Flash bad block table magic numbers */
#define SMEM_FLASH_BAD_BLOCK_TBL_MAGIC1		0xAA01DE33
#define SMEM_FLASH_BAD_BLOCK_TBL_MAGIC2		0xDBFE99AF

/* Partition table related constants */
#define SMEM_AARM_PARTITION_TABLE	9 /* index into toc */
#define SMEM_MAX_NUM_PARTITIONS		16
#define SMEM_MAX_PARTITION_NAME_LEN	16

/* Shared mem constants */
#define SMEM_PROC_COMM_ENTRIES		4
#define SMEM_VERSION_ENTRIES		32
#define SMEM_TABLE_ENTRIES		128


typedef struct {
	u32 initialized;
	u32 free_offset;
	u32 heap_remaining;
	u32 reserved;
} smem_heap_info_t;

typedef struct {
	u32 allocated;
	u32 offset;
	u32 size;
	u32 reserved;
} smem_heap_entry_t;

typedef struct {
	proc_comm_t proc_comm[SMEM_PROC_COMM_ENTRIES];
	u32 version[SMEM_VERSION_ENTRIES];
	smem_heap_info_t heap_info;
	smem_heap_entry_t heap_toc[SMEM_TABLE_ENTRIES];
} smem_shared_t;

typedef struct {
	char name[SMEM_MAX_PARTITION_NAME_LEN];
	u32 start_blk;
	u32 numblocks;
	u32 attr;
} flash_part_t  __attribute__ ((__packed__));

typedef struct {
	u32 magic_num1;
	u32 magic_num2;
	u32 version;
	u32 num_partitions;
	flash_part_t partition[SMEM_MAX_NUM_PARTITIONS];
} smem_flash_partition_tbl_t  __attribute__ ((__packed__));


/* Flash bad block table structure */
typedef struct {
	u32 magic_num1;
	u32 magic_num2;
	u32 version;
	u32 num_bad_blocks;
	u32 bad_block[];
} smem_flash_bad_block_tbl_t;

int smem_init(void);
void * smem_get_entry(unsigned int id, unsigned int *size);

int smem_ptable_init(void);
flash_part_t* smem_ptable_get_fota_ptn(void);

#endif  /* __QC_SMEM_H */
