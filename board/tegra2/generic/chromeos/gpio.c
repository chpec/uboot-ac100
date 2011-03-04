/*
 * Copyright (c) 2011 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 * Alternatively, this software may be distributed under the terms of the
 * GNU General Public License ("GPL") version 2 as published by the Free
 * Software Foundation.
 */

/* Implementation of per-board GPIO accessor functions */

#include <common.h>
#include <asm/arch/gpio.h>
#include "../../common/lcd/gpinit/gpinit.h"

#include <chromeos/hardware_interface.h>

#define GPIO_ACTIVE_HIGH	0
#define GPIO_ACTIVE_LOW		1

#define GPIO_ACCESSOR(gpio_number, polarity) \
	tg2_gpio_direction_input(TEGRA_GPIO_PORT(gpio_number), \
			TEGRA_GPIO_BIT(gpio_number)); \
	return (polarity) ^ tg2_gpio_get_value(TEGRA_GPIO_PORT(gpio_number), \
			TEGRA_GPIO_BIT(gpio_number));

int is_firmware_write_protect_gpio_asserted(void)
{
	GPIO_ACCESSOR(TEGRA_GPIO_PH3, GPIO_ACTIVE_LOW)
}

int is_recovery_mode_gpio_asserted(void)
{
	GPIO_ACCESSOR(TEGRA_GPIO_PH0, GPIO_ACTIVE_LOW)
}

int is_developer_mode_gpio_asserted(void)
{
	GPIO_ACCESSOR(TEGRA_GPIO_PV0, GPIO_ACTIVE_HIGH)
}
