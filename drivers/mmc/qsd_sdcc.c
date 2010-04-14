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

#include <config.h>
#include <common.h>
#include <command.h>
#include <mmc.h>
#include <part.h>
#include <malloc.h>
#include <qsd_sdcc.h>
#include <asm/arch/adm.h>

#ifdef USE_PROC_COMM
#include <asm/arch/proc_comm_clients.h>
#include <asm/arch/proc_comm.h>
#endif /*USE_PROC_COMM */

#ifdef SDCC_CMD_DEBUG
#define sdcc_cmd_debug(fmt,args...) printf(fmt, ##args)
#else
#define sdcc_cmd_debug(fmt,args...)
#endif

#ifdef SDCC_DEBUG
#define sdcc_debug(fmt,args...) printf(fmt, ##args)
#else
#define sdcc_debug(fmt,args...)
#endif

/* clear the status bit. */
#define MCI_CLEAR_STATUS(base, mask) do{ IO_WRITE32(base + MCI_CLEAR, mask); } while(IO_READ32(base + MCI_STATUS) & mask);

static unsigned char sdcc_use_dm = 0;

/* Structures for use with ADM */
static uint32_t sd_adm_cmd_ptr_list[8] __attribute__ ((aligned(8))); /*  Must aligned on 8 byte boundary */
static uint32_t sd_box_mode_entry[8]   __attribute__ ((aligned(8))); /*  Must aligned on 8 byte boundary */

/* Any one of these will be set when command processing is complete. */
#define MCI_STATUS_CMD_COMPLETE_MASK        (MCI_STATUS__CMD_SENT___M | \
                                             MCI_STATUS__CMD_RESPONSE_END___M | \
                                             MCI_STATUS__CMD_TIMEOUT___M | \
                                             MCI_STATUS__CMD_CRC_FAIL___M)

/* Any one of these will be set during data read processing. */
#define MCI_STATUS_DATA_READ_COMPLETE_MASK  (MCI_STATUS_BLK_END_MASK | \
                                             MCI_STATUS_DATA_RD_ERR_MASK)

#define MCI_STATUS_DATA_RD_ERR_MASK         (MCI_STATUS__DATA_CRC_FAIL___M | \
                                             MCI_STATUS__DATA_TIMEOUT___M  | \
                                             MCI_STATUS__RX_OVERRUN___M    | \
                                             MCI_STATUS__START_BIT_ERR___M)

#define MCI_STATUS_BLK_END_MASK             (MCI_STATUS__DATA_BLK_END___M |  \
                                             MCI_STATUS__DATAEND___M)

#define MCI_STATUS_READ_DATA_MASK           (MCI_STATUS__RXDATA_AVLBL___M|  \
                                             MCI_STATUS__RXACTIVE___M)


/* verify if data read was successful and clear up status bits */
int sdcc_read_data_cleanup(struct mmc *mmc)
{
    int err = 0;
    uint32_t base   = ((sdcc_params_t*)(mmc->priv))->base;
    uint32_t status = IO_READ32(base + MCI_STATUS);

    if(status & MCI_STATUS__RX_OVERRUN___M)
    {
        err = SDCC_ERR_RX_OVERRUN;
    }
    else if (status & MCI_STATUS__DATA_TIMEOUT___M)
    {
        err = SDCC_ERR_DATA_TIMEOUT;
    }
    else if (status & MCI_STATUS__DATA_CRC_FAIL___M)
    {
        err = SDCC_ERR_DATA_CRC_FAIL;
    }
    else if (status & MCI_STATUS__START_BIT_ERR___M )
    {
        err = SDCC_ERR_DATA_START_BIT;
    }
    else
    {
        /* Wait for the blk status bits to be set. */
        while (!(IO_READ32(base + MCI_STATUS) & MCI_STATUS__DATAEND___M));
    }

    /* Clear the status bits. */
    MCI_CLEAR_STATUS(base, MCI_STATUS_DATA_READ_COMPLETE_MASK);

    return err;
}

