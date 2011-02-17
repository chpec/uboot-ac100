/*
 *  (C) Copyright 2010
 *  NVIDIA Corporation <www.nvidia.com>
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

#include <common.h>
#include <asm/io.h>
#include <asm/arch/nv_drf.h>
#include <asm/arch/nvcommon.h>
#include <asm/arch/tegra2.h>
#include <asm/arch/nv_hardware_access.h>
#include <asm/types.h>
#include <div64.h>

#include "gpinit.h"
#include "../../sdmmc/nvboot_util.h"

#define OSC_CTRL_OSC_FREQ_MASK		(3<<30)
#define OSC_CTRL_OSC_FREQ_13MHZ		(0<<30)
#define OSC_CTRL_OSC_FREQ_19_2MHZ	(1<<30)
#define OSC_CTRL_OSC_FREQ_12MHZ		(2<<30)
#define OSC_CTRL_OSC_FREQ_26MHZ		(3<<30)
#define OSC_CTRL_MASK			(0x3f2 | OSC_CTRL_OSC_FREQ_MASK)

#define OSC_FREQ_DET_TRIG		(1<<31)

#define OSC_FREQ_DET_BUSY		(1<<31)
#define OSC_FREQ_DET_CNT_MASK		0xFFFF

#define PERIPH_CLK_SOURCE_MASK		(3<<30)
#define PERIPH_CLK_SOURCE_SHIFT		30
#define PERIPH_CLK_SOURCE_4BIT_MASK	(7<<28)
#define PERIPH_CLK_SOURCE_4BIT_SHIFT	28
#define PERIPH_CLK_SOURCE_ENABLE	(1<<28)
#define PERIPH_CLK_SOURCE_DIVU71_MASK	0xFF
#define PERIPH_CLK_SOURCE_DIVU16_MASK	0xFFFF

#define PERIPH_CLK_TO_ENB_REG(c)	((c->clk_num / 32) * 4)
#define PERIPH_CLK_TO_ENB_SET_REG(c)	((c->clk_num / 32) * 8)
#define PERIPH_CLK_TO_ENB_BIT(c)	(1 << (c->clk_num % 32))

#define PLL_BASE			0x0
#define PLL_BASE_BYPASS			(1<<31)
#define PLL_BASE_ENABLE			(1<<30)
#define PLL_BASE_REF_ENABLE		(1<<29)
#define PLL_BASE_OVERRIDE		(1<<28)
#define PLL_BASE_LOCK			(1<<27)
#define PLL_BASE_DIVP_MASK		(0x7<<20)
#define PLL_BASE_DIVP_SHIFT		20
#define PLL_BASE_DIVN_MASK		(0x3FF<<8)
#define PLL_BASE_DIVN_SHIFT		8
#define PLL_BASE_DIVM_MASK		(0x1F)
#define PLL_BASE_DIVM_SHIFT		0

#define PLL_MISC(c)		(((c)->flags & PLL_ALT_MISC_REG) ? 0x4 : 0xc)
#define PLL_MISC_LOCK_ENABLE(c)	(((c)->flags & PLLU) ? (1<<22) : (1<<18))

#define PLL_MISC_CPCON_SHIFT		8
#define PLL_MISC_CPCON_MASK		(0xF<<PLL_MISC_CPCON_SHIFT)

#define PLLU_BASE_POST_DIV		(1<<20)

static phys_addr_t reg_clk_base = TEGRA_CLK_RESET_BASE;

#define clk_writel(value, reg) \
	writel(value, (u32)reg_clk_base + (reg))
#define clk_readl(reg) \
	readl((u32)reg_clk_base + (reg))


static struct tegra_clk_init_table common_clk_init_table[] = {
	/* name		parent		rate		enabled */
	{ "clk_m",	NULL,		0,		true },
	{ "pll_p",	"clk_m",	216000000,	true },
	{ NULL,		NULL,		0,		0},
};

