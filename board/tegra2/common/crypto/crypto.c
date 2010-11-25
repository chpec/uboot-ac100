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

/*
 * crypto.c - Cryptography support
 */

#include <common.h>
#include <asm/arch/nvcommon.h>
#include <asm/arch/nvboot_error.h>
#include "crypto.h"
#include "nvaes_ref.h"

/* Local function declarations */
static void
apply_cbc_chain_data(NvU8 *cbc_chain_data, NvU8 *src, NvU8 *dst);

static NvBootError
determine_crypto_ops(security_mode security, NvBool *encrypt, NvBool *sign);

static void
generate_key_schedule(NvU8 *key, NvU8 *key_schedule, NvBool encrypt_data);

static void
encrypt_object( NvU8	*key_schedule,
		NvU8	*src,
		NvU8	*dst,
		NvU32	num_aes_blocks);

static NvBootError
encrypt_and_sign(NvU8		*key,
		 security_mode	security,
		 NvU8		*src,
		 NvU32		length,
		 NvU8		*sig_dst);

NvBool enable_debug_crypto = NV_FALSE;

/* Implementation */
static NvU8 zero_key[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

static void
print_vector(char *name, NvU32 num_bytes, NvU8 *data)
{
	NvU32 i;

	printf("%s [%d] @0x%08x", name, num_bytes, (NvU32)data);
	for (i=0; i<num_bytes; i++) {
		if ( i % 16 == 0 )
			printf(" = ");
		printf("%02x", data[i]);
		if ( (i+1) % 16 != 0 )
			printf(" ");
	}
	printf("\n");
}


static void
apply_cbc_chain_data(NvU8 *cbc_chain_data, NvU8 *src, NvU8 *dst)
{
	int i;

	for (i = 0; i < 16; i++) {
		*dst++ = *src++ ^ *cbc_chain_data++;
	}
}

static NvBootError
determine_crypto_ops(security_mode security, NvBool *encrypt, NvBool *sign)
{
	NvBootError Error = NvBootError_Success;

	switch (security) {
	case security_mode_Plaintext:
		*encrypt = NV_FALSE;
		*sign = NV_FALSE;
		break;

	case security_mode_Checksum:
		*encrypt = NV_FALSE;
		*sign = NV_TRUE;
		break;

	case security_mode_Encrypted:
		*encrypt = NV_TRUE;
		*sign = NV_TRUE;
		break;

	default:
		printf("determine_crypto_ops: security mode not set.\n");
		Error = NvBootError_InvalidParameter;
	}

	return Error;
}

static void
generate_key_schedule(NvU8 *key, NvU8 *key_schedule, NvBool encrypt_data)
{
	/* Expand the key to produce a key schedule. */
	if (encrypt_data == NV_TRUE) {
		/* Expand the provided key. */
		nv_aes_expand_key(key, key_schedule);
	} else {
		/*
		 * The only need for a key is for signing/checksum purposes, so
		 * expand a key of 0's.
		 */
		nv_aes_expand_key(zero_key, key_schedule);
	}
}

static void
encrypt_object(NvU8	*key_schedule,
		NvU8	*src,
		NvU8	*dst,
		NvU32	num_aes_blocks)
{
	NvU32 i;
	NvU8 *cbc_chain_data;
	NvU8  tmp_data[KEY_LENGTH];

	cbc_chain_data = zero_key; /* Convenient array of 0's for IV */

	for (i = 0; i < num_aes_blocks; i++) {
		if (enable_debug_crypto) {
			printf("encrypt_object: block %d of %d\n", i,
							num_aes_blocks);
			print_vector("AES Src", KEY_LENGTH, src);
		}

		/* Apply the chain data */
		apply_cbc_chain_data(cbc_chain_data, src, tmp_data);

		if (enable_debug_crypto) print_vector("AES Xor", KEY_LENGTH,
							tmp_data);

		/* encrypt the AES block */
		nv_aes_encrypt(tmp_data, key_schedule, dst);

		if (enable_debug_crypto)
			print_vector("AES Dst", KEY_LENGTH, dst);
		/* Update pointers for next loop. */
		cbc_chain_data = dst;
		src += KEY_LENGTH;
		dst += KEY_LENGTH;
	}
}

static void
left_shift_vector(NvU8	*In,
		  NvU8	*Out,
		  NvU32	Size)
{
	NvU32 i;
	NvU8 Carry = 0;

	for (i=0; i<Size; i++) {
		NvU32 j = Size-1-i;

		Out[j] = (In[j] << 1) | Carry;
		Carry = In[j] >> 7; /* get most significant bit */
	}
}

static void
sign_objext(
	NvU8	*key,
	NvU8	*key_schedule,
	NvU8	*src,
	NvU8	*dst,
	NvU32	num_aes_blocks)
{
	NvU32 i;
	NvU8 *cbc_chain_data;

	NvU8	L[KEY_LENGTH];
	NvU8	K1[KEY_LENGTH];
	NvU8	tmp_data[KEY_LENGTH];

	cbc_chain_data = zero_key; /* Convenient array of 0's for IV */

	/* compute K1 constant needed by AES-CMAC calculation */

	for (i=0; i<KEY_LENGTH; i++)
		tmp_data[i] = 0;

	encrypt_object(key_schedule, tmp_data, L, 1);

	if (enable_debug_crypto)
		print_vector("AES(key, nonce)", KEY_LENGTH, L);

	left_shift_vector(L, K1, sizeof(L));

	if (enable_debug_crypto)
		print_vector("L", KEY_LENGTH, L);

	if ( (L[0] >> 7) != 0 ) /* get MSB of L */
		K1[KEY_LENGTH-1] ^= AES_CMAC_CONST_RB;

	if (enable_debug_crypto)
		print_vector("K1", KEY_LENGTH, K1);

	/* compute the AES-CMAC value */
	for (i = 0; i < num_aes_blocks; i++) {
		/* Apply the chain data */
		apply_cbc_chain_data(cbc_chain_data, src, tmp_data);

		/* for the final block, XOR K1 into the IV */
		if ( i == num_aes_blocks-1 )
			apply_cbc_chain_data(tmp_data, K1, tmp_data);

		/* encrypt the AES block */
		nv_aes_encrypt(tmp_data, key_schedule, (NvU8*)dst);

		if (enable_debug_crypto) {
			printf("sign_objext: block %d of %d\n", i,
							num_aes_blocks);
			print_vector("AES-CMAC Src", KEY_LENGTH, src);
			print_vector("AES-CMAC Xor", KEY_LENGTH, tmp_data);
			print_vector("AES-CMAC Dst", KEY_LENGTH, (NvU8*)dst);
		}

		/* Update pointers for next loop. */
		cbc_chain_data = (NvU8*)dst;
		src += KEY_LENGTH;
	}

	if (enable_debug_crypto)
		print_vector("AES-CMAC Hash", KEY_LENGTH, (NvU8*)dst);
}

static NvBootError
encrypt_and_sign(NvU8		*key,
		security_mode	security,
		NvU8		*src,
		NvU32		length,
		NvU8		*sig_dst)
{
	NvBool	encrypt_data;
	NvBool	sign_data;
	NvU32	num_aes_blocks;
	NvU8	key_schedule[4*NVAES_STATECOLS*(NVAES_ROUNDS+1)];
	NvBootError Error;

	Error = determine_crypto_ops(security, &encrypt_data, &sign_data);
	if (Error != NvBootError_Success)
		return Error;

	if (enable_debug_crypto) {
		printf("encrypt_and_sign: Length = %d\n", length);
		print_vector("AES key", KEY_LENGTH, key);
	}

	generate_key_schedule(key, key_schedule, encrypt_data);

	num_aes_blocks = ICEIL(length, KEY_LENGTH);

	if (encrypt_data == NV_TRUE) {
		if (enable_debug_crypto)
			printf("encrypt_and_sign: begin encryption\n");

		/* Perform this in place, resulting in src being encrypted. */
		encrypt_object(key_schedule, src, src, num_aes_blocks);

		if (enable_debug_crypto)
			printf("encrypt_and_sign: end encryption\n");
	}

	if (sign_data == NV_TRUE) {
		if (enable_debug_crypto)
			printf("encrypt_and_sign: begin signing\n");

		/* encrypt the data, overwriting the result in signature. */
		sign_objext(key, key_schedule, src, sig_dst, num_aes_blocks);

		if (enable_debug_crypto)
			printf("encrypt_and_sign: end signing\n");
	}

	return Error;
}

NvBootError
encrypt_and_sign_block(NvU8	*key,
		security_mode	security,
		NvU8		*block_image,
		NvU32		block_len,
		NvU8		*signature)
{
	NvBootError e;

	e = encrypt_and_sign(key, security, block_image, block_len, signature);
	return e;
}

NvBootError
sign_data_block(NvU8 *source,
		NvU32 length,
		NvU8 *signature)
{
	return encrypt_and_sign_block(zero_key,
				security_mode_Checksum,
				source,
				length,
				signature);
}