/* Read data from controller fifo */
int sdcc_read_data(struct mmc *mmc, struct mmc_cmd *cmd, struct mmc_data *data)
{
    uint32_t status;
    uint16_t byte_count   = 0;
    uint32_t *dest_ptr    = (uint32_t *)(data->dest);
    uint32_t base         = ((sdcc_params_t*)(mmc->priv))->base;
    uint32_t adm_crci_num = ((sdcc_params_t*)(mmc->priv))->adm_crci_num; /*  ADM CRCI number */

    if(sdcc_use_dm)
    {
        uint32_t num_rows;
        uint32_t addr_shft;
        uint32_t rows_per_block;
        uint16_t row_len;

        row_len = SDCC_FIFO_SIZE;
        rows_per_block = (data->blocksize / SDCC_FIFO_SIZE);
        num_rows = rows_per_block * data->blocks;

        /*  Initialize the DM Box mode command entry (single entry) */
        sd_box_mode_entry[0] = (ADM_CMD_LIST_LC | (adm_crci_num << 3) | ADM_ADDR_MODE_BOX);
        sd_box_mode_entry[1] = base + MCI_FIFO;                      /*  SRC addr */
        sd_box_mode_entry[2] = (uint32_t) data->dest;                /*  DST addr */
        sd_box_mode_entry[3] = ((row_len << 16) | (row_len << 0));   /*  SRC/DST row len */
        sd_box_mode_entry[4] = ((num_rows << 16) | (num_rows << 0)); /*  SRC/DST num rows */
        sd_box_mode_entry[5] = ((0 << 16) | (SDCC_FIFO_SIZE << 0));  /*  SRC/DST offset */

        /*  Initialize the DM Command Pointer List (single entry) */
        addr_shft = ((uint32_t)(&sd_box_mode_entry[0])) >> 3;
        sd_adm_cmd_ptr_list[0] = (ADM_CMD_PTR_LP | ADM_CMD_PTR_CMD_LIST | addr_shft);

        /*  Start ADM transfer */
        if (adm_start_transfer(ADM_AARM_SD_CHN, sd_adm_cmd_ptr_list) != 0)
        {
           return SDCC_ERR_DATA_ADM_ERR;
        }

#ifdef CONFIG_DCACHE
        /* Invalidate cache so buffer ADM updated can be seen. */
        invalidate_dcache_range((uint32_t)data->dest,
                                (uint32_t)data->dest + (data->blocks * data->blocksize));
#endif
    }
    else
    {
        int read_len = (data->blocks) * (data->blocksize);

        while((status = IO_READ32(base + MCI_STATUS)) & MCI_STATUS_READ_DATA_MASK)
        {
            /* rx data available bit is not cleared immidiately.
             * read only the requested amount of data and wait for
             * the bit to be cleared.
             */
            if(byte_count < read_len)
            {
                if(status & MCI_STATUS__RXDATA_AVLBL___M)
                {
                    *dest_ptr = IO_READ32(base + MCI_FIFO);
                    dest_ptr++;
                    byte_count += 4;
                }
            }
        }
    }
    return sdcc_read_data_cleanup(mmc);
}

/* Set SD MCLK speed */
static int sdcc_mclk_set(int instance, enum SD_MCLK_speed speed)
{
#ifndef USE_PROC_COMM
    #error "use proc_comm"
#else
    proc_comm_set_sdcard_clk(instance, speed);
    proc_comm_enable_sdcard_clk(instance);
#endif /*USE_PROC_COMM*/

   return 0;
}

