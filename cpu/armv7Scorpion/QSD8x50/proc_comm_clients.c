/*
 * Copyright (c) 2007-2010, Code Aurora Forum. All rights reserved.
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

#ifdef USE_PROC_COMM

#include <asm/arch/proc_comm.h>

extern int msm_proc_comm(proc_comm_t *cmd_pkt);

extern void gpio_tlmm_config(uint32_t gpio, uint8_t func,
	                     uint8_t dir, uint8_t pull,
			     uint8_t drvstr, uint32_t enable);
extern uint32_t proc_comm_get_clk_rate(uint32_t clk);
extern uint32_t proc_comm_set_clk_rate(uint32_t clk, uint32_t rate);
extern uint32_t proc_comm_set_clk_flags(uint32_t clk, uint32_t flags);
extern uint32_t proc_comm_clk_enable(uint32_t clk);
extern uint32_t proc_comm_clk_disable(uint32_t clk);
extern uint32_t proc_comm_is_clk_enabled(uint32_t clk);

void proc_comm_vreg_control(int vreg, int level, int state)
{
    proc_comm_t pc_pkt;

    /* If turning it ON, set the level first. */
    if(state)
    {
        pc_pkt.command = PROC_COMM_VREG_SET_LEVEL;
        pc_pkt.data1   = vreg;
        pc_pkt.data2   = level;
        pc_pkt.status  = PROC_COMM_INVALID_STATUS;

        do
        {
            msm_proc_comm((proc_comm_t *)&pc_pkt);
        }while(PROC_COMM_CMD_SUCCESS != pc_pkt.status);
    }


    pc_pkt.command = PROC_COMM_VREG_SWITCH;
    pc_pkt.data1 = vreg;
    pc_pkt.data2 = state ? PROC_COMM_ENABLE : PROC_COMM_DISABLE;
    pc_pkt.status  = PROC_COMM_INVALID_STATUS;

    do{
        msm_proc_comm((proc_comm_t *)&pc_pkt);
    }while(PROC_COMM_CMD_SUCCESS != pc_pkt.status);
}

#ifndef CONFIG_GENERIC_MMC
void proc_comm_sdcard_power(int state)
{
    volatile proc_comm_t pc_pkt;
    while(1) {
        pc_pkt.command = PROC_COMM_VREG_SWITCH;
        pc_pkt.data1 = PROC_COMM_VREG_SDC;
        pc_pkt.data2 = state ? PROC_COMM_ENABLE : PROC_COMM_DISABLE;

        msm_proc_comm((proc_comm_t *)&pc_pkt);
        debug("PROC_COMM_VREG_SWITCH TRY\n");
        if(PROC_COMM_CMD_SUCCESS != pc_pkt.status) {
            debug("Error: PROC_COMM_VREG_SWITCH failed...retrying\n");
        } else {
            debug("PROC_COMM_VREG_SWITCH DONE\n");
            break;
        }
      }

}
#endif

