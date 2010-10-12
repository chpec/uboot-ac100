/*
 *  nand_qc.c - NAND functions using NAND controller and Application Data Mover
 *
 * Copyright (c) 2009-10, Code Aurora Forum. All rights reserved.
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
#ifdef CONFIG_CMD_NAND
#include <nand.h>
#include <malloc.h>
#include <asm-arm/arch/smem.h>
#include <asm/arch/adm.h>
#include <asm-armv7Scorpion/mmu.h>
#include <asm/errno.h>

smem_flash_bad_block_tbl_t * bbt = NULL;
int apps_part_start_blk = 0;
int apps_part_numblocks = 0;
unsigned last_command = 0;
unsigned id_count = 0;



#define NAND_RD_PAGE_ADM_CMD_PTR_NUM      20      // 5 command lists * 4 codewords
#define NAND_RD_ID_ADM_CMD_PTR_NUM         6      
                                             
// ADM command pointer list for NAND Read ID 
uint32_t nand_rd_id_adm_cmd_ptr_list[NAND_RD_ID_ADM_CMD_PTR_NUM] __attribute__ ((aligned(8))); // Must aligned on 8 byte boundary

// ADM command lists for NAND Read ID
si_cmd_list_t   rd_id_reset_lst[5]        __attribute__ ((aligned(8))); // Must aligned on 8 byte boundary  
si_cmd_list_t   rd_id_execute_lst         __attribute__ ((aligned(8))); // Must aligned on 8 byte boundary  
si_cmd_list_t   rd_id_status_lst          __attribute__ ((aligned(8))); // Must aligned on 8 byte boundary  
si_cmd_list_t   rd_id_cmd_lst             __attribute__ ((aligned(8))); // Must aligned on 8 byte boundary  
si_cmd_list_t   rd_id_read_id_lst[2]      __attribute__ ((aligned(8))); // Must aligned on 8 byte boundary  

// Data for NAND Read ID command lists
uint32_t rd_id_reset_chipsel;         
uint32_t rd_id_reset_burstm;
uint32_t rd_id_reset_cmd[3];
uint32_t rd_id_reset_cfg[2];
uint32_t rd_id_reset_cmd_vld;
uint32_t rd_id_execute_data[1];
uint32_t rd_id_fetchid_cmd[1];

// Need to manage coherency on the following variables because ADM writes to them.
// Align them on an L1 cache boundary and make them the same size as an L1 cache
// line so they don't share a cache line with any other variables or structures.
// Only the first uint32_t in each array is used.
uint32_t rd_id_status_reset[8]    __attribute__ ((aligned(32))); // Must aligned on 32 byte boundary
uint32_t rd_id_status_fetch_id[8] __attribute__ ((aligned(32))); // Must aligned on 32 byte boundary
uint32_t rd_id_flash_id_value[8]  __attribute__ ((aligned(32))); // Must aligned on 32 byte boundary

// ADM command pointer list for NAND Read Page
uint32_t nand_rd_page_adm_cmd_ptr_list[NAND_RD_PAGE_ADM_CMD_PTR_NUM] __attribute__ ((aligned(8))); // Must aligned on 8 byte boundary

// ADM command lists for NAND Read Page
si_cmd_list_t   rd_page_cmd_setup_lst     __attribute__ ((aligned(8))); // Must aligned on 8 byte boundary  
si_cmd_list_t   rd_page_cfg_lst[2]        __attribute__ ((aligned(8))); // Must aligned on 8 byte boundary
si_cmd_list_t   rd_page_execute_lst       __attribute__ ((aligned(8))); // Must aligned on 8 byte boundary
si_cmd_list_t   rd_page_status_lst[4]     __attribute__ ((aligned(8))); // Must aligned on 8 byte boundary
si_cmd_list_t   rd_page_read_data_lst[4]  __attribute__ ((aligned(8))); // Must aligned on 8 byte boundary

// Data for NAND Read Page command lists
uint32_t rd_page_cmd_setup_data[4];
uint32_t rd_page_cfg_data_cfgn[2];
uint32_t rd_page_cfg_data_ecc[1];         
uint32_t rd_page_execute_data[1];       

// Need to manage coherency on this array because ADM writes status to it that the CPU then
// has to check. Align it on an L1 cache boundary and make it the same size as an L1 cache
// line so it does not share a cache line with any other variables. 
uint32_t rd_page_status[8]  __attribute__ ((aligned(32))); // Must aligned on 32 byte boundary

extern int apps_part_start_blk;
extern int apps_part_numblocks;

#define NAND_CODEWORD_SIZE  0x200

static int nand_read_page_dm(nand_info_t *nand, uint32_t page, u_char *buf);

/*
 * nand_read_qc -
 * NAND page read 
 */
