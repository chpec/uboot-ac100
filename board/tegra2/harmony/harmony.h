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

#ifndef _HARMONY_H_
#define _HARMONY_H_

#define NV_ADDRESS_MAP_PPSB_CLK_RST_BASE 0x60006000
#define NVRM_PLLP_FIXED_FREQ_KHZ         (216000) 
#define NV_ADDRESS_MAP_PPSB_TMRUS_BASE  0x60005010

#define NV_DEFAULT_DEBUG_BAUD 115200
#define NV_ADDRESS_MAP_APB_MISC_BASE    0x70000000

// 8 bit access to UART A.
#define NV_UARTA_READ(Reg, value)                                             \
    do                                                                        \
    {                                                                         \
        value = NV_READ8(NV_ADDRESS_MAP_APB_UARTA_BASE + UART_##Reg##_0);     \
    } while (0)

// 8 bit access to UART A.
#define NV_UARTA_WRITE(Reg, value)                                            \
    do                                                                        \
    {                                                                         \
        NV_WRITE08((NV_ADDRESS_MAP_APB_UARTA_BASE + UART_##Reg##_0), value);  \
    } while (0)

// 8 bit access to UART D.
#define NV_UARTD_READ(Reg, value)                                             \
    do                                                                        \
    {                                                                         \
        value = NV_READ8(NV_ADDRESS_MAP_APB_UARTD_BASE + UART_##Reg##_0);     \
    } while (0)

// 8 bit access to UART D.
#define NV_UARTD_WRITE(Reg, value)                                            \
    do                                                                        \
    {                                                                         \
        NV_WRITE08((NV_ADDRESS_MAP_APB_UARTD_BASE + UART_##Reg##_0), value);  \
    } while (0)

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

const tegra2_sysinfo sysinfo = {
	"Tegra2 Harmony board",
#if defined(CONFIG_ENV_IS_IN_ONENAND)
	"OneNAND",
#else
	"NAND",
#endif
};

#define NV_ADDRESS_MAP_USB_BASE  0xC5000000
#define NV_ADDRESS_MAP_USB3_BASE 0xC5008000

#define ICUSB_CTRL		0x16C
#define PORTSC1			0x184
#define USB_SUSP_CTRL		0x400
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
///////////////////////////////////////////////////////////////////////////////
// PLL CONFIGURATION & PARAMETERS for different clock generators:
//-----------------------------------------------------------------------------
// Reference frequency     13.0MHz         19.2MHz         12.0MHz     26.0MHz 
// ----------------------------------------------------------------------------
// PLLU_ENABLE_DLY_COUNT   02 (02h)        03 (03h)        02 (02h)    04 (04h)
// PLLU_STABLE_COUNT       51 (33h)        75 (4Bh)        47 (2Fh)   102 (66h)
// PLL_ACTIVE_DLY_COUNT    05 (05h)        06 (06h)        04 (04h)    09 (09h)
// XTAL_FREQ_COUNT        127 (7Fh)       187 (BBh)       118 (76h)   254 (FEh)
///////////////////////////////////////////////////////////////////////////////
static const UsbPllDelayParams s_UsbPllDelayParams[NvBootClocksOscFreq_Num] =
{
    //ENABLE_DLY,  STABLE_CNT,  ACTIVE_DLY,  XTAL_FREQ_CNT
    {0x02,         0x33,        0x05,        0x7F}, // For NvBootClocksOscFreq_13,
    {0x03,         0x4B,        0x06,        0xBB}, // For NvBootClocksOscFreq_19_2
    {0x02,         0x2F,        0x04,        0x76}, // For NvBootClocksOscFreq_12
    {0x04,         0x66,        0x09,        0xFE}  // For NvBootClocksOscFreq_26
};

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

///////////////////////////////////////////////////////////////////////////////////////////////////
//  PLLU configuration information (reference clock is osc/clk_m and PLLU-FOs are fixed at 12MHz/60MHz/480MHz).
//
//  reference frequency         13.0MHz         19.2MHz         12.0MHz         26.0MHz    
//  ---------------------------------------------------------------------------------------
//      DIVN                    960 (3c0h)      200 (0c8h)      960 (3c0h)      960 (3c0h)
//      DIVM                    13 ( 0dh)        4 ( 04h)       12 ( 0ch)       26 ( 1ah)
// Filter frequency (MHz)       1               4.8             6           2  
// CPCON                        1100b           0011b           1100b       1100b
// LFCON0                       0               0               0           0 
///////////////////////////////////////////////////////////////////////////////
static const UsbPllClockParams s_UsbPllBaseInfo[NvBootClocksOscFreq_Num] = 
{
    //DivN, DivM, DivP, CPCON,  LFCON
    {0x3C0, 0x0D, 0x00, 0xC,      0}, // For NvBootClocksOscFreq_13,
    {0x0C8, 0x04, 0x00, 0x3,      0}, // For NvBootClocksOscFreq_19_2
    {0x3C0, 0x0C, 0x00, 0xC,      0}, // For NvBootClocksOscFreq_12
    {0x3C0, 0x1A, 0x00, 0xC,      0}  // For NvBootClocksOscFreq_26
}; 

///////////////////////////////////////////////////////////////////////////////
// Debounce values IdDig, Avalid, Bvalid, VbusValid, VbusWakeUp, and SessEnd. 
// Each of these signals have their own debouncer and for each of those one out
// of 2 debouncing times can be chosen (BIAS_DEBOUNCE_A or BIAS_DEBOUNCE_B.)
//
// The values of DEBOUNCE_A and DEBOUNCE_B are calculated as follows:
// 0xffff -> No debouncing at all
// <n> ms = <n> *1000 / (1/19.2MHz) / 4
// So to program a 1 ms debounce for BIAS_DEBOUNCE_A, we have:
// BIAS_DEBOUNCE_A[15:0] = 1000 * 19.2 / 4  = 4800 = 0x12c0
// We need to use only DebounceA for BOOTROM. We dont need the DebounceB 
// values, so we can keep those to default.
///////////////////////////////////////////////////////////////////////////////
static const NvU32 s_UsbBiasDebounceATime[NvBootClocksOscFreq_Num] =
{
    /* Ten milli second delay for BIAS_DEBOUNCE_A */
    0x7EF4,  // For NvBootClocksOscFreq_13,
    0xBB80,  // For NvBootClocksOscFreq_19_2
    0x7530,  // For NvBootClocksOscFreq_12
    0xFDE8   // For NvBootClocksOscFreq_26
};

static const NvU32 s_UsbBiasTrkLengthTime[NvBootClocksOscFreq_Num] = 
{
    /* 20 micro seconds delay after bias cell operation */
    5,  // For NvBootClocksOscFreq_13,
    7,  // For NvBootClocksOscFreq_19_2
    5,  // For NvBootClocksOscFreq_12
    9   // For NvBootClocksOscFreq_26
};

/* UTMIP Idle Wait Delay */
static const NvU8 s_UtmipIdleWaitDelay    = 17;
/* UTMIP Elastic limit */
static const NvU8 s_UtmipElasticLimit     = 16;
/* UTMIP High Speed Sync Start Delay */
static const NvU8 s_UtmipHsSyncStartDelay = 9;

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

#endif /* _HARMONY_H_ */