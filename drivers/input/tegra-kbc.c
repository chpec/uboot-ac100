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
#include <malloc.h>
#include <stdio_dev.h>

#define DEVNAME "tegra-kbc"

#define KBC_MAX_GPIO 24
#define KBC_MAX_KPENT 8
#define KBC_MAX_ROW 16
#define KBC_MAX_COL 8

#define KBC_MAX_KEY (KBC_MAX_ROW*KBC_MAX_COL)

#define TEGRA_KBC_BASE 0x7000E200

#define KBC_CONTROL_0	0
#define KBC_INT_0	4
#define KBC_ROW_CFG0_0	8
#define KBC_COL_CFG0_0	0x18
#define KBC_RPT_DLY_0	0x2c
#define KBC_KP_ENT0_0	0x30
#define KBC_KP_ENT1_0	0x34
#define KBC_ROW0_MASK_0	0x38

#define KBC_RPT_DLY 20
#define KBC_RPT_RATE 4

#define readl(addr) (*(volatile unsigned int *)(addr))
#define writel(b, addr) ((*(volatile unsigned int *) (addr)) = (b))
#define kbc_readl(addr) readl(TEGRA_KBC_BASE + addr)
#define kbc_writel(b, addr) writel(b, TEGRA_KBC_BASE + addr)

/* Define function and shift keys to untypable ASCII values */
#define KEY_FN 222
#define KEY_SHIFT 223

#ifdef CONFIG_SYS_CONSOLE_OVERWRITE_ROUTINE
extern int overwrite_console(void);
#define OVERWRITE_CONSOLE overwrite_console()
#else
#define OVERWRITE_CONSOLE 0
#endif /* CONFIG_SYS_CONSOLE_OVERWRITE_ROUTINE */

extern void config_kbc_pinmux(void);
extern void config_kbc_clock(void);

enum bool {false = 0, true = 1};

struct tegra_kbc {
	void *mmio;
	unsigned int repoll_time;
	unsigned int debounce_cnt;
	unsigned int rpt_cnt;
	int *plain_keycode;
	int *fn_keycode;
	int *shift_keycode;
};

struct tegra_kbc *kbc;

static int plain_kbd_keycode[] = {
	0, 0, 'w', 's', 'a', 'z', 0, KEY_FN,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	'5', '4', 'r', 'e', 'f', 'd', 'x', 0,
	'7', '6', 't', 'h', 'g', 'v', 'c', ' ',
	'9', '8', 'u', 'y', 'j', 'n', 'b', '\\',
	'-', '0', 'o', 'i', 'l', 'k', ',', 'm',
	0, '=', ']', '\r', 0, 0, 0, 0,
	0, 0, 0, 0, KEY_SHIFT, KEY_SHIFT, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	'[', 'p', '\'', ';', '/', '.', 0, 0,
	0, 0, 0x08, '3', '2', 0, 0, 0,
	0, 0x7F, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 'q', 0, 0, '1', 0,
	0x1B, '`', 0, 0x9, 0, 0, 0, 0
};

static int shift_kbd_keycode[] = {
	0, 0, 'W', 'S', 'A', 'Z', 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	'%', '$', 'R', 'E', 'F', 'D', 'X', 0,
	'&', '^', 'T', 'H', 'G', 'V', 'C', ' ',
	'(', '*', 'U', 'Y', 'J', 'N', 'B', '|',
	'_', ')', 'O', 'I', 'L', 'K', ',', 'M',
	0, '+', '}', '\r', 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	'{', 'P', '"', ':', '?', '>', 0, 0,
	0, 0, 0x08, '#', '@', 0, 0, 0,
	0, 0x7F, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 'Q', 0, 0, '!', 0,
	0x1B, '~', 0, 0x9, 0, 0, 0, 0
};

static int fn_kbd_keycode[] = {
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	'7', 0, 0, 0, 0, 0, 0, 0,
	'9', '8', '4', 0, '1', 0, 0, 0,
	0, '/', '6', '5', '3', '2', 0, '0',
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, '\'', 0, '-', '+', '.', 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, '?', 0, 0, 0
};

