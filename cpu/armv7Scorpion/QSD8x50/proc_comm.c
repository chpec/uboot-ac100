/*
 * Copyright (C) 2007-2008 Google, Inc.
 * Copyright (c) 2009, Code Aurora Forum. All rights reserved.
 * Author: Brian Swetland <swetland@google.com>
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <common.h>
#include <asm/arch/proc_comm.h>
#ifdef USE_PROC_COMM
void msm_proc_comm_reset_modem_now(void);
int msm_proc_comm(proc_comm_t *cmd_pkt);

void gpio_tlmm_config(uint32_t gpio, uint8_t func,
                      uint8_t dir, uint8_t pull,
		      uint8_t drvstr, uint32_t enable );

volatile proc_comm_t *app_proc_comm = (proc_comm_t *) PROC_COMM_BASE;
volatile proc_comm_t *modem_proc_comm = (proc_comm_t *) (PROC_COMM_BASE + sizeof(proc_comm_t));

static inline void notify_modem(void)
{
	DSB;
	//wait for all the previous data to be written then fire
	IO_WRITE32(A2M_INT6, 1);
}

/* This function always returns 0, unless future versions check for
 * modem crash.
 * In that case it might return error.
 * This is called very early, dont debug here.
 */
static int proc_comm_wait_for(uint32_t addr, uint32_t value)
{
	while (1) {

		if (IO_READ32(addr) == value){
			return 0;
		}
		udelay(5);
	}
}

void msm_proc_comm_reset_modem_now(void)
{
	//try again even if function returns error
	while(!proc_comm_wait_for_modem_ready());

	app_proc_comm->command 	= PROC_COMM_RESET_MODEM;
	app_proc_comm->data1   	= 0;
	app_proc_comm->data2	= 0;

	notify_modem();

	return;
}

int msm_proc_comm(proc_comm_t *cmd_pkt)
{
	int ret;
	if(!cmd_pkt) {
		return -1;
	}
	proc_comm_wait_for_modem_ready();

	app_proc_comm->status = PROC_COMM_INVALID_STATUS; //cleanup last cmd status
	app_proc_comm->data1 = cmd_pkt->data1;
	app_proc_comm->data2 = cmd_pkt->data2;
	app_proc_comm->command = cmd_pkt->command;

        debug("SET:app_proc_comm->command=%d\n", app_proc_comm->command);
	debug("SET:app_proc_comm->status=%d\n", app_proc_comm->status);
	debug("SET:app_proc_comm->data1=%d\n", app_proc_comm->data1);
	debug("SET:app_proc_comm->data2=%d\n", app_proc_comm->data2);
	notify_modem();
	debug("mNOTIFY:app_proc_comm->command=%d\n", app_proc_comm->command);
	debug("mNOTIFY:app_proc_comm->status=%d\n", app_proc_comm->status);
	debug("mNOTIFY:app_proc_comm->data1=%d\n", app_proc_comm->data1);
	debug("mNOTIFY:app_proc_comm->data2=%d\n", app_proc_comm->data2);

	proc_comm_wait_for((uint32_t) &(app_proc_comm->command),
			   PROC_COMM_CMD_DONE);

//cmd_pkt->command is not updated, cause it will always be PROC_COMM_CMD_DONE
//else msm_proc_comm would not return
	cmd_pkt->status = app_proc_comm->status;
	cmd_pkt->data1 = app_proc_comm->data1;
	cmd_pkt->data2 = app_proc_comm->data2;
	/*debug("DONE:app_proc_comm->command=%d\n", app_proc_comm->command);
	debug("DONE:app_proc_comm->status=%d\n", app_proc_comm->status);
	debug("DONE:app_proc_comm->data1=%d\n", app_proc_comm->data1);
	debug("DONE:app_proc_comm->data2=%d\n", app_proc_comm->data2);
	*/
	if (app_proc_comm->status == PROC_COMM_CMD_SUCCESS) {
		ret = 0;
	} else {
		ret = -1;
	}

	app_proc_comm->command = PROC_COMM_CMD_IDLE;
	return ret;
}

int proc_comm_wait_for_modem_ready()
{
	// returns 0 to indicate success
	// we return ~0 to indicate modem ready
	return	~proc_comm_wait_for((uint32_t) &(modem_proc_comm->status),
				    PROC_COMM_READY);
}

void proc_comm_init(void)
{
	app_proc_comm->command = PROC_COMM_CMD_IDLE;
	app_proc_comm->status = PROC_COMM_INVALID_STATUS;
}

//this function should always succeed.. keep retrying till done.
void gpio_tlmm_config(uint32_t gpio, uint8_t func,
			   uint8_t dir, uint8_t pull,
			   uint8_t drvstr, uint32_t enable )
{
    volatile proc_comm_t pc_pkt;
    while(1) {
        pc_pkt.command = PROC_COMM_RPC_GPIO_TLMM_CONFIG_EX;

        pc_pkt.data1 = GPIO_CFG(gpio, func, dir, pull, drvstr);
	//sets apps proc as owner
	pc_pkt.data2 = (enable == GPIO_ENABLE) ? GPIO_ENABLE : GPIO_DISABLE;
	debug("TRY:: GPIO_TLMM_CONFIG_EX: gpio=%d, config=0x%x\n",
	      gpio, pc_pkt.data1);
        msm_proc_comm((proc_comm_t *)&pc_pkt);

        if(PROC_COMM_CMD_SUCCESS != pc_pkt.status) {
            debug("Err:GPIO_TLMM_CONFIG_EX failed...retrying\n");
            printf("Reason: %d\n", pc_pkt.status);
        } else {
            debug("DONE:: GPIO_TLMM_CONFIG_EX: data1=0x%d, data2=0x%x\n",
	      pc_pkt.data1, pc_pkt.data2);
            break;
        }
      }

}

