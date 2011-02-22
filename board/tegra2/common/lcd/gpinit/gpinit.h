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

#ifndef __GRAPHIC_INIT_H
#define __GRAPHIC_INIT_H

#include <asm/arch/nvcommon.h>

#define TEGRA_DISPLAY_BASE		0x54200000
#define TEGRA_DISPLAY_SIZE		0x00040000
#define TEGRA_TMRUS_BASE		0x60005010
#define TEGRA_TMRUS_SIZE		0x00010000
#define TEGRA_CLK_RESET_BASE		0x60006000
#define TEGRA_CLK_RESET_SIZE		0x00001000
#define TEGRA_GPIO_BASE			0x6000D000
#define TEGRA_GPIO_SIZE			0x00001000
#define TEGRA_APB_MISC_BASE		0x70000000
#define TEGAR_APB_MISC_SIZE		0x00001000
#define TEGRA_PWFM0_BASE		0x7000A000
#define TEGRA_PWFM0_SIZE		0x4
#define TEGRA_PWFM1_BASE		0x7000A010
#define TEGRA_PWFM1_SIZE		0x4
#define TEGRA_PWFM2_BASE		0x7000A020
#define TEGRA_PWFM2_SIZE		0x4
#define TEGRA_PWFM3_BASE		0x7000A030
#define TEGRA_PWFM3_SIZE		0x4
#define TEGRA_PMC_BASE			0x7000E400
#define TEGRA_PMC_SIZE			0x00000100

typedef phys_addr_t resource_size_t;

typedef enum _bool {false, true} bool;

struct resource {
	resource_size_t start;
	resource_size_t end;
	const char *name;
};

struct tegra_clk_init_table {
	const char *name;
	const char *parent;
	unsigned long rate;
	bool enabled;
};

struct clk_mux_sel {
	struct clk	*input;
	u32		value;
};

struct clk_pll_table {
	unsigned long	input_rate;
	unsigned long	output_rate;
	u16		n;
	u16		m;
	u8		p;
	u8		cpcon;
};

struct clk_ops {
	void		(*init)(struct clk *);
	int		(*enable)(struct clk *);
	void		(*disable)(struct clk *);
	int		(*set_parent)(struct clk *, struct clk *);
	int		(*set_rate)(struct clk *, unsigned long);
	long		(*round_rate)(struct clk *, unsigned long);
};

enum clk_state {
	UNINITIALIZED = 0,
	ON,
	OFF,
};

struct clk_lookup {
	const char	*dev_id;
	const char	*con_id;
	struct clk	*clk;
};

struct clk {
	struct clk_ops			*ops;
	struct clk			*parent;
	struct clk_lookup		lookup;
	unsigned long			rate;
	unsigned long			max_rate;
	u32				flags;
	u32				refcnt;
	const char			*name;
	u32				reg;
	u32				reg_shift;
	unsigned int			clk_num;
	enum clk_state			state;

	/* PLL */
	unsigned long			input_min;
	unsigned long			input_max;
	unsigned long			cf_min;
	unsigned long			cf_max;
	unsigned long			vco_min;
	unsigned long			vco_max;
	const struct clk_pll_table	*pll_table;

	/* DIV */
	u32				div;
	u32				mul;

	/* MUX */
	const struct clk_mux_sel	*inputs;
	u32				sel;
	u32				reg_mask;
};

#define DC_N_WINDOWS	3

struct tegra_dc_mode {
	int pclk;
	int h_ref_to_sync;
	int v_ref_to_sync;
	int h_sync_width;
	int v_sync_width;
	int h_back_porch;
	int v_back_porch;
	int h_active;
	int v_active;
	int h_front_porch;
	int v_front_porch;
};

struct tegra_fb_data {
	int		win;
	int		xres;
	int		yres;
	int		pitch;
	int		bits_per_pixel;
};

struct tegra_dc;

struct tegra_dc_win {
	u8			idx;
	u8			fmt;
	u32			flags;

	phys_addr_t		phys_addr;
	unsigned		x;
	unsigned		y;
	unsigned		w;
	unsigned		h;
	unsigned		stride;
	unsigned		out_x;
	unsigned		out_y;
	unsigned		out_w;
	unsigned		out_h;
	unsigned		z;

