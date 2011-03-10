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

#ifndef __CONFIG_H
#define __CONFIG_H
#include <asm/sizes.h>
#include "tegra2-common.h"

/*
 * High Level Configuration Options
 */
#define TEGRA2_SYSMEM			"mem=384M@0M nvmem=128M@384M"
#define V_PROMPT			"Tegra2 (ac100) # "
#define TEGRA_NFSPORT_DEFAULT		"usb0"

#define CONFIG_SERIAL_MULTI		1
#define CONFIG_TEGRA2_ENABLE_UARTD	0
#define CONFIG_TEGRA2_ENABLE_UARTA	1
#define CONFIG_STD_DEVICES_SETTINGS

/* UARTA: keyboard satellite board uart, default */
#define CONFIG_SYS_NS16550_COM1		NV_ADDRESS_MAP_APB_UARTA_BASE

/* UARTA: debug board uart */
#define CONFIG_SYS_NS16550_COM2		NV_ADDRESS_MAP_APB_UARTA_BASE

/* These config switches are for GPIO support */
#define CONFIG_TEGRA2_GPIO		1
#define CONFIG_CMD_TEGRA2_GPIO_INFO	1

#define LINUX_MACH_TYPE			MACH_TYPE_HARMONY
#define CONFIG_SYS_BOARD_ODMDATA	0x800C0075

#define CONFIG_I2CP_PIN_MUX		1
#define CONFIG_I2C1_PIN_MUX		1
#define CONFIG_I2C2_PIN_MUX		1
#define CONFIG_I2C3_PIN_MUX		1

#define MMC_DEV_INSTANCES 2
#define NvEmmcx_0	NvEmmc4
#define NvEmmcx_1	NvEmmc2
#define NvEmmcx_2	0
#define NvEmmcx_3	0

/* To set base address of USB controller */
#define NvUSBx_0	USB_EHCI_TEGRA_BASE_ADDR_USB3
#define NvUSBx_1	USB_EHCI_TEGRA_BASE_ADDR_USB1
#define NvUSBx_2	0
#define NvUSBx_3	0

/* LCD Settings */
#ifdef CONFIG_LCD
#define CONFIG_LCD_vl_col	1024
#define CONFIG_LCD_vl_row	600
#endif

#define CONFIG_TEGRA2_PINMUX_DISPLAY_HARMONY			\
        PINMUX(LCSN,  DISPLAYA,      PULL_UP,   TRISTATE)       \
        PINMUX(LD0,   DISPLAYA,      PULL_DOWN, NORMAL)         \
        PINMUX(LD1,   DISPLAYA,      PULL_DOWN, NORMAL)         \
        PINMUX(LD10,  DISPLAYA,      PULL_DOWN, NORMAL)         \
        PINMUX(LD11,  DISPLAYA,      PULL_DOWN, NORMAL)         \
        PINMUX(LD12,  DISPLAYA,      PULL_DOWN, NORMAL)         \
        PINMUX(LD13,  DISPLAYA,      PULL_DOWN, NORMAL)         \
        PINMUX(LD14,  DISPLAYA,      PULL_DOWN, NORMAL)         \
        PINMUX(LD15,  DISPLAYA,      PULL_DOWN, NORMAL)         \
        PINMUX(LD16,  DISPLAYA,      PULL_DOWN, NORMAL)         \
        PINMUX(LD17,  DISPLAYA,      PULL_DOWN, NORMAL)         \
        PINMUX(LD2,   DISPLAYA,      PULL_DOWN, NORMAL)         \
        PINMUX(LD3,   DISPLAYA,      PULL_DOWN, NORMAL)         \
        PINMUX(LD4,   DISPLAYA,      PULL_DOWN, NORMAL)         \
        PINMUX(LD5,   DISPLAYA,      PULL_DOWN, NORMAL)         \
        PINMUX(LD6,   DISPLAYA,      PULL_DOWN, NORMAL)         \
        PINMUX(LD7,   DISPLAYA,      PULL_DOWN, NORMAL)         \
        PINMUX(LD8,   DISPLAYA,      PULL_DOWN, NORMAL)         \
        PINMUX(LD9,   DISPLAYA,      PULL_DOWN, NORMAL)         \
        PINMUX(LDI,   DISPLAYA,      PULL_DOWN, NORMAL)         \
        PINMUX(LHP0,  DISPLAYA,      PULL_DOWN, NORMAL)         \
        PINMUX(LHP1,  DISPLAYA,      PULL_DOWN, NORMAL)         \
        PINMUX(LHP2,  DISPLAYA,      PULL_DOWN, NORMAL)         \
        PINMUX(LHS,   DISPLAYA,      PULL_UP,   NORMAL)         \
        PINMUX(LM0,   DISPLAYA,      PULL_UP,   NORMAL)         \
        PINMUX(LM1,   DISPLAYA,      PULL_UP,   TRISTATE)       \
        PINMUX(LPP,   DISPLAYA,      PULL_DOWN, NORMAL)         \
        PINMUX(LPW0,  DISPLAYA,      PULL_UP,   NORMAL)         \
        PINMUX(LPW1,  DISPLAYA,      PULL_UP,   TRISTATE)       \
        PINMUX(LPW2,  DISPLAYA,      PULL_UP,   NORMAL)         \
        PINMUX(LSC0,  DISPLAYA,      PULL_UP,   NORMAL)         \
        PINMUX(LSC1,  DISPLAYA,      PULL_UP,   TRISTATE)       \
        PINMUX(LSCK,  DISPLAYA,      PULL_UP,   TRISTATE)       \
        PINMUX(LSDA,  DISPLAYA,      PULL_UP,   TRISTATE)       \
        PINMUX(LSDI,  DISPLAYA,      PULL_UP,   TRISTATE)       \
        PINMUX(LSPI,  DISPLAYA,      PULL_UP,   NORMAL)         \
        PINMUX(LVP0,  DISPLAYA,      PULL_UP,   TRISTATE)       \
        PINMUX(LVP1,  DISPLAYA,      PULL_DOWN, NORMAL)         \
        PINMUX(LVS,   DISPLAYA,      PULL_UP,   NORMAL)         \
        PINMUX(SDC,   PWM,           PULL_UP,   NORMAL)