/* Set bus width and bus clock speed */
void sdcc_set_ios(struct mmc *mmc)
{
    uint32_t clk_reg;
    uint32_t base     = ((sdcc_params_t*)(mmc->priv))->base;
    uint32_t instance = ((sdcc_params_t*)(mmc->priv))->instance;

    if(mmc->bus_width == 1)
    {
        clk_reg = IO_READ32(base + MCI_CLK) & ~MCI_CLK__WIDEBUS___M;
        IO_WRITE32(base + MCI_CLK, (clk_reg | (BUS_WIDTH_1 << MCI_CLK__WIDEBUS___S)));
    }
    else if(mmc->bus_width == 4)
    {
        clk_reg = IO_READ32(base + MCI_CLK) & ~MCI_CLK__WIDEBUS___M;
        IO_WRITE32(base + MCI_CLK, (clk_reg | (BUS_WIDTH_4 << MCI_CLK__WIDEBUS___S)));
    }
    else if(mmc->bus_width == 8)
    {
        clk_reg = IO_READ32(base + MCI_CLK) & ~MCI_CLK__WIDEBUS___M;
        IO_WRITE32(base + MCI_CLK, (clk_reg | (BUS_WIDTH_8 << MCI_CLK__WIDEBUS___S)));
    }

    if(mmc->clock == MCLK_400KHz)
    {
        uint32_t temp32;

        sdcc_mclk_set(instance, MCLK_400KHz);

        /* Latch data on falling edge */
        temp32 = IO_READ32(base + MCI_CLK) & ~MCI_CLK__SELECT_IN___M;
        temp32 |= (MCI_CLK__SELECT_IN__ON_THE_FALLING_EDGE_OF_MCICLOCK << MCI_CLK__SELECT_IN___S);
        IO_WRITE32(base + MCI_CLK, temp32);
    }
    else if(mmc->clock == MCLK_48MHz)
    {
        uint32_t temp32;

        sdcc_mclk_set(instance, MCLK_48MHz);

        /* Card is in high speed mode, use feedback clock. */
        temp32 = IO_READ32(base + MCI_CLK);
        temp32 &= ~(MCI_CLK__SELECT_IN___M);
        temp32 |= (MCI_CLK__SELECT_IN__USING_FEEDBACK_CLOCK << MCI_CLK__SELECT_IN___S);
        IO_WRITE32(base + MCI_CLK, temp32);
    }
}

/* Program the data control registers */
void sdcc_start_data(struct mmc *mmc, struct mmc_cmd *cmd, struct mmc_data *data)
{
    uint32_t data_ctrl;
    uint32_t base = ((sdcc_params_t*)(mmc->priv))->base;
    uint32_t xfer_size = data->blocksize * data->blocks;;

    data_ctrl = MCI_DATA_CTL__ENABLE___M | (data->blocksize << MCI_DATA_CTL__BLOCKSIZE___S);

    if ( (xfer_size < SDCC_FIFO_SIZE) ||
         (xfer_size % SDCC_FIFO_SIZE)
         )
    {
        /* can't use data mover. */
        sdcc_use_dm = 0;
    }
    else
    {
        sdcc_use_dm = 1;
        data_ctrl |= MCI_DATA_CTL__DM_ENABLE___M;
    }

    if(data->flags == MMC_DATA_READ)
    {
        data_ctrl |= MCI_DATA_CTL__DIRECTION___M;
    }

    /*  Set timeout and data length */
    IO_WRITE32(base + MCI_DATA_TIMER,  RD_DATA_TIMEOUT);
    IO_WRITE32(base + MCI_DATA_LENGTH, xfer_size);

    IO_WRITE32(base + MCI_DATA_CTL, data_ctrl);
}