static unsigned long clk_measure_input_freq(void)
{
	u32 clock_autodetect;
	clk_writel(OSC_FREQ_DET_TRIG | 1, CLK_RST_CONTROLLER_OSC_FREQ_DET_0);
	do {} while (clk_readl(CLK_RST_CONTROLLER_OSC_FREQ_DET_STATUS_0) &
		OSC_FREQ_DET_BUSY);

	clock_autodetect = clk_readl(CLK_RST_CONTROLLER_OSC_FREQ_DET_STATUS_0);
	if (clock_autodetect >= 732 - 3 && clock_autodetect <= 732 + 3) {
		return 12000000;
	} else if (clock_autodetect >= 794 - 3 && clock_autodetect <= 794 + 3) {
		return 13000000;
	} else if (clock_autodetect >= 1172 - 3 &&
		clock_autodetect <= 1172 + 3) {
		return 19200000;
	} else if (clock_autodetect >= 1587 - 3 &&
		clock_autodetect <= 1587 + 3) {
		return 26000000;
	} else {
		printf("%s: Unexpected clock autodetect value %d",
			__func__, clock_autodetect);
		return 0;
	}
}

static int clk_div71_get_divider(unsigned long parent_rate, unsigned long rate)
{
	u64 divider_u71 = parent_rate * 2;
	divider_u71 += rate - 1;
	do_div(divider_u71, rate);

	if ((s64)divider_u71 - 2 < 0)
		return 0;

	if ((s64)divider_u71 - 2 > 255)
		return -1;

	return divider_u71 - 2;
}

static int clk_div16_get_divider(unsigned long parent_rate, unsigned long rate)
{
	u64 divider_u16;

	divider_u16 = parent_rate;
	divider_u16 += rate - 1;
	do_div(divider_u16, rate);

	if ((s64)divider_u16 - 1 < 0)
		return 0;

	if ((s64)divider_u16 - 1 > 255)
		return -1;

	return divider_u16 - 1;
}

static void clk_recalculate_rate(struct clk *c)
{
	u64 rate;

	if (!c->parent)
		return;

	rate = c->parent->rate;

	if (c->mul != 0 && c->div != 0) {
		rate = rate * c->mul;
		do_div(rate, c->div);
	}

	if ((rate > c->max_rate) && (c->state == ON))
		printf("clocks: Set clock %s to rate %llu, max is %lu\n",
			c->name, rate, c->max_rate);

	c->rate = rate;
}

static struct clk *tegra_get_children_clock(struct clk *c);

static void propagate_rate(struct clk *c)
{
	struct clk *c_child;
	c_child = tegra_get_children_clock(c);
	if (c_child) {
		clk_recalculate_rate(c_child);
		propagate_rate(c_child);
	}
	return;
}

static int clk_set_parent_locked(struct clk *c, struct clk *parent)
{
	int ret;

	if (!c->ops || !c->ops->set_parent)
		return -1;

	ret = c->ops->set_parent(c, parent);

	if (ret)
		return ret;

	clk_recalculate_rate(c);

	propagate_rate(c);

	return 0;
}

static int clk_set_parent(struct clk *c, struct clk *parent)
{
	int ret;
	ret = clk_set_parent_locked(c, parent);
	return ret;
}

static int clk_set_rate_locked(struct clk *c, unsigned long rate)
{
	int ret;

	if (rate > c->max_rate)
		rate = c->max_rate;

	if (!c->ops || !c->ops->set_rate)
		return -1;

	ret = c->ops->set_rate(c, rate);

	if (ret)
		return ret;

	clk_recalculate_rate(c);

	propagate_rate(c);

	return 0;
}

static int clk_set_rate(struct clk *c, unsigned long rate)
{
	int ret = 0;

	ret = clk_set_rate_locked(c, rate);

	return ret;
}

static int clk_enable_locked(struct clk *c)
{
	int ret;

	if (c->refcnt == 0) {
		if (c->parent) {
			ret = clk_enable_locked(c->parent);
			if (ret)
				return ret;
		}

		if (c->ops && c->ops->enable) {
			ret = c->ops->enable(c);
			if (ret) {
				if (c->parent)
					clk_disable_locked(c->parent);
				return ret;
			}
			c->state = ON;
		}
	}
	c->refcnt++;

	return 0;
}