#define CONFIG_TEGRA2_PINMUX                    \
        CONFIG_TEGRA2_PINMUX_DISPLAY_HARMONY

/* Environment information */
#define CONFIG_DEFAULT_ENV_SETTINGS \
	"tegraparts=" TEGRAPARTS_DEFAULT "\0" \
	"tegraboot=" TEGRABOOT_DEFAULT "\0" \
	"nfsport=" TEGRA_NFSPORT_DEFAULT "\0" \
	"console=ttyS0,115200n8\0" \
	"mem=" TEGRA2_SYSMEM "\0" \
	"smpflag=smp\0" \
	"videospec=tegrafb\0" \
	"mmcdev=" TEGRA2_MMC_DEFAULT_DEVICE "\0" \
	"lp0_vec=" LP0_VEC TEGRA_LP0_SIZE_STR AT TEGRA_LP0_DEFAULT_ADDR_STR "\0"

#define CONFIG_IPADDR		10.0.0.2
#define CONFIG_SERVERIP		10.0.0.1
#define CONFIG_LOADADDR		0x408000 /* free RAM to download kernel to */
#define CONFIG_BOOTFILE		vmlinux.uimg
#define TEGRA_EHCI_PROBE_DELAY_DEFAULT	"5000"
#define CONFIG_BOOTDELAY		2      /* -1 to disable auto boot */
#define CONFIG_EXTRA_ENV_SETTINGS \
	"usbprobedelay=" TEGRA_EHCI_PROBE_DELAY_DEFAULT "\0" \
	"usbhost=on\0" \
	CONFIG_DEFAULT_ENV_SETTINGS \
	CONFIG_STD_DEVICES_SETTINGS \
	"usbargs=setenv bootargs root=/dev/sda1 rw rootwait " \
		"${mem} " \
                "video=${videospec} " \
		"console=${console} " \
		"usbcore.old_scheme_first=1 " \
		"tegraboot=${tegraboot} " \
		"tegrap earlyprintk;\0"\
	"nandargs=setenv bootargs ${mem} " \
		"video=${videospec} " \
		"console=${console} " \
		"tegraboot=${tegraboot} " \
		"tegrapart=${tegraparts} " \
		"${smpflag} " \
		"${usb_old_scheme_first} " \
		"rw root=/dev/sda1 rootdelay=15;\0"\
	"mmcargs=setenv bootargs ${mem} " \
		"video=${videospec} " \
		"console=${console} " \
		"tegraboot=${tegraboot} " \
		"tegrapart=${tegraparts} " \
		"${smpflag} " \
		"${usb_old_scheme_first} " \
		"rw root=/dev/mmchd0p1 rootfstype=ext3 "\
		"ip=:::::${nfsport}:off;\0"\
	"nfsargs=setenv bootargs ${mem} "\
		"video=${videospec} " \
		"console=${console} " \
		"tegraboot=${tegraboot} " \
		"tegrapart=${tegraparts} " \
		"${smpflag} " \
		"${usb_old_scheme_first} " \
		"rw root=/dev/nfs ip=:::::${nfsport}:on; "\
		"if test ${ethonusbhost} = 1; then " \
			"setenv bootargs ${bootargs} " \
				"tegra_ehci_probe_delay=${usbprobedelay}; " \
		"fi;\0" \
	"usbboot=usb start; " \
		"ext2load usb 0:1 ${loadaddr} /boot/${bootfile}; " \
		"setenv bootargs root=/dev/sda1 rw rootwait " \
                "${mem} " \
                "video=${videospec} " \
		"console=${console} " \
		"usbcore.old_scheme_first=1 " \
		"tegraboot=${tegraboot} " \
                "${lp0_vec} " \
		"tegrap earlyprintk; "\
		"bootm ${loadaddr}\0" \
	"nandboot=setenv bootcmd  " \
		"run nandargs \\; " \
		"ymount /flash\\; " \
		"yrdm /flash/boot/${bootfile} ${loadaddr}\\; " \
		"yumount /flash\\; " \
		"bootm ${loadaddr}\0" \
	"mmcboot=mmc init ${mmcdev}; " \
		"ext2load mmc ${mmcdev}:3 ${loadaddr} /boot/${bootfile}; " \
		"setenv bootargs root=/dev/mmcblk${mmcdev}p3 rw rootwait " \
                "${mem} " \
                "video=${videospec} " \
		"console=${console} " \
		"usbcore.old_scheme_first=1 " \
		"tegraboot=${tegraboot} " \
                "${lp0_vec} " \
		"tegrap earlyprintk; "\
		"bootm ${loadaddr}\0" \
	"nfsboot=setenv bootcmd " \
		"run nfsargs\\; "\
		"tftpboot ${loadaddr}\\;"\
		"bootm ${loadaddr}\0"

#endif /* __CONFIG_H */
