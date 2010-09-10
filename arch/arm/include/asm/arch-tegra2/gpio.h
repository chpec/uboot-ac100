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

#define TEGRA2_GPIO_BASE	0x6000D000

#define GPIO_OFF(port)		(((port / 4) * 128) + ((port % 4) * 4))
#define GPIO_CNF(port)		(TEGRA2_GPIO_BASE + GPIO_OFF(port) + 0x00)
#define GPIO_OE(port)		(TEGRA2_GPIO_BASE + GPIO_OFF(port) + 0x10)
#define GPIO_OUT(port)		(TEGRA2_GPIO_BASE + GPIO_OFF(port) + 0x20)
#define GPIO_IN(port)		(TEGRA2_GPIO_BASE + GPIO_OFF(port) + 0x30)
#define GPIO_INT_STA(port)	(TEGRA2_GPIO_BASE + GPIO_OFF(port) + 0x40)
#define GPIO_INT_ENB(port)	(TEGRA2_GPIO_BASE + GPIO_OFF(port) + 0x50)
#define GPIO_INT_LVL(port)	(TEGRA2_GPIO_BASE + GPIO_OFF(port) + 0x60)
#define GPIO_INT_CLR(port)	(TEGRA2_GPIO_BASE + GPIO_OFF(port) + 0x70)

/*
 * Tegra2-specific GPIO API
 */

int tg2_gpio_direction_input(unsigned port, unsigned bit);
int tg2_gpio_direction_output(unsigned port, unsigned bit, int value);
int tg2_gpio_get_value(unsigned port, unsigned bit);
void tg2_gpio_set_value(unsigned port, unsigned bit, int value);

#endif
