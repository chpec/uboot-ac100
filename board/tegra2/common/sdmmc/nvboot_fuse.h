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

/**
 * @file nvboot_fuse.h
 *
 * Defines the parameters and data structures related to fuses.
 */

#ifndef INCLUDED_NVBOOT_FUSE_H
#define INCLUDED_NVBOOT_FUSE_H

#if defined(__cplusplus)
extern "C"
{
#endif

#define NVBOOT_DEVICE_KEY_BYTES (4)


/*
 * NvBootFuseBootDevice -- Peripheral device where Boot Loader is stored
 *
 * AP15 A02 split the original NAND encoding in NAND x8 and NAND x16.
 * For backwards compatibility the original enum was maintained and 
 * is implicitly x8 and aliased with the explicit x8 enum.
 *
 * This enum *MUST* match the equivalent list in nvboot_devmgr.h for
 * all valid values and None, Undefined not present in nvboot_devmgr.h
 */
typedef enum
{
    NvBootFuseBootDevice_Sdmmc = 0,
    NvBootFuseBootDevice_SnorFlash,
    NvBootFuseBootDevice_SpiFlash,
    NvBootFuseBootDevice_NandFlash,
    NvBootFuseBootDevice_NandFlash_x8  = NvBootFuseBootDevice_NandFlash,
    NvBootFuseBootDevice_NandFlash_x16 = NvBootFuseBootDevice_NandFlash,
    NvBootFuseBootDevice_MobileLbaNand,
    NvBootFuseBootDevice_MuxOneNand,
    NvBootFuseBootDevice_Max, /* Must appear after the last legal item */
    NvBootFuseBootDevice_Force32 = 0x7fffffff
} NvBootFuseBootDevice;
  
#if defined(__cplusplus)
}
#endif

#endif // INCLUDED_NVBOOT_FUSE_H
