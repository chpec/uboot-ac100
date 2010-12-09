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

#ifndef __TEGRA2_COMMON_H
#define __TEGRA2_COMMON_H
#include <asm/sizes.h>

#undef DEBUG
/*
 * High Level Configuration Options
 */
#define CONFIG_CMD_EXT2         1
#define CONFIG_ARMCORTEXA9	1	/* This is an ARM V7 CPU core */
#define CONFIG_TEGRA2		1	/* in a NVidia Tegra2 core */
#define CONFIG_MACH_TEGRA_GENERIC     1 /* which is a Tegra generic machine */
#define CONFIG_SKIP_RELOCATE_UBOOT
#define CONFIG_L2_OFF		1	/* No L2 cache */

#define CONFIG_ENABLE_CORTEXA9	1	/* enable cortex A9 core */
#ifndef CONFIG_ENABLE_CORTEXA9
  #define TEGRA2_AVP_ONLY
#endif
#undef TEGRA2_TRACE

/*
 * Configuration required for using ethernet over usb.
 */
/* disable USB-USB networking for now */
#if 0
#define CONFIG_USB_GADGET
#define CONFIG_USB_GADGET_TEGRA2
#define CONFIG_USB_GADGET_DUALSPEED
#define CONFIG_USB_DEVICE
#define CONFIG_USB_ETHER
#endif

/*
 * ethaddr settings can be overruled via environment settings:
 *  usbnet_devaddr and usbnet_hostaddr
 */
#define CONFIG_USBNET_DEV_ADDR          "e4:9b:e7:66:2f:7b"
#define CONFIG_USBNET_HOST_ADDR         "0a:fa:63:8b:e8:0a"
#define CONFIG_CMD_NET
#define CONFIG_NET_MULTI
#define CONFIG_CMD_PING
#define CONFIG_CMD_DHCP

#include <asm/arch/tegra2.h>            /* get chip and board defs */

/*
 * Display CPU and Board information
 */
#define CONFIG_DISPLAY_CPUINFO		1
#define CONFIG_DISPLAY_BOARDINFO	1

#undef CONFIG_USE_IRQ
#define CONFIG_ARCH_CPU_INIT
#define CONFIG_MISC_INIT_R
#define CONFIG_SKIP_LOWLEVEL_INIT

#define CONFIG_CMDLINE_TAG		1	/* enable passing of ATAGs */

/* Environment */
#define CONFIG_ENV_IS_NOWHERE
#define CONFIG_ENV_SIZE			SZ_128K	/* Total Size Environment */

/*
 * Size of malloc() pool
 */
#define CONFIG_SYS_MALLOC_LEN		(SZ_256K * 16)   /* 4MB  */
#define CONFIG_SYS_GBL_DATA_SIZE	128	/* bytes reserved for */
						/* initial data */

/*
 * Hardware drivers
 */

/*
 * PllX Configuration
 */
#define CONFIG_SYS_CPU_OSC_FREQUENCY    1000000        /* Set CPU clock to 1GHz */

/*
 * NS16550 Configuration
 */
#define V_NS16550_CLK			216000000	/* 216MHz (pllp_out0) */

#define CONFIG_SYS_NS16550
#define CONFIG_SYS_NS16550_SERIAL
#define CONFIG_SYS_NS16550_REG_SIZE	(-4)
#define CONFIG_SYS_NS16550_CLK		V_NS16550_CLK

/*
 * select serial console configuration
 */
#define CONFIG_CONS_INDEX	1

/* allow to overwrite serial and ethaddr */
#define CONFIG_ENV_OVERWRITE
#define CONFIG_BAUDRATE			115200
#define CONFIG_SYS_BAUDRATE_TABLE	{4800, 9600, 19200, 38400, 57600,\
					115200}

#define CONFIG_MMC			1
#define CONFIG_TEGRA2_MMC		1
#define TEGRA2_MMC_DEFAULT_DEVICE	"0"
#define CONFIG_DOS_PARTITION		1
#define CONFIG_EFI_PARTITION		1

/* commands to include */
#include <config_cmd_default.h>

/* turn on command-line edit/hist/auto */

#define CONFIG_CMDLINE_EDITING		1
#define CONFIG_COMMAND_HISTORY		1
#define CONFIG_AUTOCOMPLETE			1

#define TEGRAPARTS_DEFAULT		"system:3680:2bc0:800"
#define TEGRABOOT_DEFAULT 		"nand"

