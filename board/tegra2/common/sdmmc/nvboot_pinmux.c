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

#include <common.h>
#include <asm/io.h>
#include <asm/arch/nv_drf.h>
#include <asm/arch/nvcommon.h>
#include <asm/arch/tegra2.h>
#include <asm/arch/nv_hardware_access.h>

#include "nvboot_pinmux_local.h"
#include "nvboot_util.h"

/*  Each of the pin mux configurations defined in the pin mux spreadsheet are
 *  stored in chip-specific tables.  For each configuration, every pad group
 *  that must be programmed is stored as a single 32b entry, where the register
 *  offset (for both the tristate and pin mux control registers), field bit
 *  position (ditto), pin mux mask, and new pin mux state are programmed.
 *
 *  The tables are microcode for a simple state machine.  The state machine
 *  supports subroutine call/return (up to 4 levels of nesting), so that
 *  pin mux configurations which have substantial repetition can be
 *  represented compactly by separating common portion of the configurations
 *  into a subroutine.  Additionally, the state machine supports
 *  "unprogramming" of the pin mux registers, so that pad groups which are
 *  incorrectly programmed to mux from a controller may be safely disowned,
 *  ensuring that no conflicts exist where multiple pad groups are muxing
 *  the same set of signals.
 *
 *  Each module instance array has a reserved "reset" configuration at index
 *  zero.  This special configuration is used in order to disown all pad
 *  groups whose reset state refers to the module instance.  When a module
 *  instance configuration is to be applied, the reset configuration will
 *  first be applied, to ensure that no conflicts will arise between register
 *  reset values and the new configuration, followed by the application of
 *  the requested configuration.
 *
 *  Furthermore, for controllers which support dynamic pinmuxing (i.e.,
 *  the "Multiplexed" pin map option), the last table entry is reserved for
 *  a "global unset," which will ensure that all configurations are disowned.
 *  This Multiplexed configuration should be applied before transitioning
 *  from one configuration to a second one.
 *
 *  The table data has been packed into a single 32b entry to minimize code
 *  footprint using macros similar to the hardware register definitions, so
 *  that all of the shift and mask operations can be performed with the DRF
 *  macros.
 */


/*  FindConfigStart searches through an array of configuration data to find the
 *  starting position of a particular configuration in a module instance array.
 *  The stop position is programmable, so that sub-routines can be placed after
 *  the last valid true configuration */

static const NvU32*
NvBootPadFindConfigStart(const NvU32* Instance,
    NvU32 Config,
    NvU32 EndMarker)
{
    NvU32 Cnt = 0;
    while ((Cnt < Config) && (*Instance!=EndMarker))
    {
        switch (NV_DRF_VAL(MUX, ENTRY, STATE, *Instance))
        {
        case PinMuxConfig_BranchLink:
        case PinMuxConfig_OpcodeExtend:
            if (*Instance==CONFIGEND())
                Cnt++;
            Instance++;
            break;
        default:
            Instance += NVRM_PINMUX_SET_OPCODE_SIZE;
            break;
        }
    }

    //  Ugly postfix.  In modules with bonafide subroutines, the last
    //  configuration CONFIGEND() will be followed by a MODULEDONE()
    //  token, with the first Set/Unset/Branch of the subroutine 
    //  following that.  To avoid leaving the "PC" pointing to a
    //  MODULEDONE() in the case where the first subroutine should be
    //  executed, fudge the "PC" up by one, to point to the subroutine.
    if (EndMarker==SUBROUTINESDONE() && *Instance==MODULEDONE())
        Instance++;

    if (*Instance==EndMarker)
        Instance = NULL;

    return Instance;
}


