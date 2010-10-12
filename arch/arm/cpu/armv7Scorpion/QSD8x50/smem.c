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

volatile static smem_shared_t *shared = (void*) SMEM_START;

int smem_init(void)
{
	if (shared->heap_info.initialized)
		return SMEM_SUCCESS;
	else
		return SMEM_ERROR;
}

void * smem_get_entry(unsigned int id, unsigned int *size)
{
	smem_heap_entry_t *he;

	if (!shared->heap_info.initialized)
		return NULL;

	if (id >= SMEM_TABLE_ENTRIES)
		return NULL;

	he = &(shared->heap_toc[id]);
	if (he && (!he->allocated || he->size == 0))
		return NULL;

	if (size)
		*size = he->size;

	return ((void *) shared) + he->offset;
}
