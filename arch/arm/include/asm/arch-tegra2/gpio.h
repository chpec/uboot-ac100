/*
 * arch/asm-arm/arch-tegra2/gpio.h
 *
 * This file is licensed under the terms of the GNU General Public
 * License version 2.  This program is licensed "as is" without any
 * warranty of any kind, whether express or implied.
 */

/*
 * Based on (mostly copied from) Kirkwood based Linux 2.6 kernel driver.
 *
 * Tom Warren - twarren@nvidia.com - Wednesday, July 28 2010
 * Copyright (C) 2010 NVIDIA Corporation
 */

#ifndef __TEGRA2_GPIO_H
#define __TEGRA2_GPIO_H

/* from kernel include/linux/bitops.h */
#define BITS_PER_BYTE 8
#define BITS_TO_LONGS(nr)	DIV_ROUND_UP(nr, BITS_PER_BYTE * sizeof(long))

#define GPIO_MAX		224
#define TEGRA2_GPIO0_BASE	0x6000D000

#define pin_to_port(pin)    	(pin / 8)
#define pin_to_bit(pin)    	(pin % 8)

#define GPIO_OFF(pin)		(pin_to_port(pin) << 5)
#define GPIO_CNF(pin)		(TEGRA2_GPIO0_BASE + GPIO_OFF(pin) + 0x00)
#define GPIO_OE(pin)	    	(TEGRA2_GPIO0_BASE + GPIO_OFF(pin) + 0x10)
#define GPIO_OUT(pin)	    	(TEGRA2_GPIO0_BASE + GPIO_OFF(pin) + 0x20)
#define GPIO_IN(pin)	    	(TEGRA2_GPIO0_BASE + GPIO_OFF(pin) + 0x30)
#define GPIO_INT_STA(pin)	(TEGRA2_GPIO0_BASE + GPIO_OFF(pin) + 0x40)
#define GPIO_INT_ENB(pin)	(TEGRA2_GPIO0_BASE + GPIO_OFF(pin) + 0x50)
#define GPIO_INT_LVL(pin)	(TEGRA2_GPIO0_BASE + GPIO_OFF(pin) + 0x60)
#define GPIO_INT_CLR(pin)	(TEGRA2_GPIO0_BASE + GPIO_OFF(pin) + 0x70)

/*
 * Tegra2-specific GPIO API
 */

int tg2_gpio_direction_input(unsigned pin);
int tg2_gpio_direction_output(unsigned pin, int value);
int tg2_gpio_get_value(unsigned pin);
void tg2_gpio_set_value(unsigned pin, int value);

#endif
