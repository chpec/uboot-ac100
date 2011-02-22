/*
 * Copyright (c) 2011 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef __configs_chromeos_developer_h__
#define __configs_chromeos_developer_h__

#include <configs/chromeos/common.h>

#define CONFIG_CMD_AUTOSCRIPT
#define CONFIG_CMD_BDI
#define CONFIG_CMD_BOOTD
#define CONFIG_CMD_CONSOLE
#define CONFIG_CMD_CROS
#define CONFIG_CMD_ECHO
#define CONFIG_CMD_FPGA
#define CONFIG_CMD_IMI
#define CONFIG_CMD_ITEST
#define CONFIG_CMD_LOADB
#define CONFIG_CMD_LOADS
#define CONFIG_CMD_READ
#define CONFIG_CMD_MISC
#define CONFIG_CMD_SETGETDCR
#define CONFIG_CMD_XIMG

#define CONFIG_MMC
#define CONFIG_EFI_PARTITION
#define CONFIG_DOS_PARTITION

#define CONFIG_CMD_READ
#define CONFIG_CMD_MMC
#define CONFIG_CMD_EXT2
#define CONFIG_CMD_FAT

#define CONFIG_CMD_MEMORY

/*
 * The following make using the console nicer.
 */
#define CONFIG_DISPLAY_CPUINFO          1

#define CONFIG_SYS_LONGHELP             1
#define CONFIG_CMDLINE_EDITING          1
#define CONFIG_COMMAND_HISTORY          1
#define CONFIG_AUTOCOMPLETE             1
#define CONFIG_SYS_HUSH_PARSER          1
#define	CONFIG_SYS_PROMPT_HUSH_PS2      "> "

/*
 * Recovery, normal and stub configs define CONFIG_ENV_IS_NOWHERE, but this
 * developer config requires a saved environment. The developer header file
 * in include/config/chromeos/<arch>/developer.h should define where the
 * environment is, for example CONFIG_ENV_IS_IN_SPI_FLASH for SPI flash.
 * If a saved environment is not available for a particular architecture or
 * board, you can define CONFIG_ENV_IS_NOWHERE in that header file instead.
 */
#define CONFIG_CMD_SAVEENV

/* Enable USB Ethernet dongles */
#define CONFIG_USB_HOST_ETHER
#define CONFIG_USB_ETHER_ASIX

/*
 * Ethernet support
 */
#define CONFIG_CMD_NET
#define CONFIG_NET_MULTI
#define CONFIG_CMD_PING
#define CONFIG_CMD_DHCP

/*
 * BOOTP options
 */
#define CONFIG_BOOTP_SUBNETMASK
#define CONFIG_BOOTP_GATEWAY
#define CONFIG_BOOTP_HOSTNAME
#define CONFIG_BOOTP_BOOTPATH

#define CONFIG_IPADDR		10.0.0.2
#define CONFIG_SERVERIP		10.0.0.1
#define CONFIG_BOOTFILE		uImage

/*
 * We decorate the nfsroot name so that multiple users / boards can easily
 * share an NFS server:
 *   user - username, e.g. 'frank'
 *   board - board, e.g. 'seaboard'
 *   serial - serial number, e.g. '1234'
 */
#define CONFIG_ROOTPATH		"/export/nfsroot-${user}-${board}-${serial#}"



#endif //__configs_chromeos_developer_h__
