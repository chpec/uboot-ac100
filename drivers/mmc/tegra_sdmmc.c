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

/* MMC_DEV_INSTANCES, NvEmmcx_x defined in header file like tegra2_harmony.h, tegra2_seaboard.h */
NvEmmcDeviceId mmc_device_id[5] = {
	NvEmmcx_0,
	NvEmmcx_1,
	NvEmmcx_2,
	NvEmmcx_3,
	0
};

static block_dev_desc_t mmc_blk_dev[MMC_DEV_INSTANCES];
static int mmc_current_dev_index = -1;
static NvBootSdmmcContext s_Context;

block_dev_desc_t *mmc_get_dev(int dev)
{
	if (dev >= MMC_DEV_INSTANCES)
		return NULL;

	if (mmc_current_dev_index != dev)
		return NULL;

	return &mmc_blk_dev[dev];
}

NvEmmcDeviceId mmc_get_device_id(int dev)
{
	if (dev >= 0 && dev < MMC_DEV_INSTANCES) {
	    return mmc_device_id[dev];
        }

        /* undefined device */
	return NvUnknownId;
}

NvEmmcDeviceId mmc_get_current_device_id(void)
{
        return mmc_get_device_id(mmc_current_dev_index);
}

inline
void mmc_set_current_device(int dev)
{
        mmc_current_dev_index = dev;
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

    if (mmc_current_dev_index != dev) {
        printf("Read Failed. Please issue 'mmc init %d' first\n", dev); 
    	return 0;
    }

    pages_2_read = blkcnt;      /* TODO: change to generic code. */
                                /* here assume page_size == block_size. */
    pages_per_block = 32;	/* TODO: change to generic code. */
    page_size = DUMMY_PAGE_SIZE;            /* TODO: */

    for (i=0; i < pages_2_read; i++) {
         block_2_read = (start + i)/pages_per_block;
         page_2_read = ((start + i) % pages_per_block);

         if (SdmmcReadPage(block_2_read, page_2_read, p_buffer)) {
		goto fail;
         };

         memcpy((unsigned char *)buffer, p_buffer, page_size);
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

    if (mmc_current_dev_index != dev) {
        printf("Write Failed. Please issue 'mmc init %d' first\n", dev); 
    	return 0;
    }

    pages_2_write = blkcnt;     /* TODO: change to generic code. */
                                /* here assume page_size == block_size. */
    pages_per_block = 32;	/* TODO: change to generic code. */
    page_size = DUMMY_PAGE_SIZE;            /* TODO: */

    for (i=0; i < pages_2_write; i++) {
         block_2_write = (start + i)/pages_per_block;
         page_2_write = ((start + i) % pages_per_block);

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

int mmc_legacy_init(int dev)
{
    NvBootSdmmcParams *Params;
    NvU32 BlockSize;
    NvU32 PageSize;
    NvU32 ParamIndex = 0;

    mmc_set_current_device(dev);

    p_buffer = (unsigned char *)dummy_buffer;

    /* init SD/MMC */
    switch (mmc_get_device_id(dev)) {
    case NvEmmc4:
        ParamIndex = 0x30;
        break;
    case NvEmmc2:
        ParamIndex = 0x12;
        break;
    case NvEmmc3:
        ParamIndex = 0x12;
        break;
    default:
        printf("[%s:%d] Invalid device number %d\n", __FUNCTION__, __LINE__, dev); 
	goto fail;
    }

    NvBootSdmmcGetParams(ParamIndex, &Params);
    if (NvBootSdmmcInit(mmc_get_device_id(dev), Params, &s_Context)) {
	goto fail;
    }

    NvBootSdmmcGetBlockSizes(mmc_get_device_id(dev), Params, &BlockSize, &PageSize);

    mmc_blk_dev[dev].blksz = (1 << PageSize);       

    mmc_blk_dev[dev].if_type = IF_TYPE_MMC;
    mmc_blk_dev[dev].part_type = PART_TYPE_DOS;
    mmc_blk_dev[dev].dev = dev;
    mmc_blk_dev[dev].lun = 0;
    mmc_blk_dev[dev].type = 0;

    /* This needs to be probed, go with something sufficiently large to read
     * the partition table for now
     */
    mmc_blk_dev[dev].lba = 0x10000000;
    mmc_blk_dev[dev].removable = 0;

    /* Block Readsi/Writes should use the SD Block Driver Read/Write interface */
    mmc_blk_dev[dev].block_read = mmc_bread;
    mmc_blk_dev[dev].block_write = mmc_bwrite;
    printf("EMMC %d Probed Successfully\n", dev);
    init_part(&mmc_blk_dev[dev]);
    return 0;

fail :
    printf("[%s:%d] EMMC %d Probe Failed\n", __FUNCTION__, __LINE__, dev); 
    mmc_set_current_device(-1);
    return -1 ; 
}
