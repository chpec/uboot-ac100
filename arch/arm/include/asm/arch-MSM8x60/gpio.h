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

#ifndef _GPIO_H_
#define _GPIO_H_

#include <common.h>

#define TLMM_BASE_ADDR (0x800000)
#define GPIO_BASE (0x1000)
#define GPIO_BASE_ADDR(x) (TLMM_BASE_ADDR + GPIO_BASE + (x)*0x10)

#define SDC3_HDRV_PULL_CTL (0x20A4)

/*SDC related*/
extern void sdcard_gpio_config(int instance);
extern void sdcard_power(int state);

/* GPIO TLMM: Function -- GPIO specific */

/* GPIO TLMM: Direction */
enum {
	GPIO_INPUT,
	GPIO_OUTPUT,
};

/* GPIO TLMM: Pullup/Pulldown */
enum {
	GPIO_NO_PULL,
	GPIO_PULL_DOWN,
	GPIO_KEEPER,
	GPIO_PULL_UP,
};

/* GPIO TLMM: Drive Strength */
enum {
	GPIO_2MA,
	GPIO_4MA,
	GPIO_6MA,
	GPIO_8MA,
	GPIO_10MA,
	GPIO_12MA,
	GPIO_14MA,
	GPIO_16MA,
};

enum {
	GPIO_ENABLE,
	GPIO_DISABLE,
};

#endif /* __GPIO_H_ */

