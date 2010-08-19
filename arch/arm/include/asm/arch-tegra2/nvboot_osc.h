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
#ifndef INCLUDED_NVBOOT_OSC_H
#define INCLUDED_NVBOOT_OSC_H

/**
 * Defines the oscillator frequencies supported by the hardware.
 */

#if defined(__cplusplus)
extern "C"
{
#endif

/**
 * Set of oscillator frequencies supported by the hardware.
 */
/*
 * Set of oscillator frequencies supproted in the internal API
 * + invalid (measured but not in any valid band)
 * + unknown (not measured at all)
 * The order of the enum MUST match the HW definition in OSC_CTRL for the first
 * four values.  Note that this is a violation of the SW convention on enums.
 */
typedef enum
{  
    /// Specifies an oscillator frequency of 13MHz.
    NvBootClocksOscFreq_13 = 0x0,

    /// Specifies an oscillator frequency of 19.2MHz.
    NvBootClocksOscFreq_19_2,

    /// Specifies an oscillator frequency of 12MHz.
    NvBootClocksOscFreq_12,

    /// Specifies an oscillator frequency of 26MHz.
    NvBootClocksOscFreq_26,

    NvBootClocksOscFreq_Num,       // dummy to get number of frequencies
    NvBootClocksOscFreq_Unknown,
    NvBootClocksOscFreq_Force32 = 0x7fffffff
} NvBootClocksOscFreq;

#if defined(__cplusplus)
}
#endif

#endif /* #ifndef INCLUDED_NVBOOT_OSC_H */
