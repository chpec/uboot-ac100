/*
 * Copyright (c) 2007-2009, Code Aurora Forum. All rights reserved.
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

#ifndef _PROC_COMM_CLIENTS_H_
#define _PROC_COMM_CLIENTS_H_

/*LCDC related*/
extern void proc_comm_set_lcdc_clk(int rate);
extern void proc_comm_enable_lcdc_clk(void);
extern void proc_comm_enable_lcdc_pad_clk(void);
extern uint32_t proc_comm_get_lcdc_clk(void);

/*HSUSB related*/
extern void proc_comm_usb_vbus_power(int state);
extern void proc_comm_usb_reset_phy(void);
extern void proc_comm_enable_hsusb_clk(void);
extern void proc_comm_disable_hsusb_clk(void);

/*UART related*/
extern uint32_t proc_comm_get_uart_clk(int uart_base_addr);

/*SDC related*/
extern void proc_comm_sdcard_gpio_config(int instance);
extern void proc_comm_sdcard_power(int state);
extern void proc_comm_set_sdcard_clk(int instance, int rate);
extern uint32_t proc_comm_get_sdcard_clk(int instance);
extern void proc_comm_enable_sdcard_clk(int instance);
extern void proc_comm_disable_sdcard_clk(int instance);
extern uint32_t proc_comm_is_sdcard_clk_enabled(int instance);
extern void proc_comm_enable_sdcard_pclk(int instance);
extern void proc_comm_disable_sdcard_pclk(int instance);
extern uint32_t proc_comm_is_sdcard_pclk_enabled(int instance);

#endif /*_PROC_COMM_CLIENTS_H*/