int nand_read_qc(nand_info_t *nand, off_t ofs, size_t *len, u_char *buf)
{
    struct nand_chip *dev = nand->priv;
    size_t bytes_remaining = *len;
    u_char *buflocal = buf;
    uint32_t ofspage;
    uint32_t apps_part_start_page;
    uint32_t ofsblock;
    uint32_t numblocks;
    u_char * pagebuf = NULL;
    int rc;
    

    if (bytes_remaining == 0)
    {
        return 0;
    }

    // Verify buffer address is mapped in the MMU.
    if (mmu_is_mapped((uint32_t)buf) == 0)
    {
        printf("ERROR: Invalid target address\n");
        return -EINVAL;
    }

    if (apps_part_start_blk == 0)
    {
        printf("NAND: starting block of APPS partition is unknown\n");
        return 0;
    }

    /* 
     * Can't assume the user knows which block the APPS partition starts in.
     * To boot from NAND, user should run "nboot <addr> 0 0".  (offset = 0)
     * This function will add the APPS partition offset automatically.
     */

    // Convert ofs (byte) to a block offset
    ofsblock = ofs >> dev->phys_erase_shift;
    ofsblock += apps_part_start_blk;
    numblocks = (*len >> dev->phys_erase_shift);
    // Add a block for a partial block. "pagemask" is really a "block" mask.
    if (*len & dev->pagemask)
    {
        numblocks++;
    }

    // Make sure the page address is within the apps partition.
    if ((ofsblock + numblocks) > (apps_part_start_blk + apps_part_numblocks))
    {
        return -EINVAL;
    }

    // Convert the offset to a page offset, again with respect to the
    // start of the APPS partition.
    apps_part_start_page = apps_part_start_blk << (dev->phys_erase_shift - dev->page_shift);
    ofspage = apps_part_start_page + (ofs >> dev->page_shift);

    while (1)
    {
       if (bytes_remaining < dev->subpagesize)
       {
          // allocate a page size buffer and read into it
          pagebuf = malloc(dev->subpagesize);
          if (pagebuf == NULL)
          {
              return(-ENOMEM);        
          }
          rc = nand_read_page_dm(nand, ofspage, pagebuf);
          // copy just the remaining bytes to caller's buf
          memcpy(buflocal, pagebuf, bytes_remaining);
          free(pagebuf);
          bytes_remaining = 0;
       }
       else
       {
          // Read a full page into caller's buffer
          rc = nand_read_page_dm(nand, ofspage, buflocal);
          ofspage++;
          bytes_remaining -= dev->subpagesize;
          buflocal += dev->subpagesize;
       }

       if (rc != 0)
       {
           return (-EIO);
       }

       if (bytes_remaining == 0)
       {
           break;
       }
    }

    return(0);

}


/* 
 * nand_read_id_dm -
 * NAND reset and read flash ID using NAND controller and ADM
 */ 