int clk_enable(struct clk *c)
{
	int ret;
	if (c->rate > c->max_rate) {
		printf("clocks: Enable clock %s, the rate is %lu, max is %lu\n",
			c->name, c->rate, c->max_rate);
		return -1;
	}
	ret = clk_enable_locked(c);
	return ret;
}

static void clk_init(struct clk *c)
{
	if (c->ops && c->ops->init)
		c->ops->init(c);

	clk_recalculate_rate(c);
}

void clk_disable_locked(struct clk *c)
{
	if (c->refcnt == 0) {
		printf("Attempting to disable clock %s with refcnt 0", c->name);
		return;
	}
	if (c->refcnt == 1) {
		if (c->ops && c->ops->disable)
			c->ops->disable(c);

		if (c->parent)
			clk_disable_locked(c->parent);

		c->state = OFF;
	}
	c->refcnt--;
}

unsigned long clk_get_rate(struct clk *c)
{
	unsigned long ret;
	ret = c->rate;
	return ret;
}

static int clk_reparent(struct clk *c, struct clk *parent)
{
	c->parent = parent;
	return 0;
}

/* Periph clk ops */

static void tegra2_periph_clk_init(struct clk *c)
{
	u32 val = clk_readl(c->reg);
	const struct clk_mux_sel *mux = 0;
	const struct clk_mux_sel *sel;
	u32 shift;

	if (c->flags & PERIPH_SOURCE_CLK_4BIT)
		shift = PERIPH_CLK_SOURCE_4BIT_SHIFT;
	else
		shift = PERIPH_CLK_SOURCE_SHIFT;

	if (c->flags & MUX) {
		for (sel = c->inputs; sel->input != NULL; sel++) {
			if (val >> shift == sel->value)
				mux = sel;
		}
		c->parent = mux->input;
	} else {
		c->parent = c->inputs[0].input;
	}

	if (c->flags & DIV_U71) {
		u32 divu71 = val & PERIPH_CLK_SOURCE_DIVU71_MASK;
		c->div = divu71 + 2;
		c->mul = 2;
	} else if (c->flags & DIV_U16) {
		u32 divu16 = val & PERIPH_CLK_SOURCE_DIVU16_MASK;
		c->div = divu16 + 1;
		c->mul = 1;
	} else {
		c->div = 1;
		c->mul = 1;
	}

	c->state = ON;
	if (!(clk_readl(CLK_RST_CONTROLLER_CLK_OUT_ENB_L_0 +
			PERIPH_CLK_TO_ENB_REG(c)) & PERIPH_CLK_TO_ENB_BIT(c)))
		c->state = OFF;
	if (!(c->flags & PERIPH_NO_RESET))
		if (clk_readl(CLK_RST_CONTROLLER_RST_DEVICES_L_0 +
			PERIPH_CLK_TO_ENB_REG(c)) & PERIPH_CLK_TO_ENB_BIT(c))
			c->state = OFF;
}

static int tegra2_periph_clk_enable(struct clk *c)
{
	u32 val;

	clk_writel(PERIPH_CLK_TO_ENB_BIT(c),
	     CLK_RST_CONTROLLER_CLK_ENB_L_SET_0 + PERIPH_CLK_TO_ENB_SET_REG(c));
	if (!(c->flags & PERIPH_NO_RESET) && !(c->flags & PERIPH_MANUAL_RESET))
		clk_writel(PERIPH_CLK_TO_ENB_BIT(c),
			CLK_RST_CONTROLLER_RST_DEV_L_CLR_0 +
			PERIPH_CLK_TO_ENB_SET_REG(c));
	if (c->flags & PERIPH_EMC_ENB) {
		/* The EMC peripheral clock has 2 extra enable bits */
		/* FIXME: Do they need to be disabled? */
		val = clk_readl(c->reg);
		val |= 0x3 << 24;
		clk_writel(val, c->reg);
	}
	return 0;
}

