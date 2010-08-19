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
#ifndef INCLUDED_NVBOOT_CLOCKS_H
#define INCLUDED_NVBOOT_CLOCKS_H

#if defined(__cplusplus)
extern "C"
{
#endif

/*
 * Set of valid count ranges per frequency.
 * Measuring 13 gives exactly 406 e.g.
 * The chosen range parameter is:
 * - more than the expected combined frequency deviation
 * - less than half the  relative distance between 12 and 13
 * - expressed as a ratio against a power of two to avoid floating point
 * - so that intermediate overflow is not possible
 *
 * The chosen factor is 1/64 or slightly less than 1.6% = 2^-6
 * Rounding is performed in such way as to guarantee at least the range
 * that is down for min and up for max
 * the range macros receive the frequency in kHz as argument
 * division by 32 kHz then becomes a shift by 5 to the right 
 *
 * The macros are defined for a frequency of 32768 Hz (not 32000 Hz). 
 * They use 2^-5 ranges, or about 3.2% and dispense with the rounding.
 * Also need to use the full value in Hz in the macro
 */

#define NVBOOT_CLOCKS_MIN_RANGE(F) (( F - (F>>5) - (1<<15) + 1 ) >> 15)
#define NVBOOT_CLOCKS_MAX_RANGE(F) (( F + (F>>5) + (1<<15) - 1 ) >> 15)

// For an easier ECO (keeping same number of instructions), we need a
// special case for 12 min range
#define NVBOOT_CLOCKS_MIN_CNT_12 (NVBOOT_CLOCKS_MIN_RANGE(12000000) -1)
#define NVBOOT_CLOCKS_MAX_CNT_12 NVBOOT_CLOCKS_MAX_RANGE(12000000)

#define NVBOOT_CLOCKS_MIN_CNT_13 NVBOOT_CLOCKS_MIN_RANGE(13000000)
#define NVBOOT_CLOCKS_MAX_CNT_13 NVBOOT_CLOCKS_MAX_RANGE(13000000)

#define NVBOOT_CLOCKS_MIN_CNT_19_2 NVBOOT_CLOCKS_MIN_RANGE(19200000)
#define NVBOOT_CLOCKS_MAX_CNT_19_2 NVBOOT_CLOCKS_MAX_RANGE(19200000)

#define NVBOOT_CLOCKS_MIN_CNT_26 NVBOOT_CLOCKS_MIN_RANGE(26000000)
#define NVBOOT_CLOCKS_MAX_CNT_26 NVBOOT_CLOCKS_MAX_RANGE(26000000)

// The stabilization delay in usec 
#define NVBOOT_CLOCKS_PLL_STABILIZATION_DELAY (300) 

// other values important when starting PLLP
#define NVBOOT_CLOCKS_PLLP_CPCON_DEFAULT (8)
#define NVBOOT_CLOCKS_PLLP_CPCON_19_2 (1)

#if defined(__cplusplus)
}
#endif

#endif /* #ifndef INCLUDED_NVBOOT_CLOCKS_H */
