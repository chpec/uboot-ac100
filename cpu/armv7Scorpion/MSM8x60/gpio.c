/*
 * Copyright (c) 2010, Code Aurora Forum. All rights reserved.
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
#include <asm/arch/gpio.h>

void gpio_tlmm_config(uint32_t gpio, uint8_t func,
			   uint8_t dir, uint8_t pull,
			   uint8_t drvstr, uint32_t enable )
{
	unsigned int val = 0;
	val |= pull;
	val |= func << 2;
	val |= drvstr << 6;
	val |= enable << 9;
	unsigned int* addr = (unsigned int*)GPIO_BASE_ADDR(gpio);
	IO_WRITE32(addr,val);
    return;
}
