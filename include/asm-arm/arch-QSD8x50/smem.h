/*
 *  smem.h - Shared memory definitions
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

#ifndef __QC_SMEM_H
#define __QC_SMEM_H


// Flash partition table magic numbers

#define SMEM_FLASH_PARTITION_TBL_MAGIC1     0x55EE73AA
#define SMEM_FLASH_PARTITION_TBL_MAGIC2     0xE35EBDDB

#define NUM_PARTITIONS_MAX   2

typedef struct flash_part
{
  char name[16];
  uint32_t start_blk;
  uint32_t numblocks;
  uint32_t res;
} flash_part_t;

                                           
typedef struct smem_flash_partition_tbl
{
  uint32_t magic_num1;
  uint32_t magic_num2;
  uint32_t version;
  uint32_t num_partitions;
  flash_part_t paritition[NUM_PARTITIONS_MAX];
} smem_flash_partition_tbl_t;

                                           
                                           
// Flash bad block table magic numbers                                           
#define SMEM_FLASH_BAD_BLOCK_TBL_MAGIC1     0xAA01DE33 
#define SMEM_FLASH_BAD_BLOCK_TBL_MAGIC2     0xDBFE99AF

// Flash bad block table structure

typedef struct smem_flash_bad_block_tbl
{
  uint32_t  magic_num1; 
  uint32_t  magic_num2;
  uint32_t  version;
  uint32_t  num_bad_blocks;
  uint32_t  bad_block[];
} smem_flash_bad_block_tbl_t;

#endif  /* __QC_SMEM_H */