int nand_read_id_dm(nand_info_t *nand)
{
    // Initialize the Reset list.
    rd_id_reset_chipsel = NAND_FLASH_CHIP_SELECT__DM_EN___M;

    rd_id_reset_lst[0].first = ADM_CMD_LIST_OCB | ADM_ADDR_MODE_SI;
    rd_id_reset_lst[0].src_addr = (uint32_t)&rd_id_reset_chipsel;
    rd_id_reset_lst[0].dst_addr = NAND_FLASH_CHIP_SELECT;
    rd_id_reset_lst[0].len = sizeof(rd_id_reset_chipsel);

    rd_id_reset_burstm = 0x00000000;  // Disable burst mode

    rd_id_reset_lst[1].first = ADM_ADDR_MODE_SI;
    rd_id_reset_lst[1].src_addr = (uint32_t)&rd_id_reset_burstm;
    rd_id_reset_lst[1].dst_addr = SFLASHC_BURST_CFG;
    rd_id_reset_lst[1].len = sizeof(rd_id_reset_burstm);

    rd_id_reset_cmd[0] = NAND_FLASH_CMD__OP_CMD__RESET_NAND_FLASH_DEVICE_OR_ONENAND_REGISTER_WRI;
    rd_id_reset_cmd[1] = 0x00000000;     // ADDR0
    rd_id_reset_cmd[2] = 0x00000000;     // ADDR1

    rd_id_reset_lst[2].first = (ADM_CRCI_NAND_CMD << 7) | ADM_ADDR_MODE_SI;
    rd_id_reset_lst[2].src_addr = (uint32_t)rd_id_reset_cmd;
    rd_id_reset_lst[2].dst_addr = NAND_FLASH_CMD;
    rd_id_reset_lst[2].len = sizeof(rd_id_reset_cmd);

    rd_id_reset_cfg[0] = CONFIG_QC_NAND_NAND_DEVn_CFG0_RD_ID_VAL; 
    rd_id_reset_cfg[1] = CONFIG_QC_NAND_NAND_DEVn_CFG1_RD_ID_VAL;
     
    rd_id_reset_lst[3].first = ADM_ADDR_MODE_SI;
    rd_id_reset_lst[3].src_addr = (uint32_t)rd_id_reset_cfg;
    rd_id_reset_lst[3].dst_addr = NAND_DEV0_CFG0;
    rd_id_reset_lst[3].len = sizeof(rd_id_reset_cfg);

    rd_id_reset_cmd_vld = FLASH_DEV_CMD_VLD__READ_START_VLD___M |
                          FLASH_DEV_CMD_VLD__WRITE_START_VLD___M |
                          FLASH_DEV_CMD_VLD__ERASE_START_VLD___M |
                          FLASH_DEV_CMD_VLD__SEQ_READ_START_VLD___M;

    rd_id_reset_lst[4].first = ADM_CMD_LIST_LC | ADM_ADDR_MODE_SI;
    rd_id_reset_lst[4].src_addr = (uint32_t)&rd_id_reset_cmd_vld;
    rd_id_reset_lst[4].dst_addr = FLASH_DEV_CMD_VLD;
    rd_id_reset_lst[4].len = sizeof(rd_id_reset_cmd_vld);                      

    // Initialize the Execute list.
    rd_id_execute_data[0] = NANDC_EXEC_CMD__EXEC_CMD__EXECUTE_THE_COMMAND;

    rd_id_execute_lst.first = ADM_CMD_LIST_LC | ADM_ADDR_MODE_SI;
    rd_id_execute_lst.src_addr = (uint32_t)rd_id_execute_data;
    rd_id_execute_lst.dst_addr = NANDC_EXEC_CMD;
    rd_id_execute_lst.len = sizeof(rd_id_execute_data);

    // Initialize the Status list.
    rd_id_status_lst.first = ADM_CMD_LIST_LC | (ADM_CRCI_NAND_DATA << 3) | ADM_ADDR_MODE_SI;
    rd_id_status_lst.src_addr = NAND_FLASH_STATUS;
    rd_id_status_lst.dst_addr = (uint32_t)&rd_id_status_reset[0];
    rd_id_status_lst.len = sizeof(uint32_t);

    // Initialize the Fetch ID command list.
    rd_id_fetchid_cmd[0] = NAND_FLASH_CMD__OP_CMD__FETCH_ID;

    rd_id_cmd_lst.first = ADM_CMD_LIST_LC | ADM_CMD_LIST_OCB | (ADM_CRCI_NAND_CMD << 7) | ADM_ADDR_MODE_SI;
    rd_id_cmd_lst.src_addr = (uint32_t)rd_id_fetchid_cmd;
    rd_id_cmd_lst.dst_addr = NAND_FLASH_CMD;
    rd_id_cmd_lst.len = sizeof(rd_id_fetchid_cmd);

    // Initialize the command list to read the ID
    rd_id_read_id_lst[0].first = (ADM_CRCI_NAND_DATA << 3) | ADM_ADDR_MODE_SI;
    rd_id_read_id_lst[0].src_addr = NAND_FLASH_STATUS;
    rd_id_read_id_lst[0].dst_addr = (uint32_t)&rd_id_status_fetch_id[0];
    rd_id_read_id_lst[0].len = sizeof(uint32_t);

    rd_id_read_id_lst[1].first = ADM_CMD_LIST_LC | ADM_CMD_LIST_OCU | ADM_ADDR_MODE_SI;
    rd_id_read_id_lst[1].src_addr = NAND_FLASH_READ_ID;
    rd_id_read_id_lst[1].dst_addr = (uint32_t)&rd_id_flash_id_value[0];
    rd_id_read_id_lst[1].len = sizeof(uint32_t);

    // Build the ADM command pointer list
    nand_rd_id_adm_cmd_ptr_list[0] = ADM_CMD_PTR_CMD_LIST | ((uint32_t)&rd_id_reset_lst[0] >> 3);
    nand_rd_id_adm_cmd_ptr_list[1] = ADM_CMD_PTR_CMD_LIST | ((uint32_t)&rd_id_execute_lst >> 3);
    nand_rd_id_adm_cmd_ptr_list[2] = ADM_CMD_PTR_CMD_LIST | ((uint32_t)&rd_id_status_lst >> 3);
    nand_rd_id_adm_cmd_ptr_list[3] = ADM_CMD_PTR_CMD_LIST | ((uint32_t)&rd_id_cmd_lst >> 3);
    nand_rd_id_adm_cmd_ptr_list[4] = ADM_CMD_PTR_CMD_LIST | ((uint32_t)&rd_id_execute_lst >> 3);
    nand_rd_id_adm_cmd_ptr_list[5] = ADM_CMD_PTR_CMD_LIST | ((uint32_t)&rd_id_read_id_lst[0] >> 3) | ADM_CMD_PTR_LP;

    // Start ADM transfer
    if (adm_start_transfer(ADM_AARM_NAND_CHN, nand_rd_id_adm_cmd_ptr_list) != 0)
    {
       return(-EIO);
    }

#ifdef CONFIG_DCACHE
    // Invalidate cache so structures ADM updated can be seen.
    invalidate_dcache_range((uint32_t)&rd_id_status_reset[0],
                            (uint32_t)&rd_id_status_reset[0] + sizeof(rd_id_status_reset)); 
    invalidate_dcache_range((uint32_t)&rd_id_status_fetch_id[0],
                            (uint32_t)&rd_id_status_fetch_id[0] + sizeof(rd_id_status_fetch_id));
    invalidate_dcache_range((uint32_t)&rd_id_flash_id_value[0],
                            (uint32_t)&rd_id_flash_id_value[0] + sizeof(rd_id_flash_id_value));
#endif

    // Check Status values
    if ((rd_id_status_reset[0] & NAND_FLASH_STATUS__OP_ERR___M) != 0)
    {
        printf("NAND: read id received bad status from reset 0x%x\n",rd_id_status_reset[0]);
        return(-EIO);
    }

    if ((rd_id_status_fetch_id[0] & NAND_FLASH_STATUS__OP_ERR___M) != 0)
    {
        printf("NAND: read id received bad status from fetch ID 0x%x\n",rd_id_status_fetch_id[0]);
        return(-EIO);
    }

    return 0; 

}


