/*
Copyright (c) 2010, Code Aurora Forum. All rights reserved.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License version 2 and
only version 2 as published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
02110-1301, USA.
*/
#include <common.h>
#include <asm/arch/smem.h>

/* partition table from SMEM */
static smem_flash_partition_tbl_t *ptable = NULL;

/* FOTA partition */
static flash_part_t fota_ptn = {
	.name = "",
};

int smem_ptable_init(void)
{
	unsigned i;
	unsigned size;

	ptable = smem_get_entry(SMEM_AARM_PARTITION_TABLE, &size);

	if (ptable == NULL)
		return SMEM_ERROR;

	if ((ptable->magic_num1 != SMEM_FLASH_PARTITION_TBL_MAGIC1) ||
		(ptable->magic_num2 != SMEM_FLASH_PARTITION_TBL_MAGIC2))
		return SMEM_ERROR;

	/* find FOTA partition */
	for (i = 0; i < ptable->num_partitions; i++) {
		if (!strcmp(ptable->partition[i].name, "0:FOTA")) {
			strcpy(fota_ptn.name, "fota");
			fota_ptn.start_blk = ptable->partition[i].start_blk;
			fota_ptn.numblocks = ptable->partition[i].numblocks;
			fota_ptn.attr = ptable->partition[i].attr;
			break;
		}
	}

	return SMEM_SUCCESS;
}

flash_part_t* smem_ptable_get_fota_ptn(void)
{
	if (fota_ptn.name[0])
		return &fota_ptn;
	else
		return NULL;
}
