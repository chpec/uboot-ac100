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

/**
 * Defines the warm boot 0 information for the boot rom.
 */

#ifndef INCLUDED_WARM_BOOT_H
#define INCLUDED_WARM_BOOT_H

#include <asm/arch/nvcommon.h>
#include <asm/arch/nvboot_error.h>

#if defined(__cplusplus)
extern "C"
{
#endif

/**
 * Defines the supported operating modes.
 */
typedef enum
{
	nvbl_mode_production = 3,

	/* Undefined. */
	nvbl_mode_undefined,

	/* Ignore -- Forces compilers to make 32-bit enums. */
	nvbl_mode_force32 = 0x7FFFFFFF
} nvbl_operating_mode;

/**
 * Defines the CMAC-AES-128 hash length in 32 bit words. (128 bits = 4 words)
 */
enum {HASH_LENGTH = 4};

/**
 * Defines the storage for a hash value (128 bits).
 */
typedef struct nvboot_hash_rec
{
	NvU32 hash[HASH_LENGTH];
} nvboot_hash;

/**
 * Defines the code header information for the boot rom.
 *
 * The code immediately follows the code header.
 *
 * Note that the code header needs to be 16 bytes aligned to preserve
 * the alignment of relevant data for hash and decryption computations without
 * requiring extra copies to temporary memory areas.
 */
typedef struct nvboot_wb_header_rec
{
	/* Specifies the length of the code header */
	NvU32		length_in_secure;

	/* Specifies the reserved words to maintain alignment */
	NvU32		reserved[3];

	/* Specifies the hash computed over the header and code,
	 * starting at random_aes_block.
	 */
	nvboot_hash	hash;

	/*  Specifies the random block of data which is not validated but
	 *  aids security.
	 */
	nvboot_hash	random_aes_block;

	/*  Specifies the length of the code header */
	NvU32		length_secure;

	/*  Specifies the starting address of the code in the
	 *  destination area.
	 */
	NvU32		destination;

	/*  Specifies the entry point of the code in the destination area. */
	NvU32		entry_point;

	/* Specifies the length of the code */
	NvU32		code_length;
} nvboot_wb_header;

typedef struct nv_fuse_hal_rec
{
	NvBool (*is_odm_production_mode)(void);
	NvBool (*is_nv_production_mode)(void);
} nv_fuse_hal;

NvBootError prepare_wb_code(NvU32 seg_address, NvU32 seg_length);

#if defined(__cplusplus)
}
#endif

#endif /* #ifndef INCLUDED_WARM_BOOT_H */