void
NvBootPadSetTriStates(
    const NvU32* Module,
    NvU32 Config,
    NvBool EnableTristate)
{
    int          StackDepth = 0;
    const NvU32 *Instance = NULL;
    const NvU32 *ReturnStack[MAX_NESTING_DEPTH+1];

    Instance = NvBootPadFindConfigStart(Module, Config, MODULEDONE());
    /*
     * The first stack return entry is NULL, so that when a ConfigEnd is
     * encountered in the "main" configuration program, we pop off a NULL
     * pointer, which causes the configuration loop to terminate.
     */
    ReturnStack[0] = NULL;

    /*
     * This loop iterates over all of the pad groups that need to be updated,
     * and updates the reference count for each appropriately.
     */
    while (Instance)
    {
        switch (NV_DRF_VAL(MUX,ENTRY, STATE, *Instance))
        {
            case PinMuxConfig_OpcodeExtend:
              /* 
               * Pop the most recent return address off of the return stack
               * (which will be NULL if no values have been pushed onto the
               * stack)
               */
              if (NV_DRF_VAL(MUX,ENTRY, OPCODE_EXTENSION, *Instance) ==
                  PinMuxOpcode_ConfigEnd)
              {
                  Instance = ReturnStack[StackDepth--];
              }
              /*
               * ModuleDone & SubroutinesDone should never be encountered
               * during execution, for properly-formatted tables.
               */
              else
              {
                  NV_ASSERT(0); // Logical entry in table
              }
              break;

            case PinMuxConfig_BranchLink:
                /*
                 * Push the next instruction onto the return stack if nesting
                 * space is available, and jump to the target.
                 */
                NV_ASSERT(StackDepth<MAX_NESTING_DEPTH);

                ReturnStack[++StackDepth] = Instance+1;

                Instance = NvBootPadFindConfigStart(
                    Module,
                    NV_DRF_VAL(MUX,ENTRY,BRANCH_ADDRESS,*Instance),
                    SUBROUTINESDONE());

                NV_ASSERT(Instance); // Invalid branch configuration in table
                break;

            case PinMuxConfig_Set:
                {
                    NvU32 TsOffs = NV_DRF_VAL(MUX,ENTRY, TS_OFFSET, *Instance);
                    NvU32 TsShift = NV_DRF_VAL(MUX,ENTRY, TS_SHIFT, *Instance);
            
                    // Perform the udpate
                    NvU32 Curr = NV_READ32(NV_ADDRESS_MAP_APB_MISC_BASE + 
                                           APB_MISC_PP_TRISTATE_REG_A_0 +
                                           4*TsOffs);
#ifdef DEBUG_PINMUX
printf("SetTri: R reg: 0x%08x, Curr: 0x%08x, Off 0x%x, Shift %d, Ena: %d\n",
                             NV_ADDRESS_MAP_APB_MISC_BASE +
                             APB_MISC_PP_TRISTATE_REG_A_0 +
                             4*TsOffs, Curr, 4*TsOffs, TsShift, EnableTristate);
#endif
                    Curr &= ~(1<<TsShift);
                    Curr |= (EnableTristate?1:0)<<TsShift;
#ifdef DEBUG_PINMUX
printf("SetTri: W reg: 0x%08x, Curr: 0x%08x\n",
                             NV_ADDRESS_MAP_APB_MISC_BASE +
                             APB_MISC_PP_TRISTATE_REG_A_0 +
                             4*TsOffs, Curr);
#endif
                    NV_WRITE32(NV_ADDRESS_MAP_APB_MISC_BASE + 
                               APB_MISC_PP_TRISTATE_REG_A_0 + 4*TsOffs,
                               Curr);

                }

                //  fall through.

                /*
                 * The "Unset" configurations are not applicable to tristate
                 * configuration, so skip over them.
                 */
            case PinMuxConfig_Unset:
                Instance += NVRM_PINMUX_SET_OPCODE_SIZE;
                break;       
        }
    }
}

/*  NvRmSetPinMuxCtl will apply new pin mux configurations to the pin mux
 *  control registers.  */
