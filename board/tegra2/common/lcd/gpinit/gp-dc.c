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

#include "gp-dc_reg.h"
#include "gpinit.h"
#include "../../sdmmc/nvboot_util.h"

static const u32 tegra_dc_rgb_pintable[] = {
	DC_COM_PIN_OUTPUT_ENABLE0,	0x00000000,
	DC_COM_PIN_OUTPUT_ENABLE1,	0x00000000,
	DC_COM_PIN_OUTPUT_ENABLE2,	0x00000000,
	DC_COM_PIN_OUTPUT_ENABLE3,	0x00000000,
	DC_COM_PIN_OUTPUT_POLARITY0,	0x00000000,
	DC_COM_PIN_OUTPUT_POLARITY1,	0x01000000,
	DC_COM_PIN_OUTPUT_POLARITY2,	0x00000000,
	DC_COM_PIN_OUTPUT_POLARITY3,	0x00000000,
	DC_COM_PIN_OUTPUT_DATA0,	0x00000000,
	DC_COM_PIN_OUTPUT_DATA1,	0x00000000,
	DC_COM_PIN_OUTPUT_DATA2,	0x00000000,
	DC_COM_PIN_OUTPUT_DATA3,	0x00000000,
	DC_COM_PIN_OUTPUT_SELECT0,	0x00000000,
	DC_COM_PIN_OUTPUT_SELECT1,	0x00000000,
	DC_COM_PIN_OUTPUT_SELECT2,	0x00000000,
	DC_COM_PIN_OUTPUT_SELECT3,	0x00000000,
	DC_COM_PIN_OUTPUT_SELECT4,	0x00210222,
	DC_COM_PIN_OUTPUT_SELECT5,	0x00002200,
	DC_COM_PIN_OUTPUT_SELECT6,	0x00020000,
};

#define TEGRA_WIN_FMT_P1		0
#define TEGRA_WIN_FMT_P2		1
#define TEGRA_WIN_FMT_P4		2
#define TEGRA_WIN_FMT_P8		3
#define TEGRA_WIN_FMT_B4G4R4A4		4
#define TEGRA_WIN_FMT_B5G5R5A		5
#define TEGRA_WIN_FMT_B5G6R5		6
#define TEGRA_WIN_FMT_AB5G5R5		7
#define TEGRA_WIN_FMT_B8G8R8A8		12
#define TEGRA_WIN_FMT_R8G8B8A8		13
#define TEGRA_WIN_FMT_B6x2G6x2R6x2A8	14
#define TEGRA_WIN_FMT_R6x2G6x2B6x2A8	15
#define TEGRA_WIN_FMT_YCbCr422		16
#define TEGRA_WIN_FMT_YUV422		17
#define TEGRA_WIN_FMT_YCbCr420P		18
#define TEGRA_WIN_FMT_YUV420P		19
#define TEGRA_WIN_FMT_YCbCr422P		20
#define TEGRA_WIN_FMT_YUV422P		21
#define TEGRA_WIN_FMT_YCbCr422R		22
#define TEGRA_WIN_FMT_YUV422R		23
#define TEGRA_WIN_FMT_YCbCr422RA	24
#define TEGRA_WIN_FMT_YUV422RA		25

#define TEGRA_WIN_FLAG_ENABLED		(1 << 0)
#define TEGRA_WIN_FLAG_BLEND_PREMULT	(1 << 1)
#define TEGRA_WIN_FLAG_BLEND_COVERAGE	(1 << 2)

#define TEGRA_WIN_BLEND_FLAGS_MASK \
	(TEGRA_WIN_FLAG_BLEND_PREMULT | TEGRA_WIN_FLAG_BLEND_COVERAGE)

