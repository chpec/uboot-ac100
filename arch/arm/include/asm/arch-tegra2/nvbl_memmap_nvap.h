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
#ifndef INCLUDED_NVBL_MEMMAP_NVAP_H
#define INCLUDED_NVBL_MEMMAP_NVAP_H

/**
 * @defgroup nvbl_memmap_group NvBL Preprocessor Directives
 *
 *
 *
 * @ingroup nvbl_group
 * @{
 */

//------------------------------------------------------------------------------
//  RESTRICTION
//
//  This file is a configuration file. It should ONLY contain simple #define
//  directives defining constants. This file is included by other files that
//  only support simple substitutions.
//
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//     Chip-Independent Base Addresses
//------------------------------------------------------------------------------

#define NVAP_BASE_PA_SRAM       0x40000000              /**< Common Base physical address of internal SRAM. */
#define NVAP_BASE_PA_SRAM_SIZE  0x00020000              /**< Common Size of internal SRAM (128KB). */
#define NVAP_BASE_PA_BOOT_INFO  NVAP_BASE_PA_SRAM       /**< Common Base physical address of boot information table. */

//------------------------------------------------------------------------------
//     AP20-Specific Base Addresses
//------------------------------------------------------------------------------

#define AP20_BASE_PA_SDRAM      0x00000000              /**< AP20 Base physical address of SDRAM. */
#define AP20_BASE_PA_SRAM       NVAP_BASE_PA_SRAM       /**< AP20 Base physical address of internal SRAM. */
#define AP20_BASE_PA_SRAM_SIZE  0x00040000              /**< AP20 Size of internal SRAM (256KB). */
#define AP20_BASE_PA_NOR_FLASH  0xD0000000              /**< AP20 Base physical address of flash. */
#define AP20_BASE_PA_BOOT_INFO  AP20_BASE_PA_SRAM       /**< AP20 Base physical address of boot information table. */

//-------------------------------------------------------------------------------
// Super-temporary stacks for EXTREMELY early startup. The values chosen for
// these addresses must be valid on ALL SOCs because this value is used before
// we are able to differentiate between the SOC types.
//
// NOTE: The since CPU's stack will eventually be moved from IRAM to SDRAM, its
//       stack is placed below the AVP stack. Once the CPU stack has been moved,
//       the AVP is free to use the IRAM the CPU stack previously occupied if
//       it should need to do so.
//
// NOTE: In multi-processor CPU complex configurations, each processor will have
//       its own stack of size NVAP_EARLY_BOOT_IRAM_CPU_STACK_SIZE. CPU 0 will have
//       a limit of NVAP_LIMIT_PA_IRAM_CPU_EARLY_BOOT_STACK. Each successive CPU
//       will have a stack limit that is NVAP_EARLY_BOOT_IRAM_CPU_STACK_SIZE less
//       than the previous CPU.
//-------------------------------------------------------------------------------

#define NVAP_LIMIT_PA_IRAM_AVP_EARLY_BOOT_STACK (NVAP_BASE_PA_SRAM + NVAP_BASE_PA_SRAM_SIZE)    /**< Common AVP early boot stack limit */
#define NVAP_EARLY_BOOT_IRAM_AVP_STACK_SIZE     0x1000                                          /**< Common AVP early boot stack size */
#define NVAP_LIMIT_PA_IRAM_CPU_EARLY_BOOT_STACK (NVAP_LIMIT_PA_IRAM_AVP_EARLY_BOOT_STACK - NVAP_EARLY_BOOT_IRAM_AVP_STACK_SIZE) /**< Common CPU early boot stack limit */
#define NVAP_EARLY_BOOT_IRAM_CPU_STACK_SIZE     0x1000                                          /**< Common CPU early boot stack size */


/** @} */

#endif  // INCLUDED_NVBL_MEMMAP_NVAP_H