	int			dirty;
	struct tegra_dc		*dc;
};

struct tegra_dc_blend {
	unsigned z[DC_N_WINDOWS];
	unsigned flags[DC_N_WINDOWS];
};

struct tegra_dc {
	phys_addr_t			base;
	int				irq;
	struct clk			*clk;
	struct clk			*host1x_clk;
	bool				enabled;
	void				*out_data;
	struct tegra_dc_mode		mode;
	int				n_windows;
	struct resource			*fb_mem;
	struct tegra_fb_data		*fb;
	struct resource			*pwm;
	struct tegra_dc_win		windows[DC_N_WINDOWS];
	struct tegra_dc_blend		blend;
};

struct tegra_gpio_init_table {
	unsigned	offset;
	bool		set;
};

/* GPIO */
#define TEGRA_GPIO_NUMBER(port, bit) (((port) << 3) | ((bit) & 7))
#define TEGRA_GPIO_PORT(gpio_number)	((gpio_number) >> 3)
#define TEGRA_GPIO_BIT(gpio_number)	((gpio_number) & 7)
/* GPIO ports */
#define TEGRA_GPIO_PB2 TEGRA_GPIO_NUMBER(1, 2) /* 10 */
#define TEGRA_GPIO_PB4 TEGRA_GPIO_NUMBER(1, 4) /* 12 */
#define TEGRA_GPIO_PB5 TEGRA_GPIO_NUMBER(1, 5) /* 13 */
#define TEGRA_GPIO_PC6 TEGRA_GPIO_NUMBER(2, 6) /* 22 */
#define TEGRA_GPIO_PC7 TEGRA_GPIO_NUMBER(2, 7) /* 23 */
#define TEGRA_GPIO_PD4 TEGRA_GPIO_NUMBER(3, 4) /* 28 */
#define TEGRA_GPIO_PH0 TEGRA_GPIO_NUMBER(7, 0) /* 56 */
#define TEGRA_GPIO_PH3 TEGRA_GPIO_NUMBER(7, 3) /* 59 */
#define TEGRA_GPIO_PU5 TEGRA_GPIO_NUMBER(20, 5) /* 165 */
#define TEGRA_GPIO_PV0 TEGRA_GPIO_NUMBER(21, 0) /* 168 */
#define TEGRA_GPIO_PV2 TEGRA_GPIO_NUMBER(21, 2) /* 170 */
#define TEGRA_GPIO_PW0 TEGRA_GPIO_NUMBER(22, 0) /* 176 */

/* Clock */
#define DIV_BUS			(1 << 0)
#define DIV_U71			(1 << 1)
#define DIV_U71_FIXED		(1 << 2)
#define DIV_2			(1 << 3)
#define DIV_U16			(1 << 4)
#define PLL_FIXED		(1 << 5)
#define PLL_HAS_CPCON		(1 << 6)
#define MUX			(1 << 7)
#define PLLD			(1 << 8)
#define PERIPH_NO_RESET		(1 << 9)
#define PERIPH_NO_ENB		(1 << 10)
#define PERIPH_EMC_ENB		(1 << 11)
#define PERIPH_MANUAL_RESET	(1 << 12)
#define PLL_ALT_MISC_REG	(1 << 13)
#define PLLU			(1 << 14)
#define PERIPH_SOURCE_CLK_4BIT	(1 << 15)
#define ENABLE_ON_INIT		(1 << 28)

void tegra_gpio_enable(int gpio);
int tegra_gpio_direction_output(unsigned offset, int value);
unsigned long clk_get_rate(struct clk *c);
int clk_enable(struct clk *c);
void clk_disable_locked(struct clk *c);
void tegra2_periph_reset_deassert(struct clk *c);
void tegra2_periph_reset_assert(struct clk *c);
void tegra2_init_clocks(void);
void tegra_clk_init_from_table(struct tegra_clk_init_table *table);
struct clk *tegra_get_clock_by_name(const char *name);
void tegra_clk_common_init(void);
int poweron_3d(void);
void tegra_dc_register(struct resource *panel,
		struct tegra_dc_mode *modes,
		struct tegra_fb_data *fb);
int tegra_dc_probe(void);
void tegra_pwm_enable(void);

void gpinit(void);

#endif /*__GRAPHIC_INIT_H*/