static inline int tegra_dc_fmt_bpp(int fmt)
{
	switch (fmt) {
	case TEGRA_WIN_FMT_P1:
		return 1;

	case TEGRA_WIN_FMT_P2:
		return 2;

	case TEGRA_WIN_FMT_P4:
		return 4;

	case TEGRA_WIN_FMT_P8:
		return 8;

	case TEGRA_WIN_FMT_B4G4R4A4:
	case TEGRA_WIN_FMT_B5G5R5A:
	case TEGRA_WIN_FMT_B5G6R5:
	case TEGRA_WIN_FMT_AB5G5R5:
		return 16;

	case TEGRA_WIN_FMT_B8G8R8A8:
	case TEGRA_WIN_FMT_R8G8B8A8:
	case TEGRA_WIN_FMT_B6x2G6x2R6x2A8:
	case TEGRA_WIN_FMT_R6x2G6x2B6x2A8:
		return 32;

	case TEGRA_WIN_FMT_YCbCr422:
	case TEGRA_WIN_FMT_YUV422:
	case TEGRA_WIN_FMT_YCbCr420P:
	case TEGRA_WIN_FMT_YUV420P:
	case TEGRA_WIN_FMT_YCbCr422P:
	case TEGRA_WIN_FMT_YUV422P:
	case TEGRA_WIN_FMT_YCbCr422R:
	case TEGRA_WIN_FMT_YUV422R:
	case TEGRA_WIN_FMT_YCbCr422RA:
	case TEGRA_WIN_FMT_YUV422RA:
		/* FIXME: need to know the bpp of these formats */
		return 0;
	}
	return 0;
}

static resource_size_t resource_size(const struct resource *res)
{
	return res->end - res->start + 1;
}

static void tegra_dc_writel(struct tegra_dc *dc, unsigned long val,
				   unsigned long reg)
{
	writel(val, dc->base + reg * 4);
}

static void _tegra_dc_write_table(struct tegra_dc *dc, const u32 *table,
					 unsigned len)
{
	int i;

	for (i = 0; i < len; i++)
		tegra_dc_writel(dc, table[i * 2 + 1], table[i * 2]);
}

#define tegra_dc_write_table(dc, table)		\
	_tegra_dc_write_table(dc, table, ARRAY_SIZE(table) / 2)

#define max_t(type, x, y) ({	\
	type __max1 = (x);	\
	type __max2 = (y);	\
	__max1 > __max2 ? __max1 : __max2; })

#define BIT(n)	(1 << (n))

static void tegra_dc_set_blending(struct tegra_dc *dc,
				struct tegra_dc_blend *blend)
{
	int idx;
	idx = 0;

	tegra_dc_writel(dc, WINDOW_A_SELECT << idx,
				DC_CMD_DISPLAY_WINDOW_HEADER);
	tegra_dc_writel(dc, 0xff00, DC_WIN_BLEND_NOKEY);
	tegra_dc_writel(dc, 0xff00, DC_WIN_BLEND_1WIN);
}

static void tegra_dc_update_windows(struct tegra_dc *dc, int n)
{
	unsigned long update_mask = GENERAL_ACT_REQ;
	unsigned long val;
	int i;

	for (i = 0; i < n; i++) {
		struct tegra_dc_win *win = &dc->windows[i];
		unsigned h_dda;
		unsigned v_dda;

		dc->blend.z[win->idx] = win->z;
		dc->blend.flags[win->idx] =
				win->flags & TEGRA_WIN_BLEND_FLAGS_MASK;

		tegra_dc_writel(dc, WINDOW_A_SELECT << win->idx,
				DC_CMD_DISPLAY_WINDOW_HEADER);

		update_mask |= WIN_A_ACT_REQ << win->idx;

		if (!(win->flags & TEGRA_WIN_FLAG_ENABLED)) {
			tegra_dc_writel(dc, 0, DC_WIN_WIN_OPTIONS);
			continue;
		}

		tegra_dc_writel(dc, win->fmt, DC_WIN_COLOR_DEPTH);
		tegra_dc_writel(dc, 0, DC_WIN_BYTE_SWAP);

		h_dda = (win->w * 0x1000) / max_t(int, win->out_w - 1, 1);
		v_dda = (win->h * 0x1000) / max_t(int, win->out_h - 1, 1);

		tegra_dc_writel(dc,
				V_POSITION(win->out_y) | H_POSITION(win->out_x),
				DC_WIN_POSITION);
		tegra_dc_writel(dc,
				V_SIZE(win->out_h) | H_SIZE(win->out_w),
				DC_WIN_SIZE);
		tegra_dc_writel(dc,
				V_PRESCALED_SIZE(win->h) |
				H_PRESCALED_SIZE(win->w*
						tegra_dc_fmt_bpp(win->fmt)/8),
				DC_WIN_PRESCALED_SIZE);
		tegra_dc_writel(dc, 0, DC_WIN_H_INITIAL_DDA);
		tegra_dc_writel(dc, 0, DC_WIN_V_INITIAL_DDA);
		tegra_dc_writel(dc, V_DDA_INC(v_dda) | H_DDA_INC(h_dda),
				DC_WIN_DDA_INCREMENT);
		tegra_dc_writel(dc, win->stride, DC_WIN_LINE_STRIDE);
		tegra_dc_writel(dc, 0, DC_WIN_BUF_STRIDE);

		val = WIN_ENABLE;
		if (tegra_dc_fmt_bpp(win->fmt) < 24)
			val |= COLOR_EXPAND;
		tegra_dc_writel(dc, val, DC_WIN_WIN_OPTIONS);

		tegra_dc_writel(dc, (unsigned long)win->phys_addr,
				DC_WINBUF_START_ADDR);
		tegra_dc_writel(dc, win->x, DC_WINBUF_ADDR_H_OFFSET);
		tegra_dc_writel(dc, win->y, DC_WINBUF_ADDR_V_OFFSET);

		win->dirty = 1;
	}

	tegra_dc_set_blending(dc, &dc->blend);

	tegra_dc_writel(dc, update_mask << 8, DC_CMD_STATE_CONTROL);

	tegra_dc_writel(dc, update_mask, DC_CMD_STATE_CONTROL);

	return;
}