/*
 * nand_read_page_dm
 * NAND read page using NAND controller and ADM. 
 */
static int nand_read_page_dm(nand_info_t *nand, uint32_t page, u_char *buf)
{
  
    struct nand_chip *dev = nand->priv;
    int codewords_per_page;
    int i;
    uint32_t *cmd_ptr_list;
    static int rd_page_struct_common_init = 0;


    // Calculate number of code words per page. The NAND controller reads
    // 512 bytes (one codeword) at a time.
    codewords_per_page = dev->subpagesize / NAND_CODEWORD_SIZE;

    // Most of the ADM data structures need to be initialized only once. 
    if (rd_page_struct_common_init == 0) 
    {
        // Initialize the command setup list.
        rd_page_cmd_setup_data[0] = NAND_FLASH_CMD__OP_CMD__PAGE_READ_WITH_ECC |
                                    NAND_FLASH_CMD__PAGE_ACC___M |
                                    NAND_FLASH_CMD__LAST_PAGE___M;
        // ADDR0 and ADDR1 are setup below (for each page)
        rd_page_cmd_setup_data[3] = NAND_FLASH_CHIP_SELECT__DM_EN___M;
    
        rd_page_cmd_setup_lst.first = ADM_CMD_LIST_LC | ADM_CMD_LIST_OCB | (ADM_CRCI_NAND_CMD << 7) | ADM_ADDR_MODE_SI;
        rd_page_cmd_setup_lst.src_addr = (uint32_t)rd_page_cmd_setup_data;
        rd_page_cmd_setup_lst.dst_addr = NAND_FLASH_CMD; 
        rd_page_cmd_setup_lst.len = sizeof(rd_page_cmd_setup_data);
    
        // Initialize the NAND controller configuration list (2 entries)
        rd_page_cfg_data_cfgn[0] = CONFIG_QC_NAND_NAND_DEVn_CFG0_VAL;
        rd_page_cfg_data_cfgn[1] = CONFIG_QC_NAND_NAND_DEVn_CFG1_VAL;
    
        rd_page_cfg_lst[0].first = ADM_ADDR_MODE_SI;
        rd_page_cfg_lst[0].src_addr = (uint32_t)rd_page_cfg_data_cfgn;
        rd_page_cfg_lst[0].dst_addr = NAND_DEV0_CFG0;
        rd_page_cfg_lst[0].len = sizeof(rd_page_cfg_data_cfgn);
    
        rd_page_cfg_data_ecc[0] = NAND_CODEWORD_SIZE - 1;  // ECC over just data area
    
        rd_page_cfg_lst[1].first = ADM_CMD_LIST_LC | ADM_ADDR_MODE_SI;
        rd_page_cfg_lst[1].src_addr = (uint32_t)rd_page_cfg_data_ecc;
        rd_page_cfg_lst[1].dst_addr = EBI2_ECC_BUF_CFG;
        rd_page_cfg_lst[1].len = sizeof(rd_page_cfg_data_ecc);
    
        // Initialize the execute list
        rd_page_execute_data[0] = NANDC_EXEC_CMD__EXEC_CMD__EXECUTE_THE_COMMAND;
    
        rd_page_execute_lst.first = ADM_CMD_LIST_LC | ADM_ADDR_MODE_SI;
        rd_page_execute_lst.src_addr = (uint32_t)rd_page_execute_data;
        rd_page_execute_lst.dst_addr = NANDC_EXEC_CMD;
        rd_page_execute_lst.len = sizeof(rd_page_execute_data);
    
        // Depending on page size, there can be 1 (512) or 4 (2k) status and read data 
        // command lists to initialize.
        for (i=0; i < codewords_per_page; i++)
        {
            // Initialize the read status lists
            rd_page_status_lst[i].first = ADM_CMD_LIST_LC | (ADM_CRCI_NAND_DATA << 3) | ADM_ADDR_MODE_SI;
            rd_page_status_lst[i].src_addr = NAND_FLASH_STATUS;
            rd_page_status_lst[i].dst_addr = (uint32_t)&rd_page_status[i];
            rd_page_status_lst[i].len = sizeof(uint32_t);   
    
            // Initialize the read data buffer lists
            rd_page_read_data_lst[i].first = ADM_CMD_LIST_LC;
            rd_page_read_data_lst[i].src_addr = FLASH_BUFF0_ACC;
            // dst_addr is setup below (for each page)
            rd_page_read_data_lst[i].len = NAND_CODEWORD_SIZE;
    
            if (i == (codewords_per_page - 1))
            {
                // Last command list, unblock other channels
                rd_page_read_data_lst[i].first |= ADM_CMD_LIST_OCU;   
            }
        }
    
        // Build the ADM command pointer list
        cmd_ptr_list = nand_rd_page_adm_cmd_ptr_list;
        for (i=0; i < codewords_per_page; i++)
        {
           cmd_ptr_list[0] = ADM_CMD_PTR_CMD_LIST | ((uint32_t)&rd_page_cmd_setup_lst >> 3);
           cmd_ptr_list[1] = ADM_CMD_PTR_CMD_LIST | ((uint32_t)&rd_page_cfg_lst[0] >> 3);
           cmd_ptr_list[2] = ADM_CMD_PTR_CMD_LIST | ((uint32_t)&rd_page_execute_lst >> 3);
           cmd_ptr_list[3] = ADM_CMD_PTR_CMD_LIST | ((uint32_t)&rd_page_status_lst[i] >> 3);
           cmd_ptr_list[4] = ADM_CMD_PTR_CMD_LIST | ((uint32_t)&rd_page_read_data_lst[i] >> 3);
    
           if (i == (codewords_per_page - 1))
           {
              cmd_ptr_list[4] |= ADM_CMD_PTR_LP;
           }
           
           cmd_ptr_list += 5;
        }

        // Initialization done
        rd_page_struct_common_init = 1;
    }

    // ADM structure fields that contain the page address have to 
    // to be initialized every time.
    if (codewords_per_page == 1)
    {
        // 512 byte page
        rd_page_cmd_setup_data[1] = page << 8;              // ADDR0
        rd_page_cmd_setup_data[2] = (page >> 24) & 0xFF;    // ADDR1
    }
    else
    {
        // 2K byte page
        rd_page_cmd_setup_data[1] = page << 16;             // ADDR0
        rd_page_cmd_setup_data[2] = (page >> 16) & 0xFF;    // ADDR1
    }

    // ADM structure fields that contain the target buffer address have to 
    // be initialized every time.
    for (i=0; i < codewords_per_page; i++)
    {
        rd_page_read_data_lst[i].dst_addr = (uint32_t)(buf + (i * NAND_CODEWORD_SIZE));
    }

    // Start ADM transfer
    if (adm_start_transfer(ADM_AARM_NAND_CHN, nand_rd_page_adm_cmd_ptr_list) != 0)
    {
       return(-EIO); 
    }

#ifdef CONFIG_DCACHE
    // Invalidate cache so structures and buffers ADM updated can be seen.
    invalidate_dcache_range((uint32_t)&rd_page_status[0],
                            (uint32_t)&rd_page_status[0] + sizeof(rd_page_status));
    invalidate_dcache_range((uint32_t)buf, 
                            (uint32_t)buf + dev->subpagesize);
#endif

    // Check NAND status for errors
    for (i=0; i < codewords_per_page; i++)
    {
        if ((rd_page_status[i] & NAND_FLASH_STATUS__OP_ERR___M) != 0)
        {
            printf("NAND: read page received bad status 0x%x page 0x%x\n",rd_page_status[i],page);
            return(-EIO); 
        }
    }

    return 0;
}


