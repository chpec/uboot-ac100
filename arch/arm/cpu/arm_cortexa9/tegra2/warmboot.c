/*
 * (C) Copyright 2010
 * NVIDIA Corporation <www.nvidia.com>
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
#include <asm/arch/nv_drf.h>
#include <asm/arch/nvboot_error.h>
#include <asm/arch/nvcommon.h>
#include <asm/arch/nverror.h>
#include <asm/arch/tegra2.h>
#include <asm/arch/nv_hardware_access.h>
#include <asm/arch/warmboot.h>

NvBootError sign_data_block(NvU8 *source,
			  NvU32 length,
			  NvU8 *hash);

/* -------------------------------------
 *	Configuration parameters
 * -------------------------------------
 */

/* -------------------------------------
 *	Definitions
 * -------------------------------------
 */

/* NOTE: If more than one of the following is enabled, only one of them will
 *	 actually be used. RANDOM takes precedence over PATTERN and ZERO, and
 *	 PATTERN takes precedence overy ZERO.
 */
#define RANDOM_AES_BLOCK_IS_RANDOM	1	/* to randomize the header */
#define RANDOM_AES_BLOCK_IS_PATTERN	1	/* to patternize the header */
#define RANDOM_AES_BLOCK_IS_ZERO	1	/* to clear the header */

/* Address at which AP20 WB0 code runs */
/* The address must not overlap the bootrom's IRAM usage */
#define AP20_WB0_RUN_ADDRESS		0x40020000
#define AP20_IRAM_BASE			0x40000000

/* -------------------------------------
 *	Global External Labels
 * -------------------------------------
 */

void wb_start(void);	/* Start of WB assembly code */
void wb_end(void);	/* End of WB assembly code */

#define NV_ADDRESS_MAP_FUSE_BASE		0x7000F800UL
#define FUSE_PRODUCTION_MODE_0			_MK_ADDR_CONST(0x100)
#define FUSE_FA_0				_MK_ADDR_CONST(0x148)
#define FUSE_SECURITY_MODE_0			_MK_ADDR_CONST(0x1a0)

/* -------------------------------------
 *	Local Functions
 * -------------------------------------
 */
static NvU32 get_major_version(void)
{
	NvU32 reg = 0;
	NvU32 major_id = 0;

	reg = NV_READ32( NV_ADDRESS_MAP_APB_MISC_BASE + APB_MISC_GP_HIDREV_0) ;
	major_id = NV_DRF_VAL(APB_MISC, GP_HIDREV, MAJORREV, reg);
	return major_id;
}

static NvBool is_production_mode_fuse_set(void)
{
	NvU32 reg;

	reg = NV_READ32(NV_ADDRESS_MAP_FUSE_BASE + FUSE_PRODUCTION_MODE_0);
	if (reg)
		return NV_TRUE;
	else
		return NV_FALSE;
}

static NvBool is_odm_production_mode_fuse_set(void)
{
	NvU32 reg;

	reg = NV_READ32(NV_ADDRESS_MAP_FUSE_BASE + FUSE_SECURITY_MODE_0);
	if (reg)
		return NV_TRUE;
	else
		return NV_FALSE;
}

static NvBool ap20_is_failure_analysis_mode(void)
{
	volatile NvU32 reg;

	reg = NV_READ32(NV_ADDRESS_MAP_FUSE_BASE + FUSE_FA_0);
	if (reg)
		return NV_TRUE;
	else
		return NV_FALSE;
}

static NvBool ap20_is_odm_production_mode(void)
{
	if (!ap20_is_failure_analysis_mode() &&
	    is_odm_production_mode_fuse_set())
		return NV_TRUE;
	else
		return NV_FALSE;
}

static NvBool ap20_is_production_mode(void)
{
	if (get_major_version() == 0)
		return NV_TRUE;

	if (!ap20_is_failure_analysis_mode() &&
	     is_production_mode_fuse_set() &&
	    !is_odm_production_mode_fuse_set())
		return NV_TRUE;
	else
		return NV_FALSE;
}

static NvBool fuse_get_ap20_hal(nv_fuse_hal *hal)
{
	NvU32 reg;

	reg = NV_READ32(NV_ADDRESS_MAP_APB_MISC_BASE + APB_MISC_GP_HIDREV_0);
	if (NV_DRF_VAL(APB_MISC_GP, HIDREV, CHIPID, reg) == 0x20) {
		hal->is_odm_production_mode = ap20_is_odm_production_mode;
		hal->is_nv_production_mode = ap20_is_production_mode;
		return NV_TRUE;
	}
	return NV_FALSE;
}

static nvbl_operating_mode nv_fuse_get_operation_mode(void)
{
	nv_fuse_hal hal;
	if (fuse_get_ap20_hal(&hal)) {
		if (hal.is_odm_production_mode()) {
			printf("!!! OdmProductionMode is not supported !!!\n");
			return nvbl_mode_undefined;
		} else {
			if (hal.is_nv_production_mode())
				return nvbl_mode_production;
			else
				return nvbl_mode_undefined;
		}
	}
	return nvbl_mode_undefined;
}

static NvU64 query_random_seed(void)
{
	return 0;
}