static void tegra2_periph_clk_disable(struct clk *c)
{
	clk_writel(PERIPH_CLK_TO_ENB_BIT(c),
		CLK_RST_CONTROLLER_CLK_ENB_L_CLR_0 +
		PERIPH_CLK_TO_ENB_SET_REG(c));
}

void tegra2_periph_reset_deassert(struct clk *c)
{
	if (!(c->flags & PERIPH_NO_RESET))
		clk_writel(PERIPH_CLK_TO_ENB_BIT(c),
			CLK_RST_CONTROLLER_RST_DEV_L_CLR_0 +
			PERIPH_CLK_TO_ENB_SET_REG(c));
}

void tegra2_periph_reset_assert(struct clk *c)
{
	if (!(c->flags & PERIPH_NO_RESET))
		clk_writel(PERIPH_CLK_TO_ENB_BIT(c),
			CLK_RST_CONTROLLER_RST_DEV_L_SET_0 +
			PERIPH_CLK_TO_ENB_SET_REG(c));
}

static int tegra2_periph_clk_set_parent(struct clk *c, struct clk *p)
{
	u32 val;
	const struct clk_mux_sel *sel;
	u32 mask, shift;

	if (c->flags & PERIPH_SOURCE_CLK_4BIT) {
		mask = PERIPH_CLK_SOURCE_4BIT_MASK;
		shift = PERIPH_CLK_SOURCE_4BIT_SHIFT;
	} else {
		mask = PERIPH_CLK_SOURCE_MASK;
		shift = PERIPH_CLK_SOURCE_SHIFT;
	}

	for (sel = c->inputs; sel->input != NULL; sel++) {
		if (sel->input == p) {
			val = clk_readl(c->reg);
			val &= ~mask;
			val |= (sel->value) << shift;

			if (c->refcnt)
				clk_enable_locked(p);

			clk_writel(val, c->reg);

			if (c->refcnt && c->parent)
				clk_disable_locked(c->parent);

			clk_reparent(c, p);
			return 0;
		}
	}

	return -1;
}

static int tegra2_periph_clk_set_rate(struct clk *c, unsigned long rate)
{
	u32 val;
	int divider;

	if (c->flags & DIV_U71) {
		divider = clk_div71_get_divider(c->parent->rate, rate);
		if (divider >= 0) {
			val = clk_readl(c->reg);
			val &= ~PERIPH_CLK_SOURCE_DIVU71_MASK;
			val |= divider;
			clk_writel(val, c->reg);
			c->div = divider + 2;
			c->mul = 2;
			return 0;
		}
	} else if (c->flags & DIV_U16) {
		divider = clk_div16_get_divider(c->parent->rate, rate);
		if (divider >= 0) {
			val = clk_readl(c->reg);
			val &= ~PERIPH_CLK_SOURCE_DIVU16_MASK;
			val |= divider;
			clk_writel(val, c->reg);
			c->div = divider + 1;
			c->mul = 1;
			return 0;
		}
	} else if (c->parent->rate <= rate) {
		c->div = 1;
		c->mul = 1;
		return 0;
	}
	return -1;
}

static long tegra2_periph_clk_round_rate(struct clk *c,
	unsigned long rate)
{
	int divider;

	if (c->flags & DIV_U71) {
		divider = clk_div71_get_divider(c->parent->rate, rate);
		if (divider < 0)
			return divider;

		return c->parent->rate * 2 / (divider + 2);
	} else if (c->flags & DIV_U16) {
		divider = clk_div16_get_divider(c->parent->rate, rate);
		if (divider < 0)
			return divider;
		return c->parent->rate / (divider + 1);
	}
	return -1;
}

static struct clk_ops tegra_periph_clk_ops = {
	.init			= &tegra2_periph_clk_init,
	.enable			= &tegra2_periph_clk_enable,
	.disable		= &tegra2_periph_clk_disable,
	.set_parent		= &tegra2_periph_clk_set_parent,
	.set_rate		= &tegra2_periph_clk_set_rate,
	.round_rate		= &tegra2_periph_clk_round_rate,
};

