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

#ifndef _COMMON_BOARD_H_
#define _COMMON_BOARD_H_

#define NV_ADDRESS_MAP_PPSB_CLK_RST_BASE 0x60006000
#define NVRM_PLLP_FIXED_FREQ_KHZ         (216000)
#define NV_ADDRESS_MAP_PPSB_TMRUS_BASE  0x60005010

#define NV_DEFAULT_DEBUG_BAUD 115200
#define NV_ADDRESS_MAP_APB_MISC_BASE    0x70000000

#define NV_FLD_MASK(d, r, f) NV_FIELD_SHIFTMASK(d##_##r##_0_##f##_RANGE)

#define NV_MISC_READ(Reg, value)                                              \
    do                                                                        \
    {                                                                         \
        value = NV_READ32(NV_ADDRESS_MAP_APB_MISC_BASE + APB_MISC_##Reg##_0); \
    } while (0)

#define NV_MISC_WRITE(Reg, value)                                             \
    do                                                                        \
    {                                                                         \
        NV_WRITE32((NV_ADDRESS_MAP_APB_MISC_BASE + APB_MISC_##Reg##_0),       \
                   value);                                                    \
    } while (0)

#define NV_CLK_RST_READ(reg, value)                                           \
    do                                                                        \
    {                                                                         \
        value = NV_READ32(NV_ADDRESS_MAP_PPSB_CLK_RST_BASE                    \
                          + CLK_RST_CONTROLLER_##reg##_0);                    \
    } while (0)

#define NV_CLK_RST_WRITE(reg, value)                                          \
    do                                                                        \
    {                                                                         \
        NV_WRITE32((NV_ADDRESS_MAP_PPSB_CLK_RST_BASE                          \
                    + CLK_RST_CONTROLLER_##reg##_0), value);                  \
    } while (0)

#define CONFIG(regt, regm, field, value)                                      \
    do                                                                        \
    {                                                                         \
        NvU32 Reg;                                                            \
        NV_MISC_READ(PP_PIN_MUX_CTL_##regm, Reg);                             \
        Reg = NV_FLD_SET_DRF_DEF(APB_MISC, PP_PIN_MUX_CTL_##regm,             \
                                 field##_SEL, value, Reg);                    \
        NV_MISC_WRITE(PP_PIN_MUX_CTL_##regm, Reg);                            \
        NV_MISC_READ(PP_TRISTATE_REG_##regt, Reg);                            \
        Reg = NV_FLD_SET_DRF_DEF(APB_MISC, PP_TRISTATE_REG_##regt,            \
                                 Z_##field, NORMAL, Reg);                     \
        NV_MISC_WRITE(PP_TRISTATE_REG_##regt, Reg);                           \
    } while (0)

#define NV_ADDRESS_MAP_USB_BASE  0xC5000000
#define NV_ADDRESS_MAP_USB3_BASE 0xC5008000

#define USB2D_USBCMD		0x140
#define ICUSB_CTRL		0x16C
#define PORTSC1			0x184
#define USB_SUSP_CTRL		0x400
#define USB_PHY_VBUS_SENSORS_0	0x404
#define USB1_LEGACY_CTRL	0x410
#define UTMIP_PLL_CFG1		0x804
#define UTMIP_XCVR_CFG0		0x808
#define UTMIP_HSRX_CFG0		0x810
#define UTMIP_HSRX_CFG1		0x814
#define UTMIP_TX_CFG0		0x820
#define UTMIP_MISC_CFG1		0x828
#define UTMIP_DEBOUNCE_CFG0	0x82C
#define UTMIP_BAT_CHRG_CFG0	0x830
#define UTMIP_SPARE_CFG0	0x834
#define UTMIP_XCVR_CFG1		0x838
#define UTMIP_BIAS_CFG1		0x83C

/**
 * Structure defining the fields for USB UTMI clocks delay Parameters.
 */
typedef struct UsbPllDelayParamsRec
{
    // Pll-U Enable Delay Count
    NvU8 EnableDelayCount;
    //PLL-U Stable count
    NvU8 StableCount;
    //Pll-U Active delay count
    NvU8 ActiveDelayCount;
    //PLL-U Xtal frequency count
    NvU8 XtalFreqCount;
} UsbPllDelayParams;

/**
 * Structure defining the fields for USB PLLU configuration Parameters.
 */
typedef struct UsbPllClockParamsRec
{
    //PLL feedback divider.
    NvU32 N;
    //PLL input divider.
    NvU32 M;
    //post divider (2^n)
    NvU32 P;
    //Base PLLC charge pump setup control
    NvU32 CPCON;
    //Base PLLC loop filter setup control.
    NvU32 LFCON;
} UsbPllClockParams;

#define Bit0  0x00000001
#define Bit1  0x00000002
#define Bit2  0x00000004
#define Bit3  0x00000008
#define Bit4  0x00000010
#define Bit5  0x00000020
#define Bit6  0x00000040
#define Bit7  0x00000080
#define Bit8  0x00000100
#define Bit9  0x00000200
#define Bit10 0x00000400
#define Bit11 0x00000800
#define Bit12 0x00001000
#define Bit13 0x00002000
#define Bit14 0x00004000
#define Bit15 0x00008000
#define Bit16 0x00010000
#define Bit17 0x00020000
#define Bit18 0x00040000
#define Bit19 0x00080000
#define Bit20 0x00100000
#define Bit21 0x00200000
#define Bit22 0x00400000
#define Bit23 0x00800000
#define Bit24 0x01000000
#define Bit25 0x02000000
#define Bit26 0x04000000
#define Bit27 0x08000000
#define Bit28 0x10000000
#define Bit29 0x20000000
#define Bit30 0x40000000
#define Bit31 0x80000000

void NvBlUartClockInit(NvU32 reset_register,
		       NvU32 reset_mask,
		       NvU32 reset_enable,
		       NvU32 reset_disable,
		       NvU32 clock_register,
		       NvU32 clock_mask,
		       NvU32 clock_enable,
		       NvU32 clock_source_register,
		       NvU32 clock_source);
void NvBlUartInitBase(NvU8 * uart_base);
NvU32 NvBlUartRxReady(NvU8 const * uart_base);
NvU32 NvBlUartRx(NvU8 const * uart_base);

void NvBlAvpStallUs(NvU32 MicroSec);
void NvBlAvpStallMs(NvU32 MilliSec);

#endif /* _COMMON_BOARD_H_ */