static void determine_crypto_options(NvBool* is_encrypted, NvBool* is_signed, 
					NvBool* use_zero_key)
{
	switch (nv_fuse_get_operation_mode()) {
	case nvbl_mode_production:
		*is_encrypted = NV_FALSE;
		*is_signed = NV_TRUE;
		*use_zero_key = NV_TRUE;
		break;

	case nvbl_mode_undefined:
	default:
		*is_encrypted = NV_FALSE;
		*is_signed = NV_FALSE;
		*use_zero_key  = NV_FALSE;
		break;
	}
}

static NvBootError sign_wb_code(NvU32 start, NvU32 length, NvBool use_zero_key)
{
	NvBootError e;
	NvU8 *source;		/* Pointer to source */
	NvU8 *hash;

	/* Calculate AES block parameters. */
	source = (NvU8 *)(start + offsetof(nvboot_wb_header, random_aes_block));
	length -= offsetof(nvboot_wb_header, random_aes_block);
	hash = (NvU8 *)(start + offsetof(nvboot_wb_header, hash));
	e = sign_data_block(source, length, hash);
	return e;
}

/* -------------------------------------
 *	Global Functions
 * -------------------------------------
 */

NvBootError prepare_wb_code(NvU32 seg_address, NvU32 seg_length)
{
	NvBootError e = NvBootError_NotInitialized;		/* Error code */

	NvU32			start;		/* start of the actual code */
	NvU32			end;		/* end of the actual code */
	NvU32			actual_length;	/* length of the actual code */
	NvU32			length;		/* length of the 
						   signed/encrypted code */
	nvboot_wb_header	*src_header;	/* Pointer to src WB header */
	nvboot_wb_header	*dst_header;	/* Pointer to dest WB header */
	NvBool			is_encrypted;	/* Segment is encrypted */
	NvBool			is_signed;	/* Segment is signed */
	NvBool			use_zero_key;	/* Use key of all zeros */

	/* Determine crypto options. */
	determine_crypto_options(&is_encrypted, &is_signed, &use_zero_key);

	/* Get the actual code limits. */
	start  = (NvU32)wb_start;
	end = (NvU32)wb_end;
	actual_length = end - start;
	length = ((actual_length + 15) >> 4) << 4;

	/* The region specified by seg_address must be in SDRAM and must be
	 * nonzero in length.
	 */
	if ((seg_length  == 0) ||(seg_address == 0) ||
	    (seg_address >= AP20_IRAM_BASE)) {
		goto fail;
	}

	/* Things must be 16-byte aligned. */
	if ((seg_length & 0xF) || (seg_address & 0xF)) {
		goto fail;
	}

	/* Will the code fit? */
	if (seg_length < length) {
		goto fail;
	}

	/* Get a pointers to the source and destination region header. */
	src_header = (nvboot_wb_header*)start;
	dst_header = (nvboot_wb_header*)seg_address;

	/* Populate the random_aes_block as requested. */
#if RANDOM_AES_BLOCK_IS_RANDOM
	{
		NvU64	*pAesBlock = (NvU64*)&(src_header->random_aes_block);
		NvU64	*pEnd = (NvU64*)(((NvU32)pAesBlock) +
					sizeof(src_header->random_aes_block));
		do {
			*pAesBlock++ = query_random_seed();
		} while (pAesBlock < pEnd);
	}
#elif RANDOM_AES_BLOCK_IS_PATTERN
	{
		NvU32	*pAesBlock = (NvU32*)&(src_header->random_aes_block);
		NvU32	*pEnd = (NvU32*)(((NvU32)pAesBlock) +
					sizeof(src_header->random_aes_block));

		do {
			*pAesBlock++ = RANDOM_AES_BLOCK_IS_PATTERN;
		} while (pAesBlock < pEnd);
	}
#elif RANDOM_AES_BLOCK_IS_ZERO
	{
		NvU32	*pAesBlock = (NvU32*)&(src_header->random_aes_block);
		NvU32	*pEnd = (NvU32*)(((NvU32)pAesBlock) +
					sizeof(src_header->random_aes_block));

		do {
			*pAesBlock++ = 0;
		} while (pRandomAesBlock < pEnd);
	}
#endif

	/* Populate the header. */
	src_header->length_in_secure = length;
	src_header->length_secure = length;
	src_header->destination = AP20_WB0_RUN_ADDRESS;
	src_header->entry_point = AP20_WB0_RUN_ADDRESS;
	src_header->code_length = length - sizeof(nvboot_wb_header);

	if (is_encrypted) {
		/* Encryption is not supported */
		printf("!!!! No Encrypion implemented!!!!\n");
		dst_header->length_in_secure = 0;
		e = NvBootError_Unimplemented;
		goto fail;
	} else {
		/* No, just copy the code directly. */
		memcpy(dst_header, src_header, length);
	}

	/* Clear the signature in the destination code segment. */
	memset(&(dst_header->hash), 0, sizeof(dst_header->hash));

	if (is_signed) {
		NV_CHECK_ERROR_CLEANUP(sign_wb_code(seg_address, length,
					use_zero_key));
	}

fail:
	if (e != NvBootError_Success)
		printf("WB code not copied to LP0 location! (error=0x%x)\n", e);

	return e;
}