/* Set proper bit for the cmd register value */
static void sdcc_get_cmd_reg_value(struct mmc_cmd *cmd, struct mmc_data *data, uint16_t *creg)
{
    *creg = cmd->cmdidx | MCI_CMD__ENABLE___M;

    if(cmd->resp_type & MMC_RSP_PRESENT)
    {
        *creg |= MCI_CMD__RESPONSE___M;

        if(cmd->resp_type & MMC_RSP_136)
        {
            *creg |= MCI_CMD__LONGRSP___M;
        }
    }
    if(cmd->resp_type & MMC_RSP_BUSY)
    {
        *creg |= MCI_CMD__PROG_ENA___M;
    }

    /* Set the DAT_CMD bit for data commands */
    if( (cmd->cmdidx == CMD17) ||
        (cmd->cmdidx == CMD18) ||
        (cmd->cmdidx == CMD24) ||
        (cmd->cmdidx == CMD25) ||
        (cmd->cmdidx == CMD53)
      )
    {
        /* We don't need to set this bit for switch function (6) and
         * read scr (51) commands in SD case.
         */
        *creg |= MCI_CMD__DAT_CMD___M;
    }

    if(cmd->resp_type & MMC_RSP_OPCODE)
    {
        /* Don't know what is this for. Seems to work fine
         * without doing anything for this response type.
         */
    }
}

/* Program the cmd registers */
void sdcc_start_command(struct mmc *mmc, struct mmc_cmd *cmd, struct mmc_data *data)
{
    uint16_t creg;
    uint32_t base = ((sdcc_params_t*)(mmc->priv))->base;

    sdcc_get_cmd_reg_value(cmd, data, &creg);

    IO_WRITE32(base + MCI_ARGUMENT, cmd->cmdarg);
    IO_WRITE32(base + MCI_CMD, creg);
}

/* Send command as well as send/receive data */
int sdcc_send_cmd(struct mmc *mmc, struct mmc_cmd *cmd, struct mmc_data *data)
{
    int err = 0;
    uint32_t status;
    uint32_t base = ((sdcc_params_t*)(mmc->priv))->base;

    if((status = IO_READ32(base + MCI_STATUS)))
    {
        /* Some implementation error. */
        /* must always start with all status bits cleared. */
        sdcc_debug("\n Invalid status while entering: status = %x, cmd = %d", status, cmd->cmdidx);
        return SDCC_ERR_INVALID_STATUS;
    }

    if(data)
    {
        if(data->flags == MMC_DATA_WRITE)
        {
            /* write not yet implemented. */
            return SDCC_ERR_DATA_WRITE;
        }

        sdcc_start_data(mmc, cmd, data);
        sdcc_start_command(mmc, cmd, data);
    }
    else
    {
        sdcc_start_command(mmc, cmd, data);
    }


    /* Wait for cmd completion (any response included). */
    while( !((status = IO_READ32(base + MCI_STATUS)) & MCI_STATUS_CMD_COMPLETE_MASK) );

    /* Read response registers. */
    cmd->response[0] = IO_READ32(base + MCI_RESPn(0));
    cmd->response[1] = IO_READ32(base + MCI_RESPn(1));
    cmd->response[2] = IO_READ32(base + MCI_RESPn(2));
    cmd->response[3] = IO_READ32(base + MCI_RESPn(3));

    if(status & MCI_STATUS__CMD_CRC_FAIL___M)
    {
        if(cmd->resp_type & MMC_RSP_CRC)
        {
            /* failure. */
            err = SDCC_ERR_CRC_FAIL;
        }

        /* clear status bit. */
        MCI_CLEAR_STATUS(base, MCI_STATUS__CMD_CRC_FAIL___M);
    }
    else if(status & MCI_STATUS__CMD_TIMEOUT___M)
    {
        /* failure. */
        err = SDCC_ERR_TIMEOUT;

        /* clear timeout status bit. */
        MCI_CLEAR_STATUS(base, MCI_STATUS__CMD_TIMEOUT___M);
    }
    else if(status & MCI_STATUS__CMD_SENT___M)
    {
        /* clear CMD_SENT status bit. */
        MCI_CLEAR_STATUS(base, MCI_STATUS__CMD_SENT___M);
    }
    else if(status & MCI_STATUS__CMD_RESPONSE_END___M)
    {
        /* clear CMD_RESP_END status bit. */
        MCI_CLEAR_STATUS(base, MCI_STATUS__CMD_RESPONSE_END___M);
    }

    if(cmd->resp_type & MMC_RSP_BUSY)
    {
        /* Must wait for PROG_DONE status to be set before returning. */
        while( !(IO_READ32(base + MCI_STATUS) & MCI_STATUS__PROG_DONE___M) );

        /* now clear that bit. */
        MCI_CLEAR_STATUS(base, MCI_CLEAR__PROG_DONE_CLR___M);
    }

    /* if this was one of the data read/write cmds, handle the data. */
    if(data)
    {
        if(err)
        {
            /* there was some error while sending the cmd. cancel the data operation. */
            IO_WRITE32(base + MCI_DATA_CTL, 0x0);
        }
        else
        {
            if(data->flags == MMC_DATA_READ)
            {
                err = sdcc_read_data(mmc, cmd, data);
            }
            else
            {
                /* write data is not implemented.
                 * placeholder for future update.
                 */
                while(1);
            }
        }
    }

    /* read status bits to verify any condition that was not handled.
     * SDIO_INTR bit is set on D0 line status and is valid only in
     * case of SDIO interface. So it can be safely ignored.
     */
    if(IO_READ32(base + MCI_STATUS) & MCI_STATUS__SDIO_INTR___M)
    {
        MCI_CLEAR_STATUS(base, MCI_CLEAR__SDIO_INTR_CLR___M);
    }

    if((status = IO_READ32(base + MCI_STATUS)))
    {
        /* Some implementation error. */
        /* must always exit with all status bits cleared. */
        sdcc_debug("\n Invalid status while exiting: status = %x, cmd = %d", status, cmd->cmdidx);
        return SDCC_ERR_INVALID_STATUS;
    }

    sdcc_cmd_debug("\nsdcc_send_cmd: cmd = %d response = 0x%x", cmd->cmdidx, cmd->response[0]);

    if(err)
    {
        sdcc_debug("\nsdcc_send_cmd: err = %d, cmd = %d", err, cmd->cmdidx);
        if(data)
            sdcc_debug(", blocksize = %d\n", data->blocksize);
        else
            sdcc_debug("\n");
    }
    return err;
}