/* clk_m functions */
static unsigned long tegra2_clk_m_autodetect_rate(struct clk *c)
{
	u32 auto_clock_control = clk_readl(CLK_RST_CONTROLLER_OSC_CTRL_0) &
					~OSC_CTRL_OSC_FREQ_MASK;

	c->rate = clk_measure_input_freq();
	switch (c->rate) {
	case 12000000:
		auto_clock_control |= OSC_CTRL_OSC_FREQ_12MHZ;
		break;
	case 13000000:
		auto_clock_control |= OSC_CTRL_OSC_FREQ_13MHZ;
		break;
	case 19200000:
		auto_clock_control |= OSC_CTRL_OSC_FREQ_19_2MHZ;
		break;
	case 26000000:
		auto_clock_control |= OSC_CTRL_OSC_FREQ_26MHZ;
		break;
	default:
		printf("%s: Unexpected clock rate %ld", __func__, c->rate);
	}
	clk_writel(auto_clock_control, CLK_RST_CONTROLLER_OSC_CTRL_0);
	return c->rate;
}

static void tegra2_clk_m_init(struct clk *c)
{
	tegra2_clk_m_autodetect_rate(c);
}

static int tegra2_clk_m_enable(struct clk *c)
{
	return 0;
}

static void tegra2_clk_m_disable(struct clk *c)
{
	return;
}

static struct clk_ops tegra_clk_m_ops = {
	.init		= tegra2_clk_m_init,
	.enable		= tegra2_clk_m_enable,
	.disable	= tegra2_clk_m_disable,
};

/* PLL Functions */
static int tegra2_pll_clk_wait_for_lock(struct clk *c)
{
	int cnt;

	cnt = 0;
	while (!(clk_readl(c->reg + PLL_BASE) & PLL_BASE_LOCK)) {
		NvBootUtilWaitUS(10);
		if (cnt > 10) {
			printf("Timed out waiting for lock bit on pll %s",
				c->name);
			return -1;
		}
		cnt++;
	}
	return 0;
}

static void tegra2_pll_clk_init(struct clk *c)
{
	u32 val = clk_readl(c->reg + PLL_BASE);

	c->state = (val & PLL_BASE_ENABLE) ? ON : OFF;

	if (c->flags & PLL_FIXED && !(val & PLL_BASE_OVERRIDE)) {
		printf("Clock %s has unknown fixed frequency\n", c->name);
		c->mul = 1;
		c->div = 1;
	} else if (val & PLL_BASE_BYPASS) {
		c->mul = 1;
		c->div = 1;
	} else {
		c->mul = (val & PLL_BASE_DIVN_MASK) >> PLL_BASE_DIVN_SHIFT;
		c->div = (val & PLL_BASE_DIVM_MASK) >> PLL_BASE_DIVM_SHIFT;
		if (c->flags & PLLU)
			c->div *= (val & PLLU_BASE_POST_DIV) ? 1 : 2;
		else
			c->div *= (val & PLL_BASE_DIVP_MASK) ? 2 : 1;
	}
}

static int tegra2_pll_clk_enable(struct clk *c)
{
	u32 val;

	val = clk_readl(c->reg + PLL_BASE);
	val &= ~PLL_BASE_BYPASS;
	val |= PLL_BASE_ENABLE;
	clk_writel(val, c->reg + PLL_BASE);

	val = clk_readl(c->reg + PLL_MISC(c));
	val |= PLL_MISC_LOCK_ENABLE(c);
	clk_writel(val, c->reg + PLL_MISC(c));

	tegra2_pll_clk_wait_for_lock(c);

	return 0;
}

static void tegra2_pll_clk_disable(struct clk *c)
{
	u32 val;

	val = clk_readl(c->reg);
	val &= ~(PLL_BASE_BYPASS | PLL_BASE_ENABLE);
	clk_writel(val, c->reg);
}