void proc_comm_sdcard_gpio_config(int instance)
{

//Note: GPIO_NO_PULL is for clock lines.
switch (instance) {
case 1:
        /* Some cards had crc erorrs on multiblock reads.
         * Increasing drive strength from 8 to 16 fixed that.
         */
	gpio_tlmm_config(51, 1, GPIO_OUTPUT, GPIO_PULL_UP,
			 GPIO_16MA, GPIO_ENABLE);
	gpio_tlmm_config(52, 1, GPIO_OUTPUT, GPIO_PULL_UP,
			 GPIO_16MA, GPIO_ENABLE);
	gpio_tlmm_config(53, 1, GPIO_OUTPUT, GPIO_PULL_UP,
			 GPIO_16MA, GPIO_ENABLE);
	gpio_tlmm_config(54, 1, GPIO_OUTPUT, GPIO_PULL_UP,
			 GPIO_16MA, GPIO_ENABLE);
	gpio_tlmm_config(55, 1, GPIO_OUTPUT, GPIO_PULL_UP,
			 GPIO_16MA, GPIO_ENABLE);
	gpio_tlmm_config(56, 1, GPIO_OUTPUT, GPIO_NO_PULL,
			 GPIO_16MA, GPIO_ENABLE);
	break;

case 2:
	gpio_tlmm_config(62, 1, GPIO_OUTPUT, GPIO_NO_PULL,
		         GPIO_8MA, GPIO_ENABLE);
	gpio_tlmm_config(63, 1, GPIO_OUTPUT, GPIO_PULL_UP,
		         GPIO_8MA, GPIO_ENABLE);
	gpio_tlmm_config(64, 1, GPIO_OUTPUT, GPIO_PULL_UP,
		         GPIO_8MA, GPIO_ENABLE);
	gpio_tlmm_config(65, 1, GPIO_OUTPUT, GPIO_PULL_UP,
		         GPIO_8MA, GPIO_ENABLE);
	gpio_tlmm_config(66, 1, GPIO_OUTPUT, GPIO_PULL_UP,
		         GPIO_8MA, GPIO_ENABLE);
	gpio_tlmm_config(67, 1, GPIO_OUTPUT, GPIO_PULL_UP,
		         GPIO_8MA, GPIO_ENABLE);
	break;

case 3:
	gpio_tlmm_config(88, 1, GPIO_OUTPUT, GPIO_NO_PULL, GPIO_8MA,
			GPIO_ENABLE);
        gpio_tlmm_config(89, 1, GPIO_OUTPUT, GPIO_PULL_UP, GPIO_8MA,
			GPIO_ENABLE);
        gpio_tlmm_config(90, 1, GPIO_OUTPUT, GPIO_PULL_UP, GPIO_8MA,
			GPIO_ENABLE);
        gpio_tlmm_config(91, 1, GPIO_OUTPUT, GPIO_PULL_UP, GPIO_8MA,
			GPIO_ENABLE);
        gpio_tlmm_config(92, 1, GPIO_OUTPUT, GPIO_PULL_UP, GPIO_8MA,
			GPIO_ENABLE);
        gpio_tlmm_config(93, 1, GPIO_OUTPUT, GPIO_PULL_UP, GPIO_8MA,
			GPIO_ENABLE);
        gpio_tlmm_config(158, 1, GPIO_OUTPUT, GPIO_PULL_UP, GPIO_8MA,
			GPIO_ENABLE);
        gpio_tlmm_config(159, 1, GPIO_OUTPUT, GPIO_PULL_UP, GPIO_8MA,
			GPIO_ENABLE);
        gpio_tlmm_config(160, 1, GPIO_OUTPUT, GPIO_PULL_UP, GPIO_8MA,
			GPIO_ENABLE);
        gpio_tlmm_config(161, 1, GPIO_OUTPUT, GPIO_PULL_UP, GPIO_8MA,
			GPIO_ENABLE);
	break;

case 4:
	gpio_tlmm_config(142, 3, GPIO_OUTPUT, GPIO_NO_PULL,
		         GPIO_8MA, GPIO_ENABLE);
	gpio_tlmm_config(143, 3, GPIO_OUTPUT, GPIO_PULL_UP,
		         GPIO_8MA, GPIO_ENABLE);
	gpio_tlmm_config(144, 2, GPIO_OUTPUT, GPIO_PULL_UP,
		         GPIO_8MA, GPIO_ENABLE);
	gpio_tlmm_config(145, 2, GPIO_OUTPUT, GPIO_PULL_UP,
		         GPIO_8MA, GPIO_ENABLE);
	gpio_tlmm_config(146, 3, GPIO_OUTPUT, GPIO_PULL_UP,
			 GPIO_8MA, GPIO_ENABLE);
	gpio_tlmm_config(147, 3, GPIO_OUTPUT, GPIO_PULL_UP,
			 GPIO_8MA, GPIO_ENABLE);
	break;


    } /* switch (instance) */

}

//---- USB HS related proc_comm clients

void proc_comm_usb_vbus_power(int state)
{
    volatile proc_comm_t pc_pkt;
    pc_pkt.command = PROC_COMM_PM_MPP_CONFIG;
    pc_pkt.data1 = PROC_COMM_MPP_FOR_USB_VBUS;

    pc_pkt.data2 = (PM_MPP__DLOGIC__LVL_VDD << 16) |
                    (state ? PM_MPP__DLOGIC_OUT__CTRL_HIGH :
                             PM_MPP__DLOGIC_OUT__CTRL_LOW);
    msm_proc_comm((proc_comm_t *)&pc_pkt);
    debug("PROC_COMM_MPP_CONFIG TRY\n");
    if(PROC_COMM_CMD_SUCCESS != pc_pkt.status) {
        printf("Error: PROC_COMM_MPP_CONFIG failed... not retrying\n");
    } else {
        debug("PROC_COMM_MPP_CONFIG DONE\n");
    }
}