void msleep(int a)
{
	int i;

	for (i = 0; i < a; i++)
		udelay(1000);
}

static int tegra_kbc_keycode(struct tegra_kbc *kbc, int r, int c, int spl_key)
{
	if (spl_key == KEY_FN)
		return kbc->fn_keycode[(r * KBC_MAX_COL) + c];
	else if (spl_key == KEY_SHIFT)
		return kbc->shift_keycode[(r * KBC_MAX_COL) + c];
	else
		return kbc->plain_keycode[(r * KBC_MAX_COL) + c];
}

static int tegra_kbc_find_keys(int *fifo)
{
	int rows_val[KBC_MAX_KPENT], cols_val[KBC_MAX_KPENT];
	u32 kp_ent_val[(KBC_MAX_KPENT + 3) / 4];
	u32 *kp_ents = kp_ent_val;
	u32 kp_ent = 0;
	int i, j, valid = 0;
	int spl = 0;

	for (i = 0; i < ARRAY_SIZE(kp_ent_val); i++)
		kp_ent_val[i] = kbc_readl(KBC_KP_ENT0_0 + (i*4));

	valid = 0;
	for (i = 0; i < KBC_MAX_KPENT; i++) {
		if (!(i&3))
			kp_ent = *kp_ents++;

		if (kp_ent & 0x80) {
			cols_val[valid] = kp_ent & 0x7;
			rows_val[valid++] = (kp_ent >> 3) & 0xf;
		}
		kp_ent >>= 8;
	}

	for (i = 0; i < valid; i++) {
		int k = tegra_kbc_keycode(kbc, rows_val[i], cols_val[i], 0);
		if ((k == KEY_FN) || (k == KEY_SHIFT)) {
			spl = k;
			break;
		}
	}

	j = 0;
	for (i = 0; i < valid; i++) {
		int k = tegra_kbc_keycode(kbc, rows_val[i], cols_val[i], spl);
		if (k != -1)
			fifo[j++] = k;
	}

	return j;
}

static unsigned char tegra_kbc_get_char(void)
{
	u32 val, ctl;
	int fifo[KBC_MAX_KPENT], i, cnt;
	char c = 0;

	for (i = 0; i < KBC_MAX_KPENT; i++)
		fifo[i] = 0;

	/* until all keys are released, defer further processing to
	 * the polling loop in tegra_kbc_key_repeat */
	ctl = kbc_readl(KBC_CONTROL_0);
	ctl &= ~(1<<3);
	kbc_writel(ctl, KBC_CONTROL_0);

	/* quickly bail out & reenable interrupts if the interrupt source
	 * wasn't fifo count threshold */
	val = kbc_readl(KBC_INT_0);
	kbc_writel(val, KBC_INT_0);

	if (!(val & (1<<2))) {
		ctl |= 1<<3;
		kbc_writel(ctl, KBC_CONTROL_0);
		return 0;
	}

	val = (val >> 4) & 0xf;
	if (val) {
		cnt = tegra_kbc_find_keys(fifo);

		/*
		 * Get to the firxt non-zero key value in the key fifo.
		 * The FN and Shift keys will appear as zero values.
		 * The U-boot upper layers can accept only one key.
		 */
		for (i = 0; i < cnt; i++) {
			if (fifo[i]) {
				c = fifo[i];
				break;
			}
		}
	}

	msleep((val == 1) ? kbc->repoll_time : 1);

	ctl |= (1<<3);
	kbc_writel(ctl, KBC_CONTROL_0);

	return c;
}

static int kbd_fetch_char(int loop)
{
	unsigned char c;
	static unsigned char prev_c;
	static unsigned int rpt_dly = KBC_RPT_DLY;

	do {
		c = tegra_kbc_get_char();
		if (!c) {
			prev_c = 0;
			continue;
		}

		/* This logic takes care of the repeat rate */
		if ((c != prev_c) || !(rpt_dly--))
			break;
	} while (loop);

	if (c == prev_c) {
		/* kbc_testc should return 0 to indicate repeat charachters */
		if (!loop)
			c = 0;
		else
			rpt_dly = KBC_RPT_RATE;
	} else {
		rpt_dly = KBC_RPT_DLY;
		prev_c = c;
	}

	return c;
}

