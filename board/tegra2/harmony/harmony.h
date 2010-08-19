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

#endif /* _HARMONY_H_ */