void
NvBootPadSetPinMuxCtl(
    const NvU32* Module,
    NvU32 Config)
{
    NvU32 MuxCtlOffset, MuxCtlShift, MuxCtlMask, MuxCtlSet, MuxCtlUnset;
    const NvU32 *ReturnStack[MAX_NESTING_DEPTH+1];
    const NvU32 *Instance;
    int StackDepth = 0;
    NvU32 Curr;

    ReturnStack[0] = NULL;

    Instance = NvBootPadFindConfigStart(Module, Config, MODULEDONE());

    //  Apply the new configuration, setting / unsetting as appropriate
    while (Instance)
    {
        switch (NV_DRF_VAL(MUX,ENTRY, STATE, *Instance))
        {
        case PinMuxConfig_OpcodeExtend:
            if (NV_DRF_VAL(MUX,ENTRY, OPCODE_EXTENSION, *Instance)==PinMuxOpcode_ConfigEnd)
            {
                Instance = ReturnStack[StackDepth--];
            }
            else
            {
                NV_ASSERT(0); //Logical entry in table
            }
            break;
        case PinMuxConfig_BranchLink:
            NV_ASSERT(StackDepth<MAX_NESTING_DEPTH);
            ReturnStack[++StackDepth] = Instance+1;
            Instance = NvBootPadFindConfigStart(Module,NV_DRF_VAL(MUX,ENTRY,BRANCH_ADDRESS,*Instance),SUBROUTINESDONE());
            NV_ASSERT(Instance); // Invalid branch configuration in table
            break;
        default:
        {
            MuxCtlOffset = NV_DRF_VAL(MUX, ENTRY, MUX_CTL_OFFSET, *Instance);
            MuxCtlShift  = NV_DRF_VAL(MUX, ENTRY, MUX_CTL_SHIFT,  *Instance);
            MuxCtlUnset  = NV_DRF_VAL(MUX, ENTRY, MUX_CTL_UNSET,  *Instance);
            MuxCtlSet    = NV_DRF_VAL(MUX, ENTRY, MUX_CTL_SET,    *Instance);
            MuxCtlMask   = NV_DRF_VAL(MUX, ENTRY, MUX_CTL_MASK,   *Instance);
            
            Curr = NV_READ32(NV_ADDRESS_MAP_APB_MISC_BASE +
                             APB_MISC_PP_PIN_MUX_CTL_A_0 +
                             4*MuxCtlOffset);
#ifdef DEBUG_PINMUX
printf("SetPin: R reg: 0x%08x, Curr: 0x%08x, Off 0x%x, shift %d\n",
                             NV_ADDRESS_MAP_APB_MISC_BASE +
                             APB_MISC_PP_PIN_MUX_CTL_A_0 +
                             4*MuxCtlOffset, Curr, 4*MuxCtlOffset, MuxCtlShift);
#endif            
            if (NV_DRF_VAL(MUX,ENTRY,STATE,*Instance)==PinMuxConfig_Set)
            {
                Curr &= ~(MuxCtlMask<<MuxCtlShift);
                Curr |= (MuxCtlSet<<MuxCtlShift);
            }
            else if (((Curr>>MuxCtlShift)&MuxCtlMask)==MuxCtlUnset)
            {
                NV_ASSERT(NV_DRF_VAL(MUX,ENTRY,STATE,*Instance)==PinMuxConfig_Unset);
                Curr &= ~(MuxCtlMask<<MuxCtlShift);
                Curr |= (MuxCtlSet<<MuxCtlShift);
            }
            
#ifdef DEBUG_PINMUX
printf("SetPin: W reg: 0x%08x, Curr: 0x%08x\n",
                             NV_ADDRESS_MAP_APB_MISC_BASE +
                             APB_MISC_PP_PIN_MUX_CTL_A_0 +
                             4*MuxCtlOffset, Curr);
#endif
            NV_WRITE32(NV_ADDRESS_MAP_APB_MISC_BASE +
                       APB_MISC_PP_PIN_MUX_CTL_A_0 +
                       4*MuxCtlOffset,
                       Curr);

            Instance += NVRM_PINMUX_SET_OPCODE_SIZE;
            break;
        }
        }
    }
}