static void qc_cmdfunc(struct mtd_info *mtd, unsigned command, int column, int page_addr)
{

  switch(command)
  {
  case NAND_CMD_READ0:
      break;
  case NAND_CMD_RESET:
      // READID does a reset
      break;
  case NAND_CMD_READID:
      // This command will do both a reset and read ID
      rd_id_flash_id_value[0] = 0;
      id_count = 0; // reset id count
      nand_read_id_dm(mtd);
      break;
  default:
      printf("\nUnsupported NAND command\n");
  }

  // Remember the last command.
  last_command = command;
}


static void qc_select_chip(struct mtd_info *mtd, int chip)
{


}


static uint8_t	qc_read_byte(struct mtd_info *mtd)
{
  uint8_t value;

  // The only thing that read_byte is used for is to get
  // the individual bytes from the NAND_CMD_READID function.
  // nand_read_id_dm puts the 4 byte id value in rd_id_flash_id_value[0]
  switch (id_count)
  {
  case 0:
    value = rd_id_flash_id_value[0] & 0xFF;             // MFG id 1st time (1st ID byte)
    break;
  case 1:
    value = (rd_id_flash_id_value[0] >> 8) & 0xFF;      // DEV id 1st time (2nd ID byte)
    break;
  case 2:
    value = (rd_id_flash_id_value[0] >> 16) & 0xFF;     // 3rd ID byte
    break;
  case 3:
    value = (rd_id_flash_id_value[0] >> 24) & 0xFF;     // PageSize, BlockSize, SpareSize, Org (4th ID byte)
    break;
  default:
    value = 0xFF;
  }

  id_count++;

  return(value);
}


