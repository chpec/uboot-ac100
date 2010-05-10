/*
 * Copyright (c) 2010 Samsung Electronics Co., Ltd.
 * Jiun Yu <jiun.yu@samsung.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <common.h>
#include <movi.h>
#include <asm/io.h>
#include <asm/arch/regs.h>
#include <mmc.h>

extern raw_area_t raw_area_control;

typedef u32(*copy_sd_mmc_to_mem)
(u32 channel, u32 start_block, u16 block_size, u32 *trg, u32 init);

void movi_bl2_copy(void)
{
	ulong ch;
#if defined(CONFIG_EVT1)
	ch = *(volatile u32 *)(0xD0037488);
	copy_sd_mmc_to_mem copy_bl2 =
	    (copy_sd_mmc_to_mem) (*(u32 *) (0xD0037F98));
#else
	ch = *(volatile u32 *)(0xD003A508);
	copy_sd_mmc_to_mem copy_bl2 =
	    (copy_sd_mmc_to_mem) (*(u32 *) (0xD003E008));
#endif
	u32 ret;
	if (ch == 0xEB000000)
		ret =
		    copy_bl2(0, MOVI_BL2_POS, MOVI_BL2_BLKCNT,
			     CFG_PHY_UBOOT_BASE, 0);
	else if (ch == 0xEB200000)
		ret =
		    copy_bl2(2, MOVI_BL2_POS, MOVI_BL2_BLKCNT,
			     CFG_PHY_UBOOT_BASE, 0);
	else
		printf("u-boot image must be exist at SD/MMC channel 0 or 2\n");

	if (ret == 0)
		while (1)
			;
	else
		return;
}

/*
 * Copy zImage from SD/MMC to mem
 */
#ifdef CONFIG_MCP_SINGLE
void movi_zImage_copy(void)
{
	copy_sd_mmc_to_mem copy_zImage =
	    (copy_sd_mmc_to_mem) (*(u32 *) COPY_SDMMC_TO_MEM);
	u32 ret;

	/*
	 * 0x3C6FCE is total size of 2GB SD/MMC card
	 * TODO : eMMC will be used as boot device on HP proto2 board
	 *        So, total size of eMMC will be re-defined next board.
	 */
	ret =
	    copy_zImage(0, 0x3C6FCE, MOVI_ZIMAGE_BLKCNT, CFG_PHY_KERNEL_BASE,
			1);

	if (ret == 0)
		while (1)
			;
	else
		return;
}
#endif

void print_movi_bl2_info(void)
{
	printf("%d, %d, %d\n", MOVI_BL2_POS, MOVI_BL2_BLKCNT, MOVI_ENV_BLKCNT);
}

void movi_write_env(ulong addr)
{
	movi_write(raw_area_control.image[1].start_blk,
		   raw_area_control.image[1].used_blk, addr);
}

void movi_read_env(ulong addr)
{
	movi_read(raw_area_control.image[1].start_blk,
		  raw_area_control.image[1].used_blk, addr);
}

void movi_write_bl1(ulong addr)
{
	int i;
	ulong checksum;
	ulong src;
	ulong tmp;

	src = addr;
#if defined(CONFIG_EVT1)
	addr += 16;
	for (i = 16, checksum = 0; i < SS_SIZE; i++) {
		checksum += *(u8 *) addr++;
	}
	printf("checksum : 0x%x\n", checksum);
	*(volatile u32 *)(src + 0x8) = checksum;
	movi_write(raw_area_control.image[0].start_blk,
		   raw_area_control.image[0].used_blk, src);
#else
	for (i = 0, checksum = 0; i < SS_SIZE - 4; i++) {
		checksum += *(u8 *) addr++;
	}

	tmp = *(ulong *) addr;
	*(ulong *) addr = checksum;

	movi_write(raw_area_control.image[0].start_blk,
		   raw_area_control.image[0].used_blk, src);

	*(ulong *) addr = tmp;
#endif
}

#if defined(CONFIG_VOGUES)
int movi_boot_src()
{
	ulong reg;
	ulong src;

	reg = (*(volatile u32 *)(0xE0000004)) | 0xffffffc1;

	if (reg == 0x0c)
		/* boot device is SDMMC */
		src = 0;
	else if (reg == 0x14)
		/* boot device is NOR */
		src = 1;

	return src;
}
#endif
