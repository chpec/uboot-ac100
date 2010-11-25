/*
 * Copyright (c) 2007 NVIDIA Corporation.  All rights reserved.
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

#ifndef INCLUDED_NVAES_REF_H
#define INCLUDED_NVAES_REF_H

#include "asm/arch/nvcommon.h"

#if defined(__cplusplus)
extern "C"
{
#endif

#define NVAES_STATECOLS	4	// number of columns in the state & expanded key
#define NVAES_KEYCOLS	4	// number of columns in a key
#define NVAES_ROUNDS	10	// number of rounds in encryption

void nv_aes_expand_key(NvU8 *key, NvU8 *expkey);
void nv_aes_encrypt(NvU8 *in,  NvU8 *expkey, NvU8 *out);
void nv_aes_decrypt(NvU8 *in,  NvU8 *expkey, NvU8 *out);

#if defined(__cplusplus)
}
#endif

#endif // INCLUDED_NVAES_REF_H
