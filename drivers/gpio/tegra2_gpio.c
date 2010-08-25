/*
 * drivers/gpio/tegra2_gpio.c
 *
 * NVIDIA Tegra2 GPIO handling.
 * Copyright (C) 2010 NVIDIA Corporation
 *
 * This file is licensed under the terms of the GNU General Public
 * License version 2.  This program is licensed "as is" without any
 * warranty of any kind, whether express or implied.
 */

/*
 * Based on (mostly copied from) kw_gpio.c based Linux 2.6 kernel driver.
 *
 * Tom Warren (twarren@nvidia.com) Wednesday, July 28 2010
 */

/* Define DEBUG to turn on debug print
 * #define	DEBUG		1
 */

#include <common.h>
#include <asm/io.h>
#include <asm/bitops.h>
#include <asm/arch/tegra2.h>
#include <asm/arch/gpio.h>

enum {
	TEGRA2_CMD_INFO,
	TEGRA2_CMD_DEVICE,
	TEGRA2_CMD_OUTPUT,
	TEGRA2_CMD_INPUT,
};

/* Config pin as GPIO, not SFPIO (default) */
void __set_config(unsigned pin, int type)
{
	u32 u;

	debug("__set_config: pin = %d, %s\n", pin, type ? "GPIO" : "SFPIO");
	debug("CNF-GPIO REG OFFSET: 0x%08X\n", (GPIO_CNF(pin)));

	u = readl(GPIO_CNF(pin));
	if (type)				/* GPIO */
		u |= 1 << (pin_to_bit(pin));
	else
		u &= ~(1 << (pin_to_bit(pin)));
	writel(u, GPIO_CNF(pin));
}

/* Config GPIO as input or output (OE) */
void __set_direction(unsigned pin, int output)
{
	u32 u;

	debug("__set_direction: pin = %d, %s\n", pin, output ? "OUT" : "IN");
	debug("OE-GPIO REG OFFSET: 0x%08X\n", (GPIO_OE(pin)));

	u = readl(GPIO_OE(pin));
	if (output)
		u |= 1 << (pin_to_bit(pin));
	else
		u &= ~(1 << (pin_to_bit(pin)));
	writel(u, GPIO_OE(pin));
}

/* set GPIO OUT pin as 0 or 1 */
void __set_level(unsigned pin, int high)
{
	u32 u;

	debug("__set_level: pin = %d == %d\n", pin, high);
	debug("OUT-GPIO REG OFFSET: 0x%08X\n", (GPIO_OUT(pin)));

	u = readl(GPIO_OUT(pin));
	if (high)
		u |= 1 << (pin_to_bit(pin));
	else
		u &= ~(1 << (pin_to_bit(pin)));
	writel(u, GPIO_OUT(pin));
}

/*
 * GENERIC_GPIO primitives.
 */

/* set GPIO pin as an input */
int tg2_gpio_direction_input(unsigned pin)
{
	debug("tg2_gpio_direction_input: pin = %d\n", pin);

	/* Configure as a GPIO */
	__set_config(pin, 1);

	/* Configure GPIO direction as input. */
	__set_direction(pin, 0);

	return 0;
}

/* set GPIO pin as an output, with polarity 'value' */
int tg2_gpio_direction_output(unsigned pin, int value)
{
	debug("tg2_gpio_direction_output: pin = %d, value = %d\n", pin, value);

	/* Configure as a GPIO */
	__set_config(pin, 1);

	/* Configure GPIO output value. */
	__set_level(pin, value);

	/* Configure GPIO direction as output. */
	__set_direction(pin, 1);

	return 0;
}

/* read GPIO IN value of a pin */
int tg2_gpio_get_value(unsigned pin)
{
	debug("tg2_gpio_get_value: pin = %d\n", pin);
	debug("IN-GPIO REG OFFSET: 0x%08X\n", (GPIO_IN(pin)));

	int val;

	val = readl(GPIO_IN(pin));

	return (val >> (pin_to_bit(pin))) & 1;
}

/* write GPIO OUT value of a pin */
void tg2_gpio_set_value(unsigned pin, int value)
{
	debug("tg2_gpio_set_value: pin = %d, value = %d\n", pin, value);

	/* Configure GPIO output value. */
	__set_level(pin, value);
}

#ifdef CONFIG_CMD_TEGRA2_GPIO_INFO
/*
 * Display Tegra2 GPIO information
 */