void proc_comm_usb_reset_phy(void)
{
    volatile proc_comm_t pc_pkt;
    while(1) {

        pc_pkt.command = PROC_COMM_MSM_HSUSB_PHY_RESET;
        pc_pkt.data1 = 0; //parameters are ignored
        pc_pkt.data2 = 0;

        msm_proc_comm((proc_comm_t *)&pc_pkt);
        debug("PROC_COMM_MSM_HSUSB_PHY_RESET TRY\n");
        if(PROC_COMM_CMD_SUCCESS != pc_pkt.status) {
            printf("Error: PROC_COMM_MSM_HSUSB_PHY_RESET failed...not retrying\n");
	    break; // remove to retry
        } else {
            debug("PROC_COMM_MSM_HSUSB_PHY_RESET DONE\n");
            break;
        }
    }
}

void proc_comm_enable_hsusb_clk(void)
{
    proc_comm_clk_enable(PROC_COMM_USB_HS_CLK);
}

void proc_comm_disable_hsusb_clk(void)
{
    proc_comm_clk_disable(PROC_COMM_USB_HS_CLK);
}

//---- SD card related proc_comm clients
void proc_comm_set_sdcard_clk_flags(int instance, int flags)
{
	switch(instance)
	{

	case 1:
		proc_comm_set_clk_flags(PROC_COMM_SDC1_CLK, flags);
		break;
	case 2:
		proc_comm_set_clk_flags(PROC_COMM_SDC2_CLK, flags);
		break;
	case 3:
		proc_comm_set_clk_flags(PROC_COMM_SDC3_CLK, flags);
		break;
	case 4:
		proc_comm_set_clk_flags(PROC_COMM_SDC4_CLK, flags);
		break;
	}
}

void proc_comm_set_sdcard_clk(int instance, int rate)
{
	switch(instance)
	{

	case 1:
		proc_comm_set_clk_rate(PROC_COMM_SDC1_CLK, rate);
		break;
	case 2:
		proc_comm_set_clk_rate(PROC_COMM_SDC2_CLK, rate);
		break;
	case 3:
		proc_comm_set_clk_rate(PROC_COMM_SDC3_CLK, rate);
		break;
	case 4:
		proc_comm_set_clk_rate(PROC_COMM_SDC4_CLK, rate);
		break;
	}
}

uint32_t proc_comm_get_sdcard_clk(int instance)
{
	uint32_t rate = 0;
	switch(instance)
	{

	case 1:
		rate = proc_comm_get_clk_rate(PROC_COMM_SDC1_CLK);
		break;
	case 2:
		rate = proc_comm_get_clk_rate(PROC_COMM_SDC2_CLK);
		break;
	case 3:
		rate = proc_comm_get_clk_rate(PROC_COMM_SDC3_CLK);
		break;
	case 4:
		rate = proc_comm_get_clk_rate(PROC_COMM_SDC4_CLK);
		break;
	}
	return rate;
}


void proc_comm_enable_sdcard_clk(int instance)
{
	switch(instance)
	{

	case 1:
		proc_comm_clk_enable(PROC_COMM_SDC1_CLK);
		break;
	case 2:
		proc_comm_clk_enable(PROC_COMM_SDC2_CLK);
		break;
	case 3:
		proc_comm_clk_enable(PROC_COMM_SDC3_CLK);
		break;
	case 4:
		proc_comm_clk_enable(PROC_COMM_SDC4_CLK);
		break;
	}

}

void proc_comm_disable_sdcard_clk(int instance)
{
	switch(instance)
	{

	case 1:
		proc_comm_clk_disable(PROC_COMM_SDC1_CLK);
		break;
	case 2:
		proc_comm_clk_disable(PROC_COMM_SDC2_CLK);
		break;
	case 3:
		proc_comm_clk_disable(PROC_COMM_SDC3_CLK);
		break;
	case 4:
		proc_comm_clk_disable(PROC_COMM_SDC4_CLK);
		break;
	}

}