static int tegra_dc_program_mode(struct tegra_dc *dc,
				struct tegra_dc_mode *mode)
{
	unsigned long val;
	unsigned long rate;
	unsigned long div;

	tegra_dc_writel(dc, 0x0, DC_DISP_DISP_TIMING_OPTIONS);
	tegra_dc_writel(dc, mode->h_ref_to_sync | (mode->v_ref_to_sync << 16),
			DC_DISP_REF_TO_SYNC);
	tegra_dc_writel(dc, mode->h_sync_width | (mode->v_sync_width << 16),
			DC_DISP_SYNC_WIDTH);
	tegra_dc_writel(dc, mode->h_back_porch | (mode->v_back_porch << 16),
			DC_DISP_BACK_PORCH);
	tegra_dc_writel(dc, mode->h_active | (mode->v_active << 16),
			DC_DISP_DISP_ACTIVE);
	tegra_dc_writel(dc, mode->h_front_porch | (mode->v_front_porch << 16),
			DC_DISP_FRONT_PORCH);

	tegra_dc_writel(dc, DE_SELECT_ACTIVE | DE_CONTROL_NORMAL,
			DC_DISP_DATA_ENABLE_OPTIONS);

	val = DISP_DATA_FORMAT_DF1P1C;

	val |= DISP_DATA_ALIGNMENT_MSB;

	val |= DISP_DATA_ORDER_RED_BLUE;

	tegra_dc_writel(dc, val, DC_DISP_DISP_INTERFACE_CONTROL);

	rate = clk_get_rate(dc->clk);

	div = ((rate * 2 + mode->pclk / 2) / mode->pclk) - 2;

	tegra_dc_writel(dc, 0x00010001,
			DC_DISP_SHIFT_CLOCK_OPTIONS);
	tegra_dc_writel(dc, PIXEL_CLK_DIVIDER_PCD1 | SHIFT_CLK_DIVIDER(div),
			DC_DISP_DISP_CLOCK_CONTROL);

	return 0;
}

static int tegra_dc_set_mode(struct tegra_dc *dc,
			const struct tegra_dc_mode *mode)
{
	memcpy(&dc->mode, mode, sizeof(dc->mode));

	return 0;
}

static void tegra_dc_init(struct tegra_dc *dc)
{
	tegra_dc_writel(dc, 0x00000100, DC_CMD_GENERAL_INCR_SYNCPT_CNTRL);
	tegra_dc_writel(dc, 0x0000011a, DC_CMD_CONT_SYNCPT_VSYNC);
	tegra_dc_writel(dc, 0x00000000, DC_CMD_INT_TYPE);
	tegra_dc_writel(dc, 0x00000000, DC_CMD_INT_POLARITY);
	tegra_dc_writel(dc, 0x00000020, DC_DISP_MEM_HIGH_PRIORITY);
	tegra_dc_writel(dc, 0x00000001, DC_DISP_MEM_HIGH_PRIORITY_TIMER);

	tegra_dc_writel(dc, 0x00000000, DC_CMD_INT_MASK);
	tegra_dc_writel(dc, 0x00000000, DC_CMD_INT_ENABLE);

	if (dc->mode.pclk)
		tegra_dc_program_mode(dc, &dc->mode);
}