static int tg2_gpio_info(uint8_t port)
{
	int i;
	u32 data;

	printf("Tegra2 GPIO port %d:\n\n", port);
	printf("gpio pins: 76543210\n");
	printf("-------------------\n");

	if (port < 0 || port > 27)
		return -1;

	printf("GPIO_CNF:  ");
	data = readl(GPIO_CNF(port << 5));
	for (i = 7; i >= 0; i--)
		printf("%c", data & (1 << i) ? 'g' : 's');
	printf("\n");

	printf("GPIO_OE:   ");
	data = readl(GPIO_OE(port << 5));
	for (i = 7; i >= 0; i--)
		printf("%c", data & (1 << i) ? 'o' : 'i');
	printf("\n");

	printf("GPIO_IN:   ");
	data = readl(GPIO_IN(port << 5));
	for (i = 7; i >= 0; i--)
		printf("%c", data & (1 << i) ? '1' : '0');
	printf("\n");

	printf("GPIO_OUT:  ");
	data = readl(GPIO_OUT(port << 5));
	for (i = 7; i >= 0; i--)
		printf("%c", data & (1 << i) ? '1' : '0');
	printf("\n");

	return 0;
}
#endif /* CONFIG_CMD_TEGRA2_GPIO_INFO */

cmd_tbl_t cmd_gpio[] = {
	U_BOOT_CMD_MKENT(device, 3, 0, (void *)TEGRA2_CMD_DEVICE, "", ""),
	U_BOOT_CMD_MKENT(output, 4, 0, (void *)TEGRA2_CMD_OUTPUT, "", ""),
	U_BOOT_CMD_MKENT(input, 3, 0, (void *)TEGRA2_CMD_INPUT, "", ""),
#ifdef CONFIG_CMD_TEGRA2_GPIO_INFO
	U_BOOT_CMD_MKENT(info, 2, 0, (void *)TEGRA2_CMD_INFO, "", ""),
#endif
};

int do_gpio(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	static uint8_t port = 20;
	int val;
	ulong ul_arg2 = 0;
	ulong ul_arg3 = 0;
	cmd_tbl_t *c;

	c = find_cmd_tbl(argv[1], cmd_gpio, ARRAY_SIZE(cmd_gpio));

	/* All commands but "device" require 'maxargs' arguments */
	if (!c || !((argc == (c->maxargs)) ||
		(((int)c->cmd == TEGRA2_CMD_DEVICE) &&
		 (argc == (c->maxargs - 1))))) {
		cmd_usage(cmdtp);
		return 1;
	}

	/* arg2 used as port number or pin number */
	if (argc > 2)
		ul_arg2 = simple_strtoul(argv[2], NULL, 10);

	/* arg3 used as pin or invert value */
	if (argc > 3)
		ul_arg3 = simple_strtoul(argv[3], NULL, 10) & 0x1;

	switch ((int)c->cmd) {
#ifdef CONFIG_CMD_TEGRA2_GPIO_INFO
	case TEGRA2_CMD_INFO:
		return tg2_gpio_info(port);

#endif
	case TEGRA2_CMD_DEVICE:
		if (argc == 3)
			port = (uint8_t)ul_arg2;
		printf("Current port #: %d\n", port);
		return 0;

	case TEGRA2_CMD_INPUT:
		/* arg = pin */
		tg2_gpio_direction_input(ul_arg2);
		val = tg2_gpio_get_value(ul_arg2);

		printf("port %d, pin %ld = %d\n", port, ul_arg2, val);
		return 0;

	case TEGRA2_CMD_OUTPUT:
		/* args = pin, value */
		tg2_gpio_direction_output((ul_arg2), ul_arg3);
		printf("port %d, pin %ld = %ld\n", port, ul_arg2, ul_arg3);
		return 0;

	default:
		/* We should never get here */
		return 1;
	}
}

U_BOOT_CMD(
	gpio,	5,	1,	do_gpio,
	"Tegra2 gpio access",
	"device [dev]\n"
	"	- show or set current device address\n"
#ifdef CONFIG_CMD_TEGRA2_GPIO_INFO
	"     info\n"
	"	- display info for current port\n"
#endif
	"     output pin 0|1\n"
	"	- set pin as output and drive low or high\n"
	"     input pin\n"
	"	- set pin as input and read value"
);