#undef CONFIG_YAFFS2			/* YAFFS2 Support		*/
#define CONFIG_YAFFS2_TAG_NO_ECC /* Disable YAFFS2 ECC calculation */
				 /* This is required in order to reduce nand
				  * tag space used by yaffs2 to fit the 20 bytes
				  * available with Reed-Solomon ecc enabled.
				  * Ecc is calculated and checked by nvddk
				  * stack. */
#undef CONFIG_YAFFS_DOES_ECC /* Ditto */
#define CONFIG_SYS_64BIT_STRTOUL		1
#define CONFIG_SYS_64BIT_VSPRINTF		1

/*#define CONFIG_MTD_PARTITIONS */
/*#define CONFIG_CMD_I2C		I2C serial bus support	*/
#define CONFIG_CMD_MMC		/* MMC support			*/
#define CONFIG_CMD_NAND		/* NAND support			*/
#define CONFIG_CMD_USB		/* USB Host support		*/

#undef CONFIG_CMD_FLASH		/* flinfo, erase, protect	*/
#undef CONFIG_CMD_FPGA		/* FPGA configuration Support	*/
#undef CONFIG_CMD_IMI		/* iminfo			*/
#undef CONFIG_CMD_IMLS		/* List all found images	*/
#undef CONFIG_CMD_NFS		/* NFS support			*/

#define CONFIG_SYS_NO_FLASH
#define CONFIG_SYS_I2C_SPEED		100000
#define CONFIG_SYS_I2C_SLAVE		1
#define CONFIG_SYS_I2C_BUS		0
#define CONFIG_SYS_I2C_BUS_SELECT	1
#undef CONFIG_DRIVER_TEGRA2_I2C

/*
 * Board NAND Info.
 */
#define CONFIG_NAND_TEGRA2
#define CONFIG_SYS_NAND_ADDR		NAND_BASE	/* physical address */
							/* to access nand */
#define CONFIG_SYS_NAND_BASE		NAND_BASE	/* physical address */
							/* to access nand at */
							/* CS0 */
#define CONFIG_SYS_MAX_NAND_DEVICE	1		/* Max number of NAND */
							/* devices */

/*
 * USB Host.
 */
#define CONFIG_USB_EHCI
#define CONFIG_USB_EHCI_TEGRA

#ifdef CONFIG_USB_EHCI_TEGRA
#   define CONFIG_USB_CONTROLLER_INSTANCES	 2
#   define USB_EHCI_TEGRA_BASE_ADDR_USB3         0xC5008000   /* USB3 base address */
#   define USB_EHCI_TEGRA_BASE_ADDR_USB1         0xC5000000   /* USB1 base address */
#endif

/*
 */
#define CONFIG_USB_EHCI_DATA_ALIGN		4

/*
 * This parameter affects a TXFILLTUNING field that controls how much data is
 * sent to the latency fifo before it is sent to the wire. Without this
 * parameter, the default (2) causes occasional Data Buffer Errors in OUT
 * packets depending on the buffer address and size.
 */
#define CONFIG_USB_EHCI_TXFIFO_THRESH	10

#define CONFIG_EHCI_IS_TDI
#define CONFIG_USB_STORAGE
#define CONFIG_USB_HOST_ETHER
#define CONFIG_USB_ETHER_ASIX
#define CONFIG_USB_ETHER_SMSC95XX

/* Enable Warmboot code and lp0_vec */
#define CONFIG_TEGRA2_LP0		1

#ifdef CONFIG_TEGRA2_LP0
#define TEGRA_LP0_DEFAULT_ADDR	0x1C406000  /* inside carveout memory region */
#define TEGRA_LP0_SIZE		0x2000	    /* 8KB */

/* make sure the following has the same value of TEGRA_LP0_DEFAULT_ADDR */
#define TEGRA_LP0_DEFAULT_ADDR_STR	"0x1C406000"
/* make sure the following has the same value of TEGRA_LP0_SIZE */
#define TEGRA_LP0_SIZE_STR		"0x2000"
#define AT				"@"
#define LP0_VEC				"lp0_vec="
#else
#define TEGRA_LP0_DEFAULT_ADDR_STR	" "
#define TEGRA_LP0_SIZE_STR		" "
#define AT				" "
#define LP0_VEC				" "
#endif

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
	"usbargs=setenv bootargs root=/dev/sda3 rw rootwait " \
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
		"ext2load usb 0:3 ${loadaddr} /boot/${bootfile}; " \
		"setenv bootargs root=/dev/sda3 rw rootwait " \
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

/* auto load */
/* try load from usb first, then mmc */
#undef CONFIG_BOOTCOMMAND
#define CONFIG_BOOTCOMMAND              "run usbboot ; run mmcboot"

