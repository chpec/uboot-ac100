/*
 * Copyright (c) 2009-2010, Code Aurora Forum. All rights reserved.
 *
 * (C) Copyright 2001-2002
 * Wolfgang Denk, DENX Software Engineering -- wd@denx.de
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

#include <common.h>
#include <lcd.h>
#include <video_font.h>
#ifdef USE_PROC_COMM
#include <asm/arch/proc_comm_clients.h>
#endif /*USE_PROC_COMM */

#ifdef CONFIG_LCD
DECLARE_GLOBAL_DATA_PTR;

int lcd_line_length;
int lcd_color_fg;
int lcd_color_bg;

void *lcd_base;			/* Start of framebuffer memory	*/
void *lcd_console_address;		/* Start of console buffer	*/

short console_col;
short console_row;

vidinfo_t panel_info = {
	vl_col:		LCDC_vl_col,
	vl_row:		LCDC_vl_row,
	vl_sync_width:  LCDC_vl_sync_width,
	vl_sync_height:	LCDC_vl_sync_height,
	vl_hbp:		LCDC_vl_hbp,
	vl_hfp:		LCDC_vl_hfp,
	vl_vbp:		LCDC_vl_vbp,
	vl_vfp:		LCDC_vl_vfp,
	vl_hsync_width:	LCDC_vl_hsync_width,
	vl_vsync_width:	LCDC_vl_vsync_width,
	vl_bpix:	LCD_BPP
};

void lcd_disable (void);
void lcd_enable (void);
void lcdc_init (void);
void lcdc_initpalette (void);
pixel_24bpp_t lcdc_getcolor (ushort regno);

extern void board_lcd_enable (void);
extern void board_lcd_disble (void);

/*
 *  Print test pattern if CONFIG_LCD_TESTPATTERN is defined.
 *  Assumes init was completed successfully
 */
static void lcdc_test_pattern(void)
{
    volatile unsigned char *ptr = (unsigned char *) gd->fb_base; //test pointer
#ifdef DEBUG
    printf("Displaying LCD Test pattern...to %x\n", (unsigned int) gd->fb_base);
#endif
   memset((void *)ptr, 0xff, panel_info.vl_col*panel_info.vl_row*3);
}

/*
 *  Main init function called by lcd driver.
 *  Inits and then prints test pattern if required.
 */

void lcd_ctrl_init (void *lcdbase)
{

    lcdc_init();
    lcdc_initpalette(); //must run before lcd_clear

#ifdef CONFIG_LCD_TESTPATTERN
    lcdc_test_pattern();
    lcd_enable();
#endif

    lcd_disable();
}

/*
 *  LCDC init routine, called by the lcd_ctrl_init
 */
void lcdc_init(void)
{
   unsigned int X = 0;
   unsigned int Y = 0;
   unsigned int width = panel_info.vl_col;
   unsigned int height = panel_info.vl_row;

   /* Accesses gd->fb_base, set to LCDC_FB_ADDR in the board file
    * since the value is needed really early in drv_lcd_init()
    */
   gd->fb_base = (unsigned long) LCDC_FB_ADDR;

   int hsync_period;
   int vsync_period;

   int hactive_start_x;
   int hactive_end_x;
   int vactive_start_y;
   int vactive_end_y;

   int hsync_width;
   int vsync_width;
   int vsync_starty;
   int vsync_endy;

   hsync_period = panel_info.vl_sync_width + panel_info.vl_hfp + panel_info.vl_hbp;
   vsync_period = panel_info.vl_sync_height + panel_info.vl_vfp + panel_info.vl_vbp;
   hsync_width  = panel_info.vl_hsync_width;
   vsync_width  = panel_info.vl_vsync_width * hsync_period;
   vsync_starty = panel_info.vl_vbp * hsync_period;
   vsync_endy   = (((panel_info.vl_vbp + panel_info.vl_sync_height) * hsync_period) - 1);

   // Active area of display
   hactive_start_x = X + panel_info.vl_hbp;
   hactive_end_x   = hactive_start_x + width - 1;
   vactive_start_y = (Y + panel_info.vl_vbp) * hsync_period;
   vactive_end_y   = vactive_start_y + (height * hsync_period) - 1;

#ifdef USE_PROC_COMM
   debug("Before:: LCDC_HZ=%ul\n",proc_comm_get_lcdc_clk());
   debug("LCD_NS_REG=0x%08X\n",IO_READ32(LCD_NS_REG));
   debug("LCD_MD_REG=0x%08X\n",IO_READ32(LCD_MD_REG));
   proc_comm_set_lcdc_clk(LCD_CLK_PCOM_MHZ);
   debug("LCD_NS_REG=0x%08X\n",IO_READ32(LCD_NS_REG));
   debug("LCD_MD_REG=0x%08X\n",IO_READ32(LCD_MD_REG));
   proc_comm_enable_lcdc_pad_clk();
   debug("LCD_NS_REG=0x%08X\n",IO_READ32(LCD_NS_REG));
   debug("LCD_MD_REG=0x%08X\n",IO_READ32(LCD_MD_REG));
   proc_comm_enable_lcdc_clk();
   debug("LCD_NS_REG=0x%08X\n",IO_READ32(LCD_NS_REG));
   debug("LCD_MD_REG=0x%08X\n",IO_READ32(LCD_MD_REG));
   debug("After:: LCDC_HZ=%ul\n",proc_comm_get_lcdc_clk());

#else /* USE_PROC_COMM not defined */
   debug("LCD_NS_REG=0x%08X\n",IO_READ32(LCD_NS_REG));
   debug("LCD_MD_REG=0x%08X\n",IO_READ32(LCD_MD_REG));
   IO_WRITE32(LCD_NS_REG, LCD_NS_VAL_MHZ);
   IO_WRITE32(LCD_MD_REG, LCD_MD_VAL_MHZ);
#endif

   // Stop any previous transfers
   IO_WRITE32(MDP_LCDC_EN, 0x0);

   // Write the registers
   IO_WRITE32(MDP_LCDC_HSYNC_CTL,          ((hsync_period << 16) | hsync_width));
   IO_WRITE32(MDP_LCDC_VSYNC_PERIOD,       (vsync_period * hsync_period));
   IO_WRITE32(MDP_LCDC_VSYNC_PULSE_WIDTH,  vsync_width);
   IO_WRITE32(MDP_LCDC_DISPLAY_HCTL,       (((hsync_period - panel_info.vl_hfp - 1) << 16) | panel_info.vl_hbp));
   IO_WRITE32(MDP_LCDC_DISPLAY_V_START,    vsync_starty);
   IO_WRITE32(MDP_LCDC_DISPLAY_V_END,      vsync_endy);

   IO_WRITE32(MDP_LCDC_ACTIVE_HCTL,        ((hactive_end_x << 16) | hactive_start_x | (1 << 31)));
   IO_WRITE32(MDP_LCDC_ACTIVE_V_END,       vactive_end_y);
   IO_WRITE32(MDP_LCDC_ACTIVE_V_START,     (vactive_start_y | (1 << 31)));
   IO_WRITE32(MDP_LCDC_BORDER_CLR,         0x00000000);
   IO_WRITE32(MDP_LCDC_UNDERFLOW_CTL,      0x80000000);
   IO_WRITE32(MDP_LCDC_HSYNC_SKEW,         0x00000000);
   IO_WRITE32(MDP_LCDC_CTL_POLARITY,       0x00000000);

   // Select the DMA channel for LCDC
   // For WVGA LCDC
   IO_WRITE32(MDP_DMA_P_CONFIG,        0x0010213F);  // 0x00100000 selects LCDC, must use MDP_DMA_P
   IO_WRITE32(MDP_DMA_P_SIZE,          ((height<<16) | width));
   IO_WRITE32(MDP_DMA_P_IBUF_ADDR,     gd->fb_base);
   IO_WRITE32(MDP_DMA_P_IBUF_Y_STRIDE, width*3);
   IO_WRITE32(MDP_DMA_P_OUT_XY,        0x0);         // This must be 0
}