static int tegra2_pll_clk_set_rate(struct clk *c, unsigned long rate)
{
	u32 val;
	unsigned long input_rate;
	const struct clk_pll_table *sel;

	input_rate = c->parent->rate;
	for (sel = c->pll_table; sel->input_rate != 0; sel++) {
		if (sel->input_rate == input_rate && sel->output_rate == rate) {
			c->mul = sel->n;
			c->div = sel->m * sel->p;

			val = clk_readl(c->reg + PLL_BASE);
			if (c->flags & PLL_FIXED)
				val |= PLL_BASE_OVERRIDE;
			val &= ~(PLL_BASE_DIVP_MASK | PLL_BASE_DIVN_MASK |
				 PLL_BASE_DIVM_MASK);
			val |= (sel->m << PLL_BASE_DIVM_SHIFT) |
				(sel->n << PLL_BASE_DIVN_SHIFT);

			if (c->flags & PLLU) {
				if (sel->p == 1)
					val |= PLLU_BASE_POST_DIV;
			} else {
				if (sel->p == 2)
					val |= 1 << PLL_BASE_DIVP_SHIFT;
			}
			clk_writel(val, c->reg + PLL_BASE);

			if (c->flags & PLL_HAS_CPCON) {
				val = clk_readl(c->reg + PLL_MISC(c));
				val &= ~PLL_MISC_CPCON_MASK;
				val |= sel->cpcon << PLL_MISC_CPCON_SHIFT;
				clk_writel(val, c->reg + PLL_MISC(c));
			}

			if (c->state == ON)
				tegra2_pll_clk_enable(c);

			return 0;
		}
	}
	return -1;
}

static struct clk_ops tegra_pll_ops = {
	.init			= tegra2_pll_clk_init,
	.enable			= tegra2_pll_clk_enable,
	.disable		= tegra2_pll_clk_disable,
	.set_rate		= tegra2_pll_clk_set_rate,
};

static struct clk tegra_clk_32k = {
	.name = "clk_32k",
	.rate = 32768,
	.ops  = NULL,
	.max_rate = 32768,
};

static struct clk_pll_table tegra_pll_s_table[] = {
	{32768, 12000000, 366, 1, 1, 0},
	{32768, 13000000, 397, 1, 1, 0},
	{32768, 19200000, 586, 1, 1, 0},
	{32768, 26000000, 793, 1, 1, 0},
	{0, 0, 0, 0, 0, 0},
};

static struct clk tegra_pll_s = {
	.name      = "pll_s",
	.flags     = PLL_ALT_MISC_REG,
	.ops       = &tegra_pll_ops,
	.reg       = 0xf0,
	.input_min = 32768,
	.input_max = 32768,
	.parent    = &tegra_clk_32k,
	.cf_min    = 0, /* FIXME */
	.cf_max    = 0, /* FIXME */
	.vco_min   = 12000000,
	.vco_max   = 26000000,
	.pll_table = tegra_pll_s_table,
	.max_rate  = 26000000,
};

static struct clk_mux_sel tegra_clk_m_sel[] = {
	{ .input = &tegra_clk_32k, .value = 0},
	{ .input = &tegra_pll_s,  .value = 1},
	{ 0, 0},
};
static struct clk tegra_clk_m = {
	.name      = "clk_m",
	.flags     = ENABLE_ON_INIT,
	.ops       = &tegra_clk_m_ops,
	.inputs    = tegra_clk_m_sel,
	.reg       = 0x1fc,
	.reg_mask  = (1<<28),
	.reg_shift = 28,
	.max_rate  = 26000000,
	.parent    = NULL,
};

static struct clk_pll_table tegra_pll_m_table[] = {
	{ 12000000, 666000000, 666, 12, 1, 8},
	{ 13000000, 666000000, 666, 13, 1, 8},
	{ 19200000, 666000000, 555, 16, 1, 8},
	{ 26000000, 666000000, 666, 26, 1, 8},
	{ 12000000, 600000000, 600, 12, 1, 8},
	{ 13000000, 600000000, 600, 13, 1, 8},
	{ 19200000, 600000000, 375, 12, 1, 6},
	{ 26000000, 600000000, 600, 26, 1, 8},
	{ 0, 0, 0, 0, 0, 0 },
};