void proc_comm_enable_sdcard_pclk(int instance)
{
	switch(instance)
	{

	case 1:
		proc_comm_clk_enable(PROC_COMM_SDC1_PCLK);
		break;
	case 2:
		proc_comm_clk_enable(PROC_COMM_SDC2_PCLK);
		break;
	case 3:
		proc_comm_clk_enable(PROC_COMM_SDC3_PCLK);
		break;
	case 4:
		proc_comm_clk_enable(PROC_COMM_SDC4_PCLK);
		break;
	}

}

void proc_comm_disable_sdcard_pclk(int instance)
{
	switch(instance)
	{

	case 1:
		proc_comm_clk_disable(PROC_COMM_SDC1_PCLK);
		break;
	case 2:
		proc_comm_clk_disable(PROC_COMM_SDC2_PCLK);
		break;
	case 3:
		proc_comm_clk_disable(PROC_COMM_SDC3_PCLK);
		break;
	case 4:
		proc_comm_clk_disable(PROC_COMM_SDC4_PCLK);
		break;
	}

}

uint32_t proc_comm_is_sdcard_clk_enabled(int instance)
{
	uint32_t ret=0;
	switch(instance)
	{

	case 1:
		ret = proc_comm_is_clk_enabled(PROC_COMM_SDC1_CLK);
		break;
	case 2:
		ret = proc_comm_is_clk_enabled(PROC_COMM_SDC2_CLK);
		break;
	case 3:
		ret = proc_comm_is_clk_enabled(PROC_COMM_SDC3_CLK);
		break;
	case 4:
		ret = proc_comm_is_clk_enabled(PROC_COMM_SDC4_CLK);
		break;
	}
	return ret;
}
uint32_t proc_comm_is_sdcard_pclk_enabled(int instance)
{
	uint32_t ret=0;
	switch(instance)
	{

	case 1:
		ret = proc_comm_is_clk_enabled(PROC_COMM_SDC1_PCLK);
		break;
	case 2:
		ret = proc_comm_is_clk_enabled(PROC_COMM_SDC2_PCLK);
		break;
	case 3:
		ret = proc_comm_is_clk_enabled(PROC_COMM_SDC3_PCLK);
		break;
	case 4:
		ret = proc_comm_is_clk_enabled(PROC_COMM_SDC4_PCLK);
		break;
	}
	return ret;
}

//---- UART related proc_comm sdcard clients.
uint32_t proc_comm_is_uart_clk_enabled(int instance)
{return 1;}

uint32_t proc_comm_get_uart_clk(int uart_base_addr)
{
	uint32_t rate = 0;
	switch(uart_base_addr)
	{

	case UART1_BASE:
		rate = proc_comm_get_clk_rate(PROC_COMM_UART1_CLK);
		break;
	case UART2_BASE:
		rate = proc_comm_get_clk_rate(PROC_COMM_UART2_CLK);
		break;
	case UART3_BASE:
		rate = proc_comm_get_clk_rate(PROC_COMM_UART3_CLK);
		break;
	default:
		rate = 0;
	}
	return rate;
}
/* LCD_NS_REG, LCD_MD_REG affected */

void proc_comm_set_lcdc_clk(int rate)
{
	proc_comm_set_clk_rate(PROC_COMM_MDP_LCDC_PCLK_CLK, rate);
}

void proc_comm_enable_lcdc_pad_clk(void)
{
	proc_comm_clk_enable(PROC_COMM_MDP_LCDC_PAD_PCLK_CLK);
}
/* enables LCD_NS_REG->LCD_CLK_BRANCH_ENA */
void proc_comm_enable_lcdc_clk(void)
{
	proc_comm_clk_enable(PROC_COMM_MDP_LCDC_PCLK_CLK);
}

/* enables LCD_NS_REG->MNCNTR_EN,LCD_ROOT_ENA, LCD_CLK_EXT_BRANCH_ENA */
uint32_t proc_comm_get_lcdc_clk(void)
{
        return proc_comm_get_clk_rate(PROC_COMM_MDP_LCDC_PCLK_CLK);
}

void proc_comm_end_cmds(void)
{
	volatile proc_comm_t pc_pkt;
	pc_pkt.data1 = 0;
	pc_pkt.data2 = 0;
	pc_pkt.command = PROC_COMM_END_CMDS;
	msm_proc_comm((proc_comm_t *)&pc_pkt);
	debug("PROC_COMM_END_CMDS  sent\n");
}
#endif /*USE_PROC_COMM*/