/* Initialize sd controller block */
void sdcc_controller_init(sdcc_params_t *sd)
{
    uint32_t mci_clk;

    /*  Disable all interrupts sources */
    IO_WRITE32(sd->base + MCI_INT_MASK0, 0x0);
    IO_WRITE32(sd->base + MCI_INT_MASK1, 0x0);

    /*  Clear all status bits */
    IO_WRITE32(sd->base + MCI_CLEAR, 0x07FFFFFF);

    /*  Power control to the card, enable MCICLK with power save mode */
    /*  disabled, otherwise the initialization clock cycles will be */
    /*  shut off and the card will not initialize. */
    IO_WRITE32(sd->base + MCI_POWER, MCI_POWER__CONTROL__POWERON);

    mci_clk = MCI_CLK__FLOW_ENA___M |
              (MCI_CLK__SELECT_IN__ON_THE_FALLING_EDGE_OF_MCICLOCK << MCI_CLK__SELECT_IN___S) |
              MCI_CLK__ENABLE___M;

    IO_WRITE32(sd->base + MCI_CLK, mci_clk);
}

/* Called during each scan of mmc devices */
int sdcc_init(struct mmc *mmc)
{
    sdcc_params_t *sd = (sdcc_params_t*)(mmc->priv);

    if(board_sdcc_init(sd))
    {
        /* error */
        return SDCC_ERR_GENERIC;
    }

    /* Enable clock */
    proc_comm_enable_sdcard_pclk(sd->instance);

    /* Set the interface clock */
    proc_comm_set_sdcard_clk(sd->instance, MCLK_400KHz);
    proc_comm_enable_sdcard_clk(sd->instance);


    /* Initialize controller */
    sdcc_controller_init(sd);

    return 0;
}