#define CONFIG_AUTO_COMPLETE
/*
 * Miscellaneous configurable options
 */
#define CONFIG_SYS_LONGHELP		/* undef to save memory */
#define CONFIG_SYS_HUSH_PARSER		/* use "hush" command parser */
#define CONFIG_SYS_PROMPT_HUSH_PS2	"> "
#define CONFIG_SYS_PROMPT		V_PROMPT
/* Increasing the size of the IO buffer as default nfsargs size is more than 256
  and so it is not possible to edit it */
#define CONFIG_SYS_CBSIZE		(256 * 2) /* Console I/O Buffer Size */
/* Print Buffer Size */
#define CONFIG_SYS_PBSIZE		(CONFIG_SYS_CBSIZE + \
					sizeof(CONFIG_SYS_PROMPT) + 16)
#define CONFIG_SYS_MAXARGS		16	/* max number of command args */
/* Boot Argument Buffer Size */
#define CONFIG_SYS_BARGSIZE		(CONFIG_SYS_CBSIZE)

#define CONFIG_SYS_MEMTEST_START	(TEGRA2_SDRC_CS0 + 0x600000)
								/* mem test */
#define CONFIG_SYS_MEMTEST_END		(CONFIG_SYS_MEMTEST_START + \
					0x02000000) /* 32MB */

#define CONFIG_SYS_LOAD_ADDR		(0xA00800)	/* default */
							/* load address */

#define CONFIG_SYS_HZ			1000

/*-----------------------------------------------------------------------
 * Stack sizes
 *
 * The stack sizes are set up in start.S using the settings below
 */
#define CONFIG_STACKBASE	0x2800000	/* 40MB */
#define CONFIG_STACKSIZE	SZ_128K	/* regular stack */
#ifdef CONFIG_USE_IRQ
#define CONFIG_STACKSIZE_IRQ	SZ_4K	/* IRQ stack */
#define CONFIG_STACKSIZE_FIQ	SZ_4K	/* FIQ stack */
#endif

/*-----------------------------------------------------------------------
 * Physical Memory Map
 */
#define CONFIG_NR_DRAM_BANKS	1
#define PHYS_SDRAM_1		TEGRA2_SDRC_CS0
#define PHYS_SDRAM_1_SIZE	SZ_512M

/*-----------------------------------------------------------------------
 * FLASH and environment organization
 */

#define CONFIG_SYS_MAX_FLASH_SECT	520	/* max number of sectors on */
						/* one chip */
#define CONFIG_SYS_MAX_FLASH_BANKS	2	/* max number of flash banks */
#define CONFIG_SYS_MONITOR_LEN		SZ_256K	/* Reserve 2 sectors */

#define CONFIG_SYS_FLASH_BASE		NAND_BASE

/* Monitor at start of flash */
#define CONFIG_SYS_MONITOR_BASE		CONFIG_SYS_FLASH_BASE

/*-----------------------------------------------------------------------
 * CFI FLASH driver setup
 */
/* timeout values are in ticks */
#define CONFIG_SYS_FLASH_ERASE_TOUT	(100 * CONFIG_SYS_HZ)
#define CONFIG_SYS_FLASH_WRITE_TOUT	(100 * CONFIG_SYS_HZ)

/* Flash banks JFFS2 should use */
#define CONFIG_SYS_MAX_MTD_BANKS	(CONFIG_SYS_MAX_FLASH_BANKS + \
					CONFIG_SYS_MAX_NAND_DEVICE)
#define CONFIG_SYS_JFFS2_MEM_NAND
/* use flash_info[2] */
#define CONFIG_SYS_JFFS2_FIRST_BANK	CONFIG_SYS_MAX_FLASH_BANKS
#define CONFIG_SYS_JFFS2_NUM_BANKS	1

/*
 *  LCDC configuration
 */

#define CONFIG_LCD

#ifdef CONFIG_LCD
#define CONFIG_TEGRA2_LCD
#define LCD_BPP             LCD_COLOR16
#define LCD_FB_ADDR         0x1C022000   /* FB could be passed from bl */
#define CONFIG_SYS_WHITE_ON_BLACK       /*Console colors*/
#endif

/*
 *  I2C configuration
 */
#define CONFIG_TEGRA2_I2C

#ifdef CONFIG_TEGRA2_I2C
#define CONFIG_CMD_I2C
#define CONFIG_I2C_MULTI_BUS	1
#define CONFIG_SYS_MAX_I2C_BUS	4
#endif

#endif /* __CONFIG_H */
