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
	TEGRA2_CMD_PORT,
	TEGRA2_CMD_OUTPUT,
	TEGRA2_CMD_INPUT,
};

#define GPIO_PORT(x)		((x) >> 3)
#define GPIO_BIT(x)		((x) & 0x7)

/* Config port:bit as GPIO, not SFPIO (default) */
void __set_config(unsigned port, unsigned bit, int type)
{
	u32 u;

	debug("__set_config: port = %d, bit = %d, %s\n", port, bit, type ? "GPIO" : "SFPIO");
	debug("CNF-GPIO REG OFFSET: 0x%08X\n", (GPIO_CNF(port)));

	u = readl(GPIO_CNF(port));
	if (type)				/* GPIO */
		u |= 1 << bit;
	else
		u &= ~(1 << bit);
	writel(u, GPIO_CNF(port));
}

/* Config GPIO port:bit as input or output (OE) */
void __set_direction(unsigned port, unsigned bit, int output)
{
	u32 u;

	debug("__set_direction: port = %d, bit = %d, %s\n", port, bit, output ? "OUT" : "IN");
	debug("OE-GPIO REG OFFSET: 0x%08X\n", (GPIO_OE(port)));

	u = readl(GPIO_OE(port));
	if (output)
		u |= 1 << bit;
	else
		u &= ~(1 << bit);
	writel(u, GPIO_OE(port));
}

/* set GPIO OUT port:bit as 0 or 1 */
void __set_level(unsigned port, unsigned bit, int high)
{
	u32 u;

	debug("__set_level: port = %d, bit %d == %d\n", port, bit, high);
	debug("OUT-GPIO REG OFFSET: 0x%08X\n", (GPIO_OUT(port)));

	u = readl(GPIO_OUT(port));
	if (high)
		u |= 1 << bit;
	else
		u &= ~(1 << bit);
	writel(u, GPIO_OUT(port));
}

/*
 * GENERIC_GPIO primitives.
 */

/* set GPIO port:bit as an input */
int tg2_gpio_direction_input(unsigned port, unsigned bit)
{
	debug("tg2_gpio_direction_input: port = %d, bit = %d\n", port, bit);

	/* Configure as a GPIO */
	__set_config(port, bit, 1);

	/* Configure GPIO direction as input. */
	__set_direction(port, bit, 0);

	return 0;
}

int tg2_gpio_direction_input_ex(unsigned offset)
{
	tg2_gpio_direction_input(GPIO_PORT(offset), GPIO_BIT(offset));
	return 0;
}

/* set GPIO port:bit as an output, with polarity 'value' */
int tg2_gpio_direction_output(unsigned port, unsigned bit, int value)
{
	debug("tg2_gpio_direction_output: port = %d, bit = %d, value = %d\n", port, bit, value);

	/* Configure as a GPIO */
	__set_config(port, bit, 1);

	/* Configure GPIO output value. */
	__set_level(port, bit, value);

	/* Configure GPIO direction as output. */
	__set_direction(port, bit, 1);

	return 0;
}

int tg2_gpio_direction_output_ex(unsigned offset, int value)
{
	tg2_gpio_direction_output(GPIO_PORT(offset), GPIO_BIT(offset), value);
	return 0;
}

/* read GPIO IN value of a port:bit */
int tg2_gpio_get_value(unsigned port, unsigned bit)
{
	debug("tg2_gpio_get_value: port = %d, bit = %d\n", port, bit);
	debug("IN-GPIO REG OFFSET: 0x%08X\n", (GPIO_IN(port)));

	int val;

	val = readl(GPIO_IN(port));

	return (val >> bit) & 1;
}

int tg2_gpio_get_value_ex(unsigned offset)
{
	tg2_gpio_get_value(GPIO_PORT(offset), GPIO_BIT(offset));
}

/* write GPIO OUT value of a port:bit */
void tg2_gpio_set_value(unsigned port, unsigned bit, int value)
{
	debug("tg2_gpio_set_value: port = %d, bit = %d, value = %d\n", port, bit, value);

	/* Configure GPIO output value. */
	__set_level(port, bit, value);
}