/*
 * Enables the LCDC by using MDP_LCDC_EN
 */
void lcd_enable (void)
{
    IO_WRITE32(MDP_LCDC_EN, 0x00000001);
    board_lcd_enable();
}

/*
 * Disables the LCDC by using MDP_LCDC_EN
 */
void lcd_disable (void)
{
    board_lcd_disble();
    IO_WRITE32(MDP_LCDC_EN, 0x00000000);
}

/*
 * Initialize the palette.
 * This routine just sets the palette base in panel_info
 */
void lcdc_initpalette (void)
{
#if LCD_BPP == LCD_COLOR24
    int disp_size_bytes = panel_info.vl_col *
                          panel_info.vl_row *
                          panel_info.vl_bpix / 8 ;

    // 4 byte aligned palette start.
	panel_info.vl_palette_base = (void *)((gd->fb_base + disp_size_bytes) % 4) +
                                               gd->fb_base + disp_size_bytes;
#endif

}


#if (LCD_BPP == LCD_COLOR8) || (LCD_BPP == LCD_COLOR24)
void lcd_setcolreg (ushort regno, ushort red, ushort green, ushort blue)
{

#if LCD_BPP == LCD_COLOR24
    pixel_24bpp_t *color = (void *) (panel_info.vl_palette_base + regno);
    color->blue  = (uchar) blue;
    color->green = (uchar) green;
    color->red   = (uchar) red;

#else
//NOT SUPPORTED YET
#endif /*LCD_BPP == LCD_COLOR24*/



}
#endif /* LCD_COLOR8, LCD_COLOR24 */

void lcdc_drawchar (ushort x, ushort y, uchar c)
{
    pixel_24bpp_t *dest;
    ushort row;
    dest = (pixel_24bpp_t *)(lcd_base + y * lcd_line_length + x * LCD_BPP / 8);

    /* lcd_line_length is in bytes, but pointer jumps in sizeof(pixel_24bpp_t) */
    for (row=0;  row < VIDEO_FONT_HEIGHT;  ++row,
            dest += (lcd_line_length / sizeof(pixel_24bpp_t)))  {
        pixel_24bpp_t *d = dest;
        uchar bits;
        int ctr;

        bits = video_fontdata[c * VIDEO_FONT_HEIGHT + row];
        for (ctr=0; ctr<8; ++ctr) {
            *d++ = (bits & 0x80) ?
                    lcdc_getcolor(lcd_color_fg):
                    lcdc_getcolor(lcd_color_bg);
            bits <<= 1;
        }

    }

    return;
}
/*
 *
*/
pixel_24bpp_t lcdc_getcolor(ushort regno)
{
    return *(pixel_24bpp_t *)(panel_info.vl_palette_base + regno);
}

#endif /*CONFIG_LCD*/