static void tegra_dc_rgb_enable(struct tegra_dc *dc)
{
	tegra_dc_writel(dc, PW0_ENABLE | PW1_ENABLE | PW2_ENABLE | PW3_ENABLE |
			PW4_ENABLE | PM0_ENABLE | PM1_ENABLE,
			DC_CMD_DISPLAY_POWER_CONTROL);

	tegra_dc_writel(dc, DISP_CTRL_MODE_C_DISPLAY, DC_CMD_DISPLAY_COMMAND);

	tegra_dc_write_table(dc, tegra_dc_rgb_pintable);
}

static void tegra_dc_enable(struct tegra_dc *dc)
{
	tegra2_periph_reset_deassert(dc->clk);

	tegra_dc_init(dc);

	tegra_dc_rgb_enable(dc);
}

static void tegra_fb_register(struct tegra_dc *dc,
			struct tegra_fb_data *fb_data,
			struct resource *fb_mem)
{
	struct tegra_dc_win *win;
	unsigned long fb_size = 0;
	unsigned long fb_phys = 0;

	win = &(dc->windows[fb_data->win]);

	if (fb_mem) {
		fb_size = resource_size(fb_mem);
		fb_phys = fb_mem->start;
	}

	win->x = 0;
	win->y = 0;
	win->w = fb_data->xres;
	win->h = fb_data->yres;
	win->out_x = 0;
	win->out_y = 0;
	win->out_w = fb_data->xres;
	win->out_h = fb_data->yres;
	win->z = 0;
	win->phys_addr = fb_phys;
	win->stride = fb_data->xres * fb_data->bits_per_pixel / 8;
	win->flags = TEGRA_WIN_FLAG_ENABLED;
	switch (fb_data->bits_per_pixel) {
	case 32:
	case 24:
		win->fmt = TEGRA_WIN_FMT_R8G8B8A8;
		break;
	case 16:
		win->fmt = TEGRA_WIN_FMT_B5G6R5;
		break;

	case 0:
		break;

	default:
		return;
	}
	return;
}

struct tegra_dc gp_dc;

void tegra_dc_register(struct resource *panel,
			struct tegra_dc_mode *modes,
			struct tegra_fb_data *fb)
{
	struct tegra_dc *dc;
	struct resource	*regs = NULL;

	dc = &gp_dc;

	regs = &panel[1];
	dc->base = regs->start;

	dc->fb_mem = &panel[2];

	dc->pwm = &panel[3];

	tegra_dc_set_mode(dc, &modes[0]);

	dc->fb = fb;

	return;
}

int tegra_dc_probe(void)
{
	struct tegra_dc *dc;
	int i;

	dc = &gp_dc;

	dc->host1x_clk = tegra_get_clock_by_name("host1x");;

	dc->clk = tegra_get_clock_by_name("disp1");;

	tegra_dc_enable(dc);

	dc->n_windows = DC_N_WINDOWS;
	for (i = 0; i < dc->n_windows; i++) {
		dc->windows[i].idx = i;
		dc->windows[i].dc = dc;
	}

	tegra_fb_register(dc, dc->fb, dc->fb_mem);

	tegra_dc_update_windows(dc, 1);

	return 0;
}

int poweron_3d(void)
{
	struct clk *clk;

	writel(0x101, TEGRA_PMC_BASE + 0x30);

	clk = tegra_get_clock_by_name("3d");
	tegra2_periph_reset_assert(clk);

	writel(0x101, TEGRA_PMC_BASE + 0x30);

	NvBootUtilWaitUS(10);

	writel(1 << 1, TEGRA_PMC_BASE + 0x34);

	tegra2_periph_reset_deassert(clk);

	return 0;
}

#define PWM_ENABLE (1 << 31)
void tegra_pwm_enable(void)
{
	struct tegra_dc *dc;
	u32 val;

	dc = &gp_dc;
	val = 0xdf0001;
	val |= PWM_ENABLE;
	writel(val, dc->pwm->start);
}
