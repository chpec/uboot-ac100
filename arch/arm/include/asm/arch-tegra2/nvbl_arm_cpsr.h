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
#ifndef INCLUDED_NVBL_ARM_CPSR_H
#define INCLUDED_NVBL_ARM_CPSR_H

/**
 * @defgroup nvbl_arm_cpsr_group NvBL ARM CPSR API
 *
 * 
 *
 * @ingroup nvbl_group
 * @{
 */

//==========================================================================
/**  @name ARM CPSR/SPSR Definitions
 */
/*@{*/

#define PSR_MODE_MASK   0x1F
#define PSR_MODE_USR    0x10
#define PSR_MODE_FIQ    0x11
#define PSR_MODE_IRQ    0x12
#define PSR_MODE_SVC    0x13
#define PSR_MODE_ABT    0x17
#define PSR_MODE_UND    0x1B
#define PSR_MODE_SYS    0x1F    /**< Only available on ARM Arch v4 and higher. */
#define PSR_MODE_MON    0x16    /**< only available on ARM Arch v6 and higher with TrustZone extension. */

#define PSR_F_BIT       0x40    /**< FIQ disable. */
#define PSR_I_BIT       0x80    /**< IRQ disable. */
#define PSR_A_BIT       0x100   /**< Imprecise data abort disable (only available on ARM Arch v6 and higher). */
/*@}*/

/** @} */

#endif // INCLUDED_NVBL_ARM_CPSR_H
