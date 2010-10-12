/*
 * Copyright (c) 2010 Samsung Electronics Co., Ltd.
 * Jiun Yu <jiun.yu@samsung.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <common.h>

/* jsgood: temporary */
#ifdef CONFIG_BOOT_ONENAND_IROM

#define ONENAND_ReadPage_4burst(a,b,c)	(((int(*)(uint, uint, uint *))(*((uint *)(0x0c004010))))(a,b,c))

void onenand_bl2_copy(void)
{
	int block, page;
	volatile unsigned int base = 0x27e00000;

	for (block = 0; block < 2; block++) {
		for (page = 0; page < 64; page++) {
			ONENAND_ReadPage_4burst(base, block, page);
			base += 0x200;
		}
	}
}

#endif /* CONFIG_BOOT_ONENAND */

