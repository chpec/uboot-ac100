/*
 * Copyright (c) 2010, NVIDIA Corporation.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */


#include <common.h>
#include <part.h>
#include <malloc.h>
#include <asm/arch/nv_sdmmc.h>

#undef DEBUG
#ifdef DEBUG
#define debug(fmt,args...)  printf (fmt ,##args)
#else
#define debug(fmt,args...)
#endif

static block_dev_desc_t mmc_blk_dev;


#ifdef NVRM
/* HardCode the Device ID and Instance */
static NvU32 BlockDevInstance = 1; 
static NvDdkBlockDevHandle hBlockDevHandle;
static NvDdkBlockDevInfo BlockDevInfo;
static NvRmDeviceHandle s_RmDevice = NULL; 
#endif   // NVRM

block_dev_desc_t *mmc_get_dev(int dev)
{
	return &mmc_blk_dev;
}

void nv_debug_dump (unsigned char *buffer, unsigned int size)
{
#define DUMP_LINE_SIZE  16
    int i, j;

    for (i=0; i<(size+DUMP_LINE_SIZE-1)/DUMP_LINE_SIZE; ++i) {
       printf("\n%08x ", (unsigned int)(buffer + i*DUMP_LINE_SIZE));
       for (j=0; (j < DUMP_LINE_SIZE) && ((i*DUMP_LINE_SIZE + j) < size); ++j) {
           if ((j % (DUMP_LINE_SIZE>>1)) == 0) {
               printf(" ");
           }
           printf("%02x ", (char)*(buffer + i*DUMP_LINE_SIZE + j));
       }
    }
    printf("\n");
}

/* The block read assumes that the incoming reads are for 512 block reads 
 * We will convert them to 2048 block reads and return the requested 512 bytes 
 * This means that writes will be more complicated :( 
 */	
#define DUMMY_BUFFER_SIZE	0x1000
#define DUMMY_PAGE_SIZE		512
static char dummy_buffer[DUMMY_BUFFER_SIZE+DUMMY_PAGE_SIZE] __attribute__ ((aligned(4096))); /*  Aligned on 4k byte boundary */;
static unsigned char *p_buffer;

static unsigned long
mmc_bread(int dev, unsigned long start, lbaint_t blkcnt,
	  void *buffer)
{
    unsigned int i, block_2_read, page_2_read;
    unsigned int pages_2_read, pages_per_block, page_size;

    pages_2_read = blkcnt;      // TODO: change to generic code. 
                                // here assume page_size == block_size.
    pages_per_block = 32;	// TODO: change to generic code.
    page_size = DUMMY_PAGE_SIZE;            // TODO:

    for (i=0; i < pages_2_read; i++) {
         block_2_read = (start + i)/pages_per_block;
         page_2_read = ((start + i) % pages_per_block);

         if (SdmmcReadPage(block_2_read, page_2_read, p_buffer)) {
		goto fail;
         };

         memcpy((unsigned char *)buffer, p_buffer, page_size);
#if 0
         nv_debug_dump (buff, page_size);
         nv_debug_dump ((unsigned char *)buffer, page_size);
#endif
         buffer += page_size;
    }
    return blkcnt;
fail : 
    printf(" %s:%d READ : failed \n", __FUNCTION__,__LINE__);
    return 0;
}

/* Since there is a mismatch in the sector size between ext2 (512) and the block driver (2048)
 * we do a read-modify-write as part of the block write. 
 */    

static unsigned long
mmc_bwrite(int dev,unsigned long start,lbaint_t blkcnt,
           const void *buffer)
{
    unsigned int i, block_2_write, page_2_write;
    unsigned int pages_2_write, pages_per_block, page_size;

    pages_2_write = blkcnt;      // TODO: change to generic code. 
                                // here assume page_size == block_size.
    pages_per_block = 32;	// TODO: change to generic code.
    page_size = DUMMY_PAGE_SIZE;            // TODO:

    for (i=0; i < pages_2_write; i++) {
         block_2_write = (start + i)/pages_per_block;
         page_2_write = ((start + i) % pages_per_block);

#if 0
    printf(" %s:start 0x%x (%d), blks %d, buff_addr %p, l_buff %p\n", __FUNCTION__,
                (int)start, i, (int)blkcnt, (char *)buffer, p_buffer);
#endif

         memcpy(p_buffer, (unsigned char *)buffer, page_size);
         if (SdmmcWritePage(block_2_write, page_2_write, p_buffer)) {
		goto fail;
         };

         buffer += page_size;
    }
    return blkcnt;
fail : 
    printf(" %s:%d WRITE : failed \n", __FUNCTION__,__LINE__);
    return i;
}

static NvBootSdmmcContext s_SdmmcContext;
int mmc_legacy_init(int verbose)
{
    NvBootSdmmcParams *Params;
    NvU32 BlockSize;
    NvU32 PageSize;

    p_buffer = (unsigned char *)dummy_buffer;

    // init SD/MMC
    NvBootSdmmcGetParams(0x30, &Params);
    if (NvBootSdmmcInit(Params, &s_SdmmcContext)) {
	goto fail;
    }

    NvBootSdmmcGetBlockSizes(Params, &BlockSize, &PageSize);

//    printf("BlockSize: %d, PageSize: %d\n", BlockSize, PageSize);

    mmc_blk_dev.blksz = (1 << PageSize);       

    mmc_blk_dev.if_type = IF_TYPE_MMC;
    mmc_blk_dev.part_type = PART_TYPE_DOS;
    mmc_blk_dev.dev = 0;
    mmc_blk_dev.lun = 0;
    mmc_blk_dev.type = 0;

    /* This needs to be probed, go with something sufficiently large to read
     * the partition table for now
     */
    mmc_blk_dev.lba = 0x10000000;
    mmc_blk_dev.removable = 0;

    /* Block Readsi/Writes should use the SD Block Driver Read/Write interface */
    mmc_blk_dev.block_read = mmc_bread;
    mmc_blk_dev.block_write = mmc_bwrite;
    printf("EMMC Probed Successfully\n");
    init_part(&mmc_blk_dev);
    return 0;

fail :
    printf("[%s:%d] EMMC Probe Failed\n", __FUNCTION__, __LINE__); 
    return -1 ; 
}
