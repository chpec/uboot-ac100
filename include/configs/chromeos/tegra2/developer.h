/*
 * Copyright (c) 2011 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef __configs_chromeos_tegra2_developer_h__
#define __configs_chromeos_tegra2_developer_h__

#include <configs/chromeos/developer.h>
#include <configs/chromeos/tegra2/common.h>

#include <configs/chromeos/tegra2/parts/usb.h>

/*
 * We expect all Tegra2 machines to have SPI flash as defined by the
 * firmware_layout.h config file. The environment will go there.
 */
#define CONFIG_ENV_IS_IN_SPI_FLASH 1

#define CONFIG_ENV_SECT_SIZE    CONFIG_LENGTH_ENV
#define CONFIG_ENV_OFFSET       CONFIG_OFFSET_ENV

#define CONFIG_SYS_MEMTEST_START       0x0000
#define CONFIG_SYS_MEMTEST_END         0x1000

/*
 * Developer-friendly booting options:
 *
 * dhcp_boot: get IP address from DHCP, then boot with NFS root.
 * keynfs_boot: get kernel from USB stick ext2 partition 1, then NFS root.
 * usb_boot: boot from a USB image made with image_to_usb.sh.
 * mmc_boot: boot from an image installed in eMMC,
 *
 * regen_all sets up the bootargs, so must be run after any change to
 * environment variables.
 *
 * The network adapter must be available before starting an dhcp_boot. You
 * may need to use 'usb start' first if you are using a USB ethernet adapter.
 * The same applies for usb_boot.
 */

#define CONFIG_EXTRA_ENV_SETTINGS_ARCH \
	"console=ttyS0,115200n8\0"\
	"cros_bootfile=/boot/vmlinux.uimg\0"\
	"user=user\0"\
	"board=arm\0"\
	"serial#=1\0"\
	"tftppath=/tftpboot/uImage\0"\
	"tftpserverip=10.0.0.1\0"\
	"nfsserverip=10.0.0.1\0"\
	"mmcdev=0\0"\
	"regen_all="\
		"setenv common_bootargs console=${console} "\
		"${platform_extras} noinitrd; "\
		"setenv bootargs ${common_bootargs} ${extra_bootargs} "\
		"${bootdev_bootargs}\0"\
	"regen_net_bootargs=setenv bootdev_bootargs "\
			"dev=/dev/nfs4 rw nfsroot=${nfsserverip}:${rootpath} "\
			"ip=dhcp; "\
		"run regen_all\0"\
	\
	"dhcp_setup=setenv tftppath "\
			"/tftpboot/uImage-${user}-${board}-${serial#}; "\
		"setenv rootpath " CONFIG_ROOTPATH "; "\
		"setenv autoload n; "\
		"run regen_net_bootargs\0"\
	"dhcp_boot=run dhcp_setup; "\
		"bootp; tftpboot ${loadaddr} ${tftpserverip}:${tftppath}; "\
		"bootm ${loadaddr}\0"\
	\
	"keynfs_setup="\
		"setenv rootpath " CONFIG_ROOTPATH "; "\
		"run regen_net_bootargs\0" \
	"keynfs_boot=run keynfs_setup; "\
		"ext2load usb 0:1 ${loadaddr} uImage; "\
		"bootm ${loadaddr}\0" \
	\
	"usb_setup=setenv bootdev_bootargs "\
		"root=/dev/sda3; "\
		"run regen_all\0"\
	"usb_boot=run usb_setup; "\
		"ext2load usb 0:3 ${loadaddr} ${cros_bootfile}; "\
		"bootm ${loadaddr}\0" \
	\
	"mmc_setup=mmc init ${mmcdev}; setenv bootdev_bootargs "\
		"root=/dev/mmcblk0p5; "\
		"run regen_all\0"\
	"mmc_boot=run mmc_setup; "\
		"ext2load mmc ${mmcdev}:3 ${loadaddr} ${cros_bootfile}; "\
		"bootm ${loadaddr}\0"

/*
 * Our developer-friendly boot process tries a number of things in sequence
 * until it finds a kernel. Network booting is tried first if we have Ethernet.
 */
#define CONFIG_BOOTCOMMAND \
  "usb start; "\
  "if test ${ethact} != \"\"; then "\
    "run dhcp_boot ; " \
  "fi ; " \
  "run keynfs_boot ; " \
  "run usb_boot ; " \
  "run mmc_boot"

#endif //__configs_chromeos_tegra2_developer_h__
