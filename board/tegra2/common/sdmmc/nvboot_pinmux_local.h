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

#ifndef NVRM_PINMUX_UTILS_H
#define NVRM_PINMUX_UTILS_H

/*
 * nvrm_pinmux_utils.h defines the pinmux macros to implement for the resource
 * manager.
 */

#include <asm/arch/nvcommon.h>
#include <asm/arch/nv_drf.h>
#include "nvboot_util.h"

/*  The pin mux code supports run-time trace debugging of all updates to the
 *  pin mux & tristate registers by embedding strings (cast to NvU32s) into the
 *  control tables.
 */
#define NVRM_PINMUX_DEBUG_FLAG 0
#define NVRM_PINMUX_SET_OPCODE_SIZE_RANGE 3:1


#if NVRM_PINMUX_DEBUG_FLAG
NV_CT_ASSERT(sizeof(NvU32)==sizeof(const char*));
#endif

//  The extra strings bloat the size of Set/Unset opcodes
#define NVRM_PINMUX_SET_OPCODE_SIZE ((NVRM_PINMUX_DEBUG_FLAG)?NVRM_PINMUX_SET_OPCODE_SIZE_RANGE)

#ifdef __cplusplus
extern "C"
{
#endif  /* __cplusplus */

typedef enum {
    PinMuxConfig_OpcodeExtend = 0,
    PinMuxConfig_Set = 1,
    PinMuxConfig_Unset = 2,
    PinMuxConfig_BranchLink = 3,
} PinMuxConfigStates;

typedef enum {
    PinMuxOpcode_ConfigEnd = 0,
    PinMuxOpcode_ModuleDone = 1,
    PinMuxOpcode_SubroutinesDone = 2,
} PinMuxConfigExtendOpcodes;

//  When the state is BranchLink, this is the number of words to increment the current "PC"
#define MUX_ENTRY_0_BRANCH_ADDRESS_RANGE 31:2
//  The incr1 offset from TRISTATE_REG_A_0 to the pad group's tristate register
#define MUX_ENTRY_0_TS_OFFSET_RANGE 31:26
//  The bit position within the tristate register for the pad group
#define MUX_ENTRY_0_TS_SHIFT_RANGE 25:21
//  The incr1 offset from PIN_MUX_CTL_A_0 to the pad group's pin mux control register
#define MUX_ENTRY_0_MUX_CTL_OFFSET_RANGE 20:17
//  The bit position within the pin mux control register for the pad group
#define MUX_ENTRY_0_MUX_CTL_SHIFT_RANGE 16:12
//  The mask for the pad group -- expanded to 3b for forward-compatibility
#define MUX_ENTRY_0_MUX_CTL_MASK_RANGE 10:8
//  When a pad group needs to be owned (or disowned), this value is applied
#define MUX_ENTRY_0_MUX_CTL_SET_RANGE 7:5
//  This value is compared against, to determine if the pad group should be disowned
#define MUX_ENTRY_0_MUX_CTL_UNSET_RANGE 4:2
//  for extended opcodes, this field is set with the extended opcode
#define MUX_ENTRY_0_OPCODE_EXTENSION_RANGE 3:2
//  The state for this entry
#define MUX_ENTRY_0_STATE_RANGE 1:0


#define MAX_NESTING_DEPTH 4

/*  This macro is used to generate 32b value to program the  tristate& pad mux control
 *  registers for config/unconfig for a padgroup
 */
#define PIN_MUX_ENTRY(TSOFF,TSSHIFT,MUXOFF,MUXSHIFT,MUXMASK,MUXSET,MUXUNSET,STAT) \
    (NV_DRF_NUM(MUX, ENTRY, TS_OFFSET, TSOFF) | NV_DRF_NUM(MUX, ENTRY, TS_SHIFT, TSSHIFT) | \
    NV_DRF_NUM(MUX, ENTRY, MUX_CTL_OFFSET, MUXOFF) | NV_DRF_NUM(MUX, ENTRY, MUX_CTL_SHIFT, MUXSHIFT) | \
    NV_DRF_NUM(MUX, ENTRY,MUX_CTL_MASK, MUXMASK) | NV_DRF_NUM(MUX, ENTRY,MUX_CTL_SET, MUXSET) | \
    NV_DRF_NUM(MUX, ENTRY, MUX_CTL_UNSET,MUXUNSET) | NV_DRF_NUM(MUX, ENTRY, STATE,STAT))

//  This is used to program the tristate & pad mux control registers for a pad group
#define CONFIG_VAL(TRISTATE_REG, MUXCTL_REG, PADGROUP, MUX) \
    (PIN_MUX_ENTRY(((APB_MISC_PP_TRISTATE_REG_##TRISTATE_REG##_0 - APB_MISC_PP_TRISTATE_REG_A_0)>>2), \
                APB_MISC_PP_TRISTATE_REG_##TRISTATE_REG##_0_Z_##PADGROUP##_SHIFT, \
                ((APB_MISC_PP_PIN_MUX_CTL_##MUXCTL_REG##_0 - APB_MISC_PP_PIN_MUX_CTL_A_0) >> 2), \
                APB_MISC_PP_PIN_MUX_CTL_##MUXCTL_REG##_0_##PADGROUP##_SEL_SHIFT, \
                APB_MISC_PP_PIN_MUX_CTL_##MUXCTL_REG##_0_##PADGROUP##_SEL_DEFAULT_MASK, \
                APB_MISC_PP_PIN_MUX_CTL_##MUXCTL_REG##_0_##PADGROUP##_SEL_##MUX, \
                0, PinMuxConfig_Set))

/*  This macro is used to compare a pad group against a potentially conflicting
 *  enum (where the conflict is caused by setting a new config), and to resolve the
 *  conflict by setting the conflicting pad group to a different, non-conflicting option.
 *  Read this as: if (PADGROUP) is equal to (CONFLICTMUX), replace it with (RESOLUTIONMUX)
 */
#define UNCONFIG_VAL(MUXCTL_REG, PADGROUP, CONFLICTMUX, RESOLUTIONMUX) \
    (PIN_MUX_ENTRY(0, 0, \
                  ((APB_MISC_PP_PIN_MUX_CTL_##MUXCTL_REG##_0 - APB_MISC_PP_PIN_MUX_CTL_A_0) >> 2), \
                  APB_MISC_PP_PIN_MUX_CTL_##MUXCTL_REG##_0_##PADGROUP##_SEL_SHIFT, \
                  APB_MISC_PP_PIN_MUX_CTL_##MUXCTL_REG##_0_##PADGROUP##_SEL_DEFAULT_MASK, \
                  APB_MISC_PP_PIN_MUX_CTL_##MUXCTL_REG##_0_##PADGROUP##_SEL_##RESOLUTIONMUX, \
                  APB_MISC_PP_PIN_MUX_CTL_##MUXCTL_REG##_0_##PADGROUP##_SEL_##CONFLICTMUX, \
                  PinMuxConfig_Unset))

#if NVRM_PINMUX_DEBUG_FLAG
#define CONFIG(TRISTATE_REG, MUXCTL_REG, PADGROUP, MUX) \
    (CONFIG_VAL(TRISTATE_REG, MUXCTL_REG, PADGROUP, MUX)), \
    (NvU32)(const void*)(#MUXCTL_REG "_0_" #PADGROUP "_SEL to " #MUX), \
    (NvU32)(const void*)(#TRISTATE_REG "_0_Z_" #PADGROUP)

#define UNCONFIG(MUXCTL_REG, PADGROUP, CONFLICTMUX, RESOLUTIONMUX) \
    (UNCONFIG_VAL(MUXCTL_REG, PADGROUP, CONFLICTMUX, RESOLUTIONMUX)), \
    (NvU32)(const void*)(#MUXCTL_REG "_0_" #PADGROUP "_SEL from " #CONFLICTMUX " to " #RESOLUTIONMUX), \
    (NvU32)(const void*)(NULL)
#else
#define CONFIG(TRISTATE_REG, MUXCTL_REG, PADGROUP, MUX) \
    (CONFIG_VAL(TRISTATE_REG, MUXCTL_REG, PADGROUP, MUX))
#define UNCONFIG(MUXCTL_REG, PADGROUP, CONFLICTMUX, RESOLUTIONMUX) \
    (UNCONFIG_VAL(MUXCTL_REG, PADGROUP, CONFLICTMUX, RESOLUTIONMUX))
#endif

/*  This macro is used for opcode entries in the tables */
#define PIN_MUX_OPCODE(_OP_) \
    (NV_DRF_NUM(MUX,ENTRY,STATE,PinMuxConfig_OpcodeExtend) | \
     NV_DRF_NUM(MUX,ENTRY,OPCODE_EXTENSION,(_OP_)))

/*  This is a dummy entry in the array which indicates that all setting/unsetting for
 *  a configuration is complete. */
#define CONFIGEND() PIN_MUX_OPCODE(PinMuxOpcode_ConfigEnd)

/*  This is a dummy entry in the array which indicates that the last configuration
 *  for the module instance has been passed. */
#define MODULEDONE()  PIN_MUX_OPCODE(PinMuxOpcode_ModuleDone)

/*  This is a dummy entry in the array which indicates that all "extra" configurations
 *  used by sub-routines have been passed. */
#define SUBROUTINESDONE() PIN_MUX_OPCODE(PinMuxOpcode_SubroutinesDone)

/*  This macro is used to insert a branch-and-link from one configuration to another */
#define BRANCH(_ADDR_) \
     (NV_DRF_NUM(MUX,ENTRY,STATE,PinMuxConfig_BranchLink) | \
      NV_DRF_NUM(MUX,ENTRY,BRANCH_ADDRESS,(_ADDR_)))


//  The below entries define the table format for GPIO Port/Pin-to-Tristate register mappings
//  Each table entry is 16b, and one is stored for every GPIO Port/Pin on the chip
#define MUX_GPIOMAP_0_TS_OFFSET_RANGE 15:10
//  Defines where in the 32b register the tristate control is located
#define MUX_GPIOMAP_0_TS_SHIFT_RANGE  4:0

#define TRISTATE_ENTRY(TSOFFS, TSSHIFT) \
    ((NvU16)(NV_DRF_NUM(MUX,GPIOMAP,TS_OFFSET,(TSOFFS)) | \
             NV_DRF_NUM(MUX,GPIOMAP,TS_SHIFT,(TSSHIFT))))

#define GPIO_TRISTATE(TRIREG,PADGROUP) \
    (TRISTATE_ENTRY(((APB_MISC_PP_TRISTATE_REG_##TRIREG##_0 - APB_MISC_PP_TRISTATE_REG_A_0)>>2), \
        APB_MISC_PP_TRISTATE_REG_##TRIREG##_0_Z_##PADGROUP##_SHIFT))


void
NvBootPadSetTriStates(
    const NvU32* Module,
    NvU32 Config,
    NvBool EnableTristate);

void
NvBootPadSetPinMuxCtl(
    const NvU32* Module,
    NvU32 Config);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif // NVRM_PINMUX_UTILS_H
