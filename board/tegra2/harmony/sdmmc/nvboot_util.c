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
 * nvboot_util.c - Utility functions implementation, that will be used in bootrom
 */
#include <common.h>
#include <asm/io.h>
#include <asm/arch/nv_drf.h>
#include <asm/arch/nvcommon.h>
#include <asm/arch/tegra2.h>
#include <asm/arch/nv_hardware_access.h>

#include "nvboot_util.h"

/** NvBootUtilMemset - set a region of memory to a value.
 *
 *  @param s Pointer to the memory region
 *  @param c The value to be set in the memory
 *  @param size The length of the region
 */
void
NvBootUtilMemset( void *s, NvU32 c, size_t size )
{
    memset( s, c, size );
}

/** NvBootUtilMemcpy - copy memory.from source location to the destination location
 *
 *  @param dest pointer to the destination for copy
 *  @param src pointer to the source memory
 *  @param size The length of the copy
 */
void * 
NvBootUtilMemcpy( void *dest, const void *src, size_t size )
{
    return memcpy( dest, src, size );
}

NvU32 NvBootUtilGetTimeUS( void )
{
    // Should we take care of roll over of us counter? roll over happens after 71.58 minutes.
    NvU32 usec;
    usec = *(volatile NvU32 *)(NV_ADDRESS_MAP_TMRUS_BASE);
    return usec;
}

NvU32 NvBootUtilElapsedTimeUS(NvU32 StartTime) {
    // doing a diff and ignoring the overflow gets you the correct value 
    // even at wrararound, e.g. 
    // StartTime   = 0xFFFFFFFF
    // CurrentTime = 0x00000000
    // Current - Start = 1 + overflow flag (ignored in C)
    // this would *NOT* work if the counter was not 32 bits
    return  NvBootUtilGetTimeUS() - StartTime ;
}

void NvBootUtilWaitUS( NvU32 usec )
{
    NvU32 t0;
    NvU32 t1;

    t0 = NvBootUtilGetTimeUS();
    t1 = t0;

    // Use the difference for the comparison to be wraparound safe
    while( (t1 - t0) < usec )
    {
        t1 = NvBootUtilGetTimeUS();
    }
}

NvU32 NvBootUtilGetLog2Number(NvU32 size)
{
    NvU32 ReturnValue  = 0;

    while (1)
    {
        if ( (size == 1) || (size == 0) )
            break;
        size = size >> 1;
        ReturnValue++;
    }

    return ReturnValue;
}

NvU32
NvBootUtilSwapBytesInNvU32(const NvU32 Value)
{
    NvU32 tmp = (Value << 16) | (Value >> 16); /* Swap halves */
    /* Swap bytes pairwise */
    tmp = ((tmp >> 8) & 0x00ff00ff) | ((tmp & 0x00ff00ff) << 8);
    return (tmp);
}

NvBool
NvBootUtilIsValidPadding(NvU8 *Padding, NvU32 Length)
{
    NvU8 RefVal = 0x80;

    while (Length > 0)
    {
        if (*Padding != RefVal) return NV_FALSE;
        Padding++;
        Length--;
        RefVal = 0x00;
    }

    return NV_TRUE;
}
