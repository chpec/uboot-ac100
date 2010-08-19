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

#ifndef INCLUDED_CPU_H
#define INCLUDED_CPU_H

#include <asm/arch/tegra2.h>

#define TIMER_USEC      ( 0x60005010 )
#define TIMER_CFG       ( 0x60005014 )
#define TIMER_0_BASE    ( 0x60005000 )
#define TIMER_0         ( TIMER_0_BASE + TIMER_TMR_PTV_0 )
#define TIMER_0_CLEAR   ( TIMER_0_BASE + TIMER_TMR_PCR_0 )
#define TIMER_1_BASE    ( 0x60005008 )
#define TIMER_1         ( TIMER_1_BASE + TIMER_TMR_PTV_0 )
#define TIMER_1_CLEAR   ( TIMER_1_BASE + TIMER_TMR_PCR_0 )

/* exception vectors */
#define VECTOR_BASE             ( 0x1000 )
#define VECTOR_RESET            ( VECTOR_BASE + 0 )
#define VECTOR_UNDEF            ( VECTOR_BASE + 4 )
#define VECTOR_SWI              ( VECTOR_BASE + 8 )
#define VECTOR_PREFETCH_ABORT   ( VECTOR_BASE + 12 )
#define VECTOR_DATA_ABORT       ( VECTOR_BASE + 16 )
#define VECTOR_IRQ              ( VECTOR_BASE + 24 )
#define VECTOR_FIQ              ( VECTOR_BASE + 28 )

#define MODE_DISABLE_INTR 0xc0
#define MODE_USR 0x10
#define MODE_FIQ 0x11
#define MODE_IRQ 0x12
#define MODE_SVC 0x13
#define MODE_ABT 0x17
#define MODE_UND 0x1B
#define MODE_SYS 0x1F

#define CP15_I              (1<<12) // instruction cache
#define CP15_Z              (1<<11) // branch prediction
#define CP15_C              (1<<2)  // data cache
#define CP15_M              (1<<0)  // MMU
#define CP15_V              (1<<13) // high vectors

#define APXX_EXT_MEM_START      0x00000000
#define APXX_EXT_MEM_END        0x40000000

/* For virtual map, divide the full aperture SDRAM aperture into half and use
 * the lower half for the cached mapping and upper half for the uncahced
 * mapping of the lower address region.
 */
#define APXX_EXT_MEM_MAX_SIZE_SUPPORTED \
    ((APXX_EXT_MEM_END - APXX_EXT_MEM_START) >> 1)

#define APXX_CACHED_TO_UNCACHED_OFFSET     APXX_EXT_MEM_MAX_SIZE_SUPPORTED

#define APXX_EXT_MEM_IS_UNCACHED(x)     ((((x) >= (APXX_EXT_MEM_START + \
                    APXX_EXT_MEM_MAX_SIZE_SUPPORTED))) ? NV_TRUE : NV_FALSE)

#endif