static struct clk tegra_pll_m = {
	.name      = "pll_m",
	.flags     = PLL_HAS_CPCON,
	.ops       = &tegra_pll_ops,
	.reg       = 0x90,
	.input_min = 2000000,
	.input_max = 31000000,
	.parent    = &tegra_clk_m,
	.cf_min    = 1000000,
	.cf_max    = 6000000,
	.vco_min   = 20000000,
	.vco_max   = 1200000000,
	.pll_table = tegra_pll_m_table,
	.max_rate  = 800000000,
};

static struct clk_pll_table tegra_pll_p_table[] = {
	{ 12000000, 216000000, 432, 12, 2, 8},
	{ 13000000, 216000000, 432, 13, 2, 8},
	{ 19200000, 216000000, 90,   4, 2, 1},
	{ 26000000, 216000000, 432, 26, 2, 8},
	{ 12000000, 432000000, 432, 12, 1, 8},
	{ 13000000, 432000000, 432, 13, 1, 8},
	{ 19200000, 432000000, 90,   4, 1, 1},
	{ 26000000, 432000000, 432, 26, 1, 8},
	{ 0, 0, 0, 0, 0, 0 },
};

static struct clk tegra_pll_p = {
	.name      = "pll_p",
	.flags     = ENABLE_ON_INIT | PLL_FIXED | PLL_HAS_CPCON,
	.ops       = &tegra_pll_ops,
	.reg       = 0xa0,
	.input_min = 2000000,
	.input_max = 31000000,
	.parent    = &tegra_clk_m,
	.cf_min    = 1000000,
	.cf_max    = 6000000,
	.vco_min   = 20000000,
	.vco_max   = 1400000000,
	.pll_table = tegra_pll_p_table,
	.max_rate  = 432000000,
};

static struct clk_mux_sel mux_pllm_pllc_pllp_plla[] = {
	{ .input = &tegra_pll_m, .value = 0},
	{ .input = &tegra_pll_p, .value = 2},
	{ 0, 0},
};

static struct clk_mux_sel mux_pllp_plld_pllc_clkm[] = {
	{.input = &tegra_pll_p, .value = 0},
	{.input = &tegra_clk_m, .value = 3},
	{ 0, 0},
};

static struct clk_mux_sel mux_pllp_pllc_audio_clkm_clk32[] = {
	{.input = &tegra_pll_p, .value = 0},
	{.input = &tegra_clk_m, .value = 3},
	{.input = &tegra_clk_32k, .value = 4},
	{ 0, 0},
};

#define PERIPH_CLK(_name, _dev, _con, _clk_num, _reg, _max, _inputs, _flags) \
	{						\
		.name      = _name,			\
		.lookup    = {				\
			.dev_id    = _dev,		\
			.con_id	   = _con,		\
		},					\
		.ops       = &tegra_periph_clk_ops,	\
		.clk_num   = _clk_num,			\
		.reg       = _reg,			\
		.inputs    = _inputs,			\
		.flags     = _flags,			\
		.max_rate  = _max,			\
	}

struct clk tegra_periph_clks[] = {
	PERIPH_CLK("pwm",	"pwm",		NULL,	17,	0x110,	432000000, mux_pllp_pllc_audio_clkm_clk32,	MUX | DIV_U71 | PERIPH_SOURCE_CLK_4BIT),
	PERIPH_CLK("3d",	"3d",		NULL,	24,	0x158,	300000000, mux_pllm_pllc_pllp_plla,	MUX | DIV_U71 | PERIPH_MANUAL_RESET), /* scales with voltage and process_id */
	PERIPH_CLK("2d",	"2d",		NULL,	21,	0x15c,	300000000, mux_pllm_pllc_pllp_plla,	MUX | DIV_U71), /* scales with voltage and process_id */
	PERIPH_CLK("host1x",	"host1x",	NULL,	28,	0x180,	166000000, mux_pllm_pllc_pllp_plla,	MUX | DIV_U71), /* scales with voltage and process_id */
	PERIPH_CLK("disp1",	"tegradc.0",	NULL,	27,	0x138,	600000000, mux_pllp_plld_pllc_clkm,	MUX | DIV_U71), /* scales with voltage and process_id */
};