uint32_t proc_comm_get_clk_rate(uint32_t clk)
{
    volatile proc_comm_t pc_pkt;

        pc_pkt.command = PROC_COMM_CLKCTL_RPC_RATE;
            pc_pkt.data1 = clk;
            pc_pkt.data2 = 0;

        msm_proc_comm((proc_comm_t *)&pc_pkt);
        if(PROC_COMM_CMD_SUCCESS != pc_pkt.status) {
            printf("get_clk_rate(clk=%d)failed...not retrying\n", clk);
	    printf("Reason: %d\n", pc_pkt.status);
	    return 0;
        } else {
            debug("DONE:: get_clk_rate data1=%d,data2=%d,status=%d\n",
                   pc_pkt.data1, pc_pkt.data2, pc_pkt.status);

	    return pc_pkt.data1;
	}

}

uint32_t proc_comm_set_clk_rate(uint32_t clk, uint32_t rate)
{
    volatile proc_comm_t pc_pkt;
    pc_pkt.command = PROC_COMM_CLKCTL_RPC_SET_RATE;
    pc_pkt.data1 = clk;
    pc_pkt.data2 = rate;

        msm_proc_comm((proc_comm_t *)&pc_pkt);
        if(PROC_COMM_CMD_SUCCESS != pc_pkt.status) {
            printf("set_clk_rate(clk=%d)failed... not retrying\n", clk);
	    printf("Reason: %d\n", pc_pkt.status);
	    return 1;
        } else {
            debug("DONE:: set_clk_rate data1=%d, data2=%d, status=%d\n",
                   pc_pkt.data1, pc_pkt.data2, pc_pkt.status);
	    return 0;
        }
}

uint32_t proc_comm_set_clk_flags(uint32_t clk, uint32_t flags)
{
    volatile proc_comm_t pc_pkt;
    pc_pkt.command = PROC_COMM_CLKCTL_RPC_SET_FLAGS;
    pc_pkt.data1 = clk;
    pc_pkt.data2 = flags;

        msm_proc_comm((proc_comm_t *)&pc_pkt);
        if(PROC_COMM_CMD_SUCCESS != pc_pkt.status) {
            printf("set_clk_flags(clk=%d)failed... not retrying\n", clk);
	    printf("Reason: %d\n", pc_pkt.status);
	    return 1;
        } else {
            debug("DONE:: set_clk_flags data1=%d, data2=%d, status=%d\n",
                   pc_pkt.data1, pc_pkt.data2, pc_pkt.status);
	    return 0;
        }
}

uint32_t proc_comm_clk_enable(uint32_t clk)
{
    volatile proc_comm_t pc_pkt;

        pc_pkt.command = PROC_COMM_CLKCTL_RPC_ENABLE;
            pc_pkt.data1 = clk;
            pc_pkt.data2 = 0;

        msm_proc_comm((proc_comm_t *)&pc_pkt);
        if(PROC_COMM_CMD_SUCCESS != pc_pkt.status) {
            printf("clk_enable(clk=%d)failed...not retrying\n", clk);
	    printf("Reason: %d\n", pc_pkt.status);
	    return 0;
        } else {
            debug("DONE:: clk_enable data1=%d,data2=%d,status=%d\n",
                   pc_pkt.data1, pc_pkt.data2, pc_pkt.status);

	    return pc_pkt.data1;
	}

}

uint32_t proc_comm_clk_disable(uint32_t clk)
{
    volatile proc_comm_t pc_pkt;

        pc_pkt.command = PROC_COMM_CLKCTL_RPC_DISABLE;
            pc_pkt.data1 = clk;
            pc_pkt.data2 = 0;

        msm_proc_comm((proc_comm_t *)&pc_pkt);
        if(PROC_COMM_CMD_SUCCESS != pc_pkt.status) {
            printf("clk_disable(clk=%d)failed...not retrying\n", clk);
	    printf("Reason: %d\n", pc_pkt.status);
	    return 0;
        } else {
            debug("DONE:: clk_disable data1=%d,data2=%d,status=%d\n",
                   pc_pkt.data1, pc_pkt.data2, pc_pkt.status);

	    return pc_pkt.data1;
	}

}

uint32_t proc_comm_is_clk_enabled(uint32_t clk)
{
    volatile proc_comm_t pc_pkt;

        pc_pkt.command = PROC_COMM_CLKCTL_RPC_ENABLED;
            pc_pkt.data1 = clk;
            pc_pkt.data2 = 0;

        msm_proc_comm((proc_comm_t *)&pc_pkt);
        if(PROC_COMM_CMD_SUCCESS != pc_pkt.status) {
            printf("get_clk_rate(clk=%d)failed...not retrying\n", clk);
	    printf("Reason: %d\n", pc_pkt.status);
	    return 0;
        } else {
            debug("DONE:: is_clk_enabled data1=%d,data2=%d,status=%d\n",
                   pc_pkt.data1, pc_pkt.data2, pc_pkt.status);

	    return pc_pkt.data1;
	}

}

#endif /*USE_PROC_COMM*/
