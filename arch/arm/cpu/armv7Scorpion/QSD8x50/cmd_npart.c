/*
 * Copyright 2010, Google Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 * * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Alternatively, this software may be distributed under the terms of the
 * GNU General Public License ("GPL") version 2 as published by the Free
 * Software Foundation.
 */

#include <common.h>

#if defined(CONFIG_CMD_NPART)
#include <command.h>

char unsigned volatile *	shared_start = (char unsigned volatile *) SMEM_START;
size_t				shared_size  = (size_t) SMEM_SIZE;

typedef struct ProcCommStruct
{
  uint			command;
  uint			status;
  uint			data1;
  uint			data2;
} ProcComm;

typedef struct HeapInfoStruct
{
  uint			stuff[4];
} HeapInfo;

typedef struct HeapEntryStruct
{
  uint			allocated;
  uint			offset;
  uint			size;
  uint			reserved;
} HeapEntry;

typedef struct PartitionEntryStruct
{
  char			name[16];
  int			start;       // start block number
  int			size;        // size in blocks
  int			attributes;
} PartitionEntry;

typedef struct PartitionTableStruct
{
  uint			magic[2];  // 0x55EE7EAA 0xE35EBDDB
  uint			version;   // mine appears to be version 3
  uint			length;    // length of partitions[] array
  PartitionEntry	partitons[];
} PartitionTable;

typedef struct SharedStruct
{
  ProcComm		proc_comm[4];
  uint			version[32];
  HeapInfo		heap_info;
  HeapEntry		heap_toc[128];
} Shared;

#define	SMEM_AARM_PARTITION_TABLE	9

/*************************************************************************************************/
int cmd_npart(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
  Shared         volatile *	shared     = (Shared         volatile *) shared_start;
  HeapEntry      volatile *	heap_entry = shared->heap_toc + SMEM_AARM_PARTITION_TABLE;
  PartitionTable volatile *	table      = (PartitionTable volatile *) (shared_start +
                                                                      heap_entry->offset);
  uint				i;

  printf("magic...........: %x:%x\n", table->magic[0], table->magic[1]);
  printf("version.........: %u\n", table->version);
  printf("length..........: %u\n", table->length);

  for (i = 0; i < table->length; ++i)
  {
    char			name[17];
    PartitionEntry volatile *	entry = table->partitons + i;

    strncpy(name, (char *)(entry->name), 16);
    name[16] = '\0';

    printf("    name........: %s\n", name);
    printf("    start.......: %d\n", entry->start);
    printf("    size........: %d\n", entry->size);
    printf("    attributes..: %x\n", entry->attributes);
  }

  return 0;
}
/*************************************************************************************************/

U_BOOT_CMD(npart, 1, 0, cmd_npart, "npart - display NAND partition information.\n", NULL);

#endif /* CONFIG_CMD_NPART */
