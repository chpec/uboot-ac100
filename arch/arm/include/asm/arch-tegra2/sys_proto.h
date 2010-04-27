/*
 * (C) Copyright 2010
 * NVIDIA Corporation <www.nvidia.com>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#ifndef _SYS_PROTO_H_
#define _SYS_PROTO_H_

typedef struct {
	char *board_string;
	char *nand_string;
} tegra2_sysinfo;

void v7_flush_dcache_all(u32);
void prepare_jump_to_kernel(void);
u32 get_device_type(void);
void l2cache_enable(void);
void l2cache_disable(void);

int usb_eth_initialize(bd_t *bi);
int tegra2_udc_probe(void);

#endif