static int	qc_block_bad(struct mtd_info *mtd, loff_t ofs, int getchip)
{
    struct nand_chip *dev = mtd->priv;
    int i;
    loff_t adjusted_ofs;

    if (bbt == NULL)
    {
      printf("NAND error: Bad block table pointer not initialized.\n");
      return(0);
    }

    // Convert ofs (byte) to a block offset and adjust the
    // offset to point to the start of the APPS partition.
    adjusted_ofs = ofs >> dev->phys_erase_shift;
    adjusted_ofs += apps_part_start_blk;

    // Search the bad block table that was created by the ARM9
    for (i=0; i<bbt->num_bad_blocks; i++)
    {
      if (adjusted_ofs == bbt->bad_block[i])
      {
        // Block was found in the list
        return(1);
      }
    }

    return(0);          
}


static int	qc_scan_bbt(struct mtd_info *mtd)
{
  return(1);          
}

int board_nand_init(struct nand_chip *nand)
{
    int i;
    uint32_t *value = (uint32_t *)SMEM_START;
    smem_flash_partition_tbl_t * parttbl = NULL;


    nand->options = 0;
    nand->select_chip = qc_select_chip;
    nand->cmdfunc = qc_cmdfunc;
    nand->read_byte = qc_read_byte;
    nand->scan_bbt = qc_scan_bbt;
    nand->block_bad = qc_block_bad;
    nand->ecc.mode = NAND_ECC_NONE; 

    // Get pointer to the bad block table in SMEM
    if (NULL == bbt)
    {
       // TODO: Change search in SMEM to smem_alloc call
       for (i=0; i<((256*1024)-1); i++)
       {
         if ((value[i] == SMEM_FLASH_BAD_BLOCK_TBL_MAGIC1) &&
             (value[i+1] == SMEM_FLASH_BAD_BLOCK_TBL_MAGIC2))
         {
           bbt = (smem_flash_bad_block_tbl_t *)&value[i];
           break;
         }
       }
    }

    if (NULL == bbt)
    {
        printf("NAND error: could not find bad block table\n");
        return(-1);
    }

    // Get pointer to the NAND partition table in SMEM 
    if (NULL == parttbl)
    {
       // TODO: Change search in SMEM to smem_alloc call
       for (i=0; i<((256*1024)-1); i++)
       {
         if ((value[i] == SMEM_FLASH_PARTITION_TBL_MAGIC1) && 
             (value[i+1] == SMEM_FLASH_PARTITION_TBL_MAGIC2))
         {
           parttbl = (smem_flash_partition_tbl_t *)&value[i];
           break;
         }
       }
    }

    if (NULL == parttbl)
    {
        printf("NAND error: could not find partition table\n");
        return(-1);
    }

    // Get and save information for the 0:APPS partition in the table.
    for (i=0; i<parttbl->num_partitions; i++)
    {
        if (strcmp(parttbl->partition[i].name, "0:APPS") == 0)
        {
            apps_part_start_blk = parttbl->partition[i].start_blk;
            apps_part_numblocks = parttbl->partition[i].numblocks;
        }
        
    }

    if (apps_part_start_blk == 0)
    {
        printf("NAND error: 0:APPS partition not found\n");
    }

	return 0;
}
#endif /*CONFIG_CMD_NAND*/