static int kbd_testc(void)
{
	unsigned char c = kbd_fetch_char(false);

	return (c != 0);
}

static int kbd_getc(void)
{
	unsigned char c = kbd_fetch_char(true);

	return c;
}

static void config_kbc(void)
{
	int i;

	for (i = 0; i < KBC_MAX_GPIO; i++) {
		u32 row_cfg, col_cfg;
		u32 r_shift = 5 * (i%6);
		u32 c_shift = 4 * (i%8);
		u32 r_mask = 0x1f << r_shift;
		u32 c_mask = 0xf << c_shift;
		u32 r_offs = (i / 6) * 4 + KBC_ROW_CFG0_0;
		u32 c_offs = (i / 8) * 4 + KBC_COL_CFG0_0;

		row_cfg = kbc_readl(r_offs);
		col_cfg = kbc_readl(c_offs);

		row_cfg &= ~r_mask;
		col_cfg &= ~c_mask;

		if (i < KBC_MAX_ROW)
			row_cfg |= ((i << 1) | 1) << r_shift;
		else
			col_cfg |= (((i - KBC_MAX_ROW) << 1) | 1) << c_shift;

		kbc_writel(row_cfg, r_offs);
		kbc_writel(col_cfg, c_offs);
	}
}

static int tegra_kbc_open(void)
{
	u32 val = 0;

	config_kbc();

	kbc_writel(kbc->rpt_cnt, KBC_RPT_DLY_0);

	val = kbc->debounce_cnt << 4;
	val |= 1<<14; /* fifo interrupt threshold = 1 entry */
	val |= 1<<3;  /* interrupt on FIFO threshold reached */
	val |= 1;     /* enable */
	kbc_writel(val, KBC_CONTROL_0);

	/*
	 * atomically clear out any remaining entries in the key FIFO
	 * and enable keyboard interrupts.
	 */
	while (1) {
		val = kbc_readl(KBC_INT_0);
		val >>= 4;
		if (val) {
			val = kbc_readl(KBC_KP_ENT0_0);
			val = kbc_readl(KBC_KP_ENT1_0);
		} else {
			break;
		}
	}
	kbc_writel(0x7, KBC_INT_0);

	return 0;
}

int drv_keyboard_init(void)
{
	int error;
	struct stdio_dev kbddev;
	char *stdinname;

	config_kbc_pinmux();
	config_kbc_clock();

	kbc = malloc(sizeof(*kbc));
	if (!kbc)
		return -1;

	kbc->debounce_cnt = 2;
	kbc->rpt_cnt = 5 * 32;
	kbc->debounce_cnt = min(kbc->debounce_cnt, 0x3fful);
	kbc->repoll_time = 5 + (16 + kbc->debounce_cnt) * 0x10 + kbc->rpt_cnt;
	kbc->repoll_time = (kbc->repoll_time + 31) / 32;

	kbc->plain_keycode = plain_kbd_keycode;
	kbc->fn_keycode = fn_kbd_keycode;
	kbc->shift_keycode = shift_kbd_keycode;

	stdinname = getenv("stdin");
	memset(&kbddev, 0, sizeof(kbddev));
	strcpy(kbddev.name, DEVNAME);
	kbddev.flags = DEV_FLAGS_INPUT | DEV_FLAGS_SYSTEM;
	kbddev.putc = NULL;
	kbddev.puts = NULL;
	kbddev.getc = kbd_getc;
	kbddev.tstc = kbd_testc;
	kbddev.start = tegra_kbc_open;

	error = stdio_register(&kbddev);
	if (!error) {
		/* check if this is the standard input device*/
		if (strcmp(stdinname, DEVNAME) == 0) {
			/* reassign the console */
			if (OVERWRITE_CONSOLE)
				return 1;

			error = console_assign(stdin, DEVNAME);
			if (!error)
				return 0;
			else
				return error;
		}
		return 1;
	}

	return error;
}