#define CLK(dev, con, ck)	\
	{			\
		.dev_id = dev,	\
		.con_id = con,	\
		.clk = ck,	\
	}

struct clk_lookup tegra_clk_lookups[] = {
	/* external root sources */
	CLK(NULL,	"32k_clk",	&tegra_clk_32k),
	CLK(NULL,	"clk_m",	&tegra_clk_m),
	CLK(NULL,	"pll_m",	&tegra_pll_m),
	CLK(NULL,	"pll_p",	&tegra_pll_p),
};

void tegra2_init_clocks(void)
{
	int i;
	struct clk_lookup *cl;
	struct clk *c;

	for (i = 0; i < ARRAY_SIZE(tegra_clk_lookups); i++) {
		cl = &tegra_clk_lookups[i];
		clk_init(cl->clk);
	}

	for (i = 0; i < ARRAY_SIZE(tegra_periph_clks); i++) {
		c = &tegra_periph_clks[i];
		cl = &c->lookup;
		cl->clk = c;

		clk_init(cl->clk);
	}
}

struct clk *tegra_get_clock_by_name(const char *name)
{
	struct clk *c, *ret;
	int size, i;
	struct clk_lookup *cl;
	size =  ARRAY_SIZE(tegra_clk_lookups);
	for (i = 0; i < size; i++) {
		cl = &tegra_clk_lookups[i];
		if (strcmp(cl->clk->name, name) == 0) {
			ret = cl->clk;
			return ret;
		}
	}
	size = ARRAY_SIZE(tegra_periph_clks);
	for (i = 0; i < size; i++) {
		c = &tegra_periph_clks[i];
		if (strcmp(c->name, name) == 0) {
			ret = c;
			return ret;
		}
	}
	return NULL;
}

static struct clk *tegra_get_children_clock(struct clk *c)
{
	struct clk *ck, *ret;
	int size, i;
	struct clk_lookup *cl;
	size =  ARRAY_SIZE(tegra_clk_lookups);
	for (i = 0; i < size; i++) {
		cl = &tegra_clk_lookups[i];
		if (cl->clk->parent == NULL)
			continue;
		if (strcmp(cl->clk->parent->name, c->name) == 0) {
			ret = cl->clk;
			return ret;
		}
	}
	size = ARRAY_SIZE(tegra_periph_clks);
	for (i = 0; i < size; i++) {
		ck = &tegra_periph_clks[i];
		if (ck->parent == NULL)
			continue;
		if (strcmp(ck->parent->name, c->name) == 0) {
			ret = ck;
			return ret;
		}
	}
	return NULL;
}

static int tegra_clk_init_one_from_table(struct tegra_clk_init_table *table)
{
	struct clk *c;
	struct clk *p;

	int ret = 0;

	c = tegra_get_clock_by_name(table->name);

	if (!c) {
		printf("Unable to initialize clock %s\n",
			table->name);
		return -1;
	}

	if (table->parent) {
		p = tegra_get_clock_by_name(table->parent);
		if (!p) {
			printf("Unable to find parent %s of clock %s\n",
				table->parent, table->name);
			return -1;
		}

		if (c->parent != p) {
			ret = clk_set_parent(c, p);
			if (ret) {
			     printf("Unable to set parent %s of clock %s: %d\n",
					table->parent, table->name, ret);
				return -1;
			}
		}
	}

	if (table->rate && table->rate != clk_get_rate(c)) {
		ret = clk_set_rate(c, table->rate);
		if (ret) {
			printf("Unable to set clock %s to rate %lu: %d\n",
				table->name, table->rate, ret);
			return -1;
		}
	}

	if (table->enabled) {
		ret = clk_enable(c);
		if (ret) {
			printf("Unable to enable clock %s: %d\n",
				table->name, ret);
			return -1;
		}
	}

	return 0;
}

void tegra_clk_init_from_table(struct tegra_clk_init_table *table)
{
	for (; table->name; table++)
		tegra_clk_init_one_from_table(table);
}

void tegra_clk_common_init(void)
{
	tegra2_init_clocks();
	tegra_clk_init_from_table(common_clk_init_table);
}