void tg2_gpio_set_value_ex(unsigned offset, int value)
{
	tg2_gpio_set_value(GPIO_PORT(offset), GPIO_BIT(offset), value);
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
	printf("gpio bits: 76543210\n");
	printf("-------------------\n");

	if (port < 0 || port > 27)
		return -1;

	printf("GPIO_CNF:  ");
	data = readl(GPIO_CNF(port));
	for (i = 7; i >= 0; i--)
		printf("%c", data & (1 << i) ? 'g' : 's');
	printf("\n");

	printf("GPIO_OE:   ");
	data = readl(GPIO_OE(port));
	for (i = 7; i >= 0; i--)
		printf("%c", data & (1 << i) ? 'o' : 'i');
	printf("\n");

	printf("GPIO_IN:   ");
	data = readl(GPIO_IN(port));
	for (i = 7; i >= 0; i--)
		printf("%c", data & (1 << i) ? '1' : '0');
	printf("\n");

	printf("GPIO_OUT:  ");
	data = readl(GPIO_OUT(port));
	for (i = 7; i >= 0; i--)
		printf("%c", data & (1 << i) ? '1' : '0');
	printf("\n");

	return 0;
}
#endif /* CONFIG_CMD_TEGRA2_GPIO_INFO */

cmd_tbl_t cmd_gpio[] = {
	U_BOOT_CMD_MKENT(port, 3, 0, (void *)TEGRA2_CMD_PORT, "", ""),
	U_BOOT_CMD_MKENT(output, 5, 0, (void *)TEGRA2_CMD_OUTPUT, "", ""),
	U_BOOT_CMD_MKENT(input, 4, 0, (void *)TEGRA2_CMD_INPUT, "", ""),
#ifdef CONFIG_CMD_TEGRA2_GPIO_INFO
	U_BOOT_CMD_MKENT(info, 3, 0, (void *)TEGRA2_CMD_INFO, "", ""),
#endif
};

int do_gpio(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	static uint8_t port = 0;		/* defaults to port 'A' */
	int val;
	ulong ul_arg2, ul_arg3, ul_arg4;
	cmd_tbl_t *c;

	ul_arg2 = ul_arg3 = ul_arg4 = 0;

	c = find_cmd_tbl(argv[1], cmd_gpio, ARRAY_SIZE(cmd_gpio));

	/* All commands but "port" require 'maxargs' arguments */
	if (!c || !((argc == (c->maxargs)) ||
		(((int)c->cmd == TEGRA2_CMD_PORT) &&
		 (argc == (c->maxargs - 1))))) {
		cmd_usage(cmdtp);
		return 1;
	}

	/* arg2 used as port number */
	if (argc > 2)
		ul_arg2 = simple_strtoul(argv[2], NULL, 10);

	/* arg3 used as bit */
	if (argc > 3)
		ul_arg3 = simple_strtoul(argv[3], NULL, 10) & 0xF;

	/* arg4 used as output value, 0 or 1 */
	if (argc > 4)
		ul_arg4 = simple_strtoul(argv[4], NULL, 10) & 0x1;

	switch ((int)c->cmd) {
#ifdef CONFIG_CMD_TEGRA2_GPIO_INFO
	case TEGRA2_CMD_INFO:
		if (argc == 3)
			port = (uint8_t)ul_arg2;
		return tg2_gpio_info(port);

#endif
	case TEGRA2_CMD_PORT:
		if (argc == 3)
			port = (uint8_t)ul_arg2;
		printf("Current port #: %d\n", port);
		return 0;

	case TEGRA2_CMD_INPUT:
		/* arg2 = port, arg3 = bit */
		tg2_gpio_direction_input(ul_arg2, ul_arg3);
		val = tg2_gpio_get_value(ul_arg2, ul_arg3);

		printf("port:bit %ld:%ld = %d\n", ul_arg2, ul_arg3, val);
		return 0;

	case TEGRA2_CMD_OUTPUT:
		/* args = port, bit, value */
		tg2_gpio_direction_output(ul_arg2, ul_arg3, ul_arg4);
		printf("port:bit %ld:%ld = %ld\n", ul_arg2, ul_arg3, ul_arg4);
		return 0;

	default:
		/* We should never get here */
		return 1;
	}
}

U_BOOT_CMD(
	gpio,	5,	1,	do_gpio,
	"Tegra2 gpio access",
	"port [port#]\n"
	"	- show or set current port # (decimal, 0 = 'A' to 27 = 'BB')\n"
#ifdef CONFIG_CMD_TEGRA2_GPIO_INFO
	"     info port\n"
	"	- display info for all bits in 'port'\n"
#endif
	"     output port bit 0|1\n"
	"	- set 'port:bit' as output and drive low or high\n"
	"     input port bit\n"
	"	- set 'port:bit' as input and read value"
);
