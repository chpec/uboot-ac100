/*
 * Board specific setup info
 *
 * Copyright (c) 2009, Code Aurora Forum. All rights reserved.
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

#include <config.h>
#include <version.h>
#include <asm-armv7Scorpion/armv7Scorpion.h>

platform_save_lr:
     .word    0x0

.globl platformsetup
platformsetup:

    str       lr, platform_save_lr

    /*
     * Before turning on caches, call function to set up
     * cache timing parameters.
     */
    bl        set_cache_timing_parameters

    ldr       lr, platform_save_lr

    mov       pc, lr
