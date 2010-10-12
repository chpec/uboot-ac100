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
 * nvboot_util.h - Definition of utility functions that will be used in bootrom
 */

#ifndef INCLUDED_NVBOOT_UTIL_H
#define INCLUDED_NVBOOT_UTIL_H

#include <stdarg.h>
#include <stddef.h>
#include <nvcommon.h>

#if defined(__cplusplus)
extern "C"
{
#endif

/** NvBootUtilMemset - set a region of memory to a value.
 *
 *  @param s Pointer to the memory region
 *  @param c The value to be set in the memory
 *  @param size The length of the region
 */
void
NvBootUtilMemset( void *s, NvU32 c, size_t size );

/** NvBootUtilMemcpy - copy memory.from source location to the destination location
 *
 *  @param dest pointer to the destination for copy
 *  @param src pointer to the source memory
 *  @param size The length of the copy
 */
void * 
NvBootUtilMemcpy( void *dest, const void *src, size_t size );

/** NvBootUtilGetTimeUS - returns the system time in microseconds.
 *
 *  The returned values are guaranteed to be monotonically increasing,
 *  but may wrap back to zero.
 *
 *  Some systems cannot gauantee a microsecond resolution timer.
 *  Even though the time returned is in microseconds, it is not gaurnateed
 *  to have micro-second resolution.
 *
 *  Please be advised that this APIs is mainly used for code profiling and 
 *  meant to be used direclty in driver code.
 *
 */
NvU32 NvBootUtilGetTimeUS( void );

/** NvBootUtilElapsedTimeUS() - Returns the elapsed time in microseconds
 * 
 * This function is wraparound safe, the retruned value will be the real
 * delay (modulo 2^32 microseconds, i.e. more than one hour) 
 *
 *  @param NvU32 StartTime The start point of the time interval to measure
 *  @return NvU32 Elapsed time in microseconds since StartTime
 */
NvU32 NvBootUtilElapsedTimeUS( NvU32 StartTime );

/** NvBootWaitUS - stalls the calling thread for at least the given number of
 *      microseconds.  The actual time waited might be longer, you cannot
 *      depend on this function for precise timing.
 *
 * NOTE:  It is safe to use this function at ISR time.
 *
 *  @param usec The number of microseconds to wait
 */
void NvBootUtilWaitUS( NvU32 usec );

/** NvBootUtilWait100NS_OSC - Macro that inserts a delay of at least 100ns.
 *  The macro expands into essentially a string of NOPs - reading from
 *  a volatile memory location.
 *
 *  The macro assumes a max AVP clock of 26MHz, which is the maximum
 *  oscillator frequency.  With 1 wait state per iROM read, this is
 *  one instruction dispatch each 76.9ns.  2 instructions provide the
 *  >100ns delay.  With slower oscillators, the delay is naturally longer.
 *
 *  NOTE: This timing is very approximate.
 */
#define NVBOOT_UTIL_WAIT_100NS_OSC()  \
    do                                \
    {                                 \
        volatile char a, c;           \
        volatile char *pc = &c;       \
        a = *pc;                      \
    } while(0)

/** NvBootGetLog2Number - Converrs the size in bytes to units of log2(bytes)
 * Thus, a 1KB block size is reported as 10.
 *
 *  @param usec The number of microseconds to wait
 */
NvU32 NvBootUtilGetLog2Number(NvU32 size);

/** NvBootUtilSwapBytesInNvU32 - set a region of memory to a value.
 *
 *  @param Value The constant NvU32 value in which the bytes need to be swaped
 *  @return Swaped value
 */
NvU32
NvBootUtilSwapBytesInNvU32( const NvU32 Value );

/** NvBootUtilIsValidPadding - checks if the padding is valid or not (padding is supposed to be 0x80 followed by all 0x00's)
 *
 *  @param Padding points to the padding buffer
 *  @param Length length of the padding to be validated.
 *  @return NV_TRUE if the padding is valid else NV_FALSE
 */
NvBool
NvBootUtilIsValidPadding(NvU8 *Padding, NvU32 Length);

/**
 * Runtime termination of simulation/emulation.
 * Compiles out in release builds.
 */
    
/**
 * Termination status codes
 */
typedef enum
{
    /** an assert was encountered and failed */
    NvBootUtilSimStatus_Assert = 1,
    /** successful completion */
    NvBootUtilSimStatus_Pass = 2,
    /** unsuccessful completion */
    NvBootUtilSimStatus_Fail = 3,
    /** Recovery Mode was invoked; could not run to completion */
    NvBootUtilSimStatus_RecoveryMode = 4,
    NvBootUtilSimStatus_Num,
    NvBootUtilSimStatus_Force32 = 0x7FFFFFFF
} NvBootUtilSimStatus;
    
/*
 * Assertion support (initially borrowed from nvassert.h, then stripped
 * down for BootRom use)
 */
    
#ifndef NV_ASSERT

/**
 * Runtime condition check with break into debugger if the assert fails.
 * Compiles out in release builds.
 */
#if NV_DEBUG

#define NV_ASSERT(x) \
    do { \
        if (!(x)) \
        { \
            /* break into the debugger */ \
            (NvBootUtilSimStatus_Assert, __LINE__); \
        } \
    } while( 0 )

#else // NV_DEBUG
#define NV_ASSERT(x) do {} while(0)
#endif // NV_DEBUG
#endif

#ifndef NV_CT_ASSERT
/** NV_CT_ASSERT: compile-time assert for constant values.

    This works by declaring a function with an array parameter.  If the
    assert condition is true, then the array size will be 1, otherwise
    the array size will be -1, which will generate a compilation error.

    No code should be generated by this macro.

    Three levels of macros are needed to properly expand the line number.

    This macro was taken in spirit from:
        //sw/main/drivers/common/inc/nvctassert.h
 */
#define NV_CT_ASSERT( x )            NV_CT_ASSERT_I( x, __LINE__ )
#define NV_CT_ASSERT_I( x,line )     NV_CT_ASSERT_II( x, line )
#define NV_CT_ASSERT_II( x, line ) \
    void compile_time_assertion_failed_in_line_##line( \
        int _compile_time_assertion_failed_in_line_##line[(x) ? 1 : -1])
#endif

/* Common functions */

/** Macro for taking min or max of a pair of numbers */
#define NV_MIN(a,b) (((a) < (b)) ? (a) : (b))
#define NV_MAX(a,b) (((a) > (b)) ? (a) : (b))

/*
 * Ceiling function macros
 * NV_ICEIL(a, b)      Returns the ceiling of a divided by b.
 * NV_ICEIL_LOG2(a, b) Returns the ceiling of a divided by 2^b.
 */
#define NV_ICEIL(a,b)      (((a) +       (b)  - 1) /  (b))
#define NV_ICEIL_LOG2(a,b) (((a) + (1 << (b)) - 1) >> (b))

/**
 * A helper macro to check a function's error return code and propagate any
 * errors upward.  This assumes that no cleanup is necessary in the event of
 * failure.  This macro does not locally define its own NvError variable out of
 * fear that this might burn too much stack space, particularly in debug builds
 * or with mediocre optimizing compilers.  The user of this macro is therefore
 * expected to provide their own local variable "NvError e;".
 */
#define NV_BOOT_CHECK_ERROR(expr) \
    do \
    { \
        e = (expr); \
        if (e != NvBootError_Success) \
            return e; \
    } while (0)

/**
 * A helper macro to check a function's error return code and, if an error
 * occurs, jump to a label where cleanup can take place.  Like NV_CHECK_ERROR,
 * this macro does not locally define its own NvError variable.  (Even if we
 * wanted it to, this one can't, because the code at the "fail" label probably
 * needs to do a "return e;" to propagate the error upwards.)
 */
#define NV_BOOT_CHECK_ERROR_CLEANUP(expr) \
    do \
    { \
        e = (expr); \
        if (e != NvBootError_Success) \
            goto fail; \
    } while (0)

/**
 * Helper macros to convert between 32-bit addresses and pointers.
 * Their primary function is to document when pointers are assumed to be
 * 32-bit numbers.
 *
 * NOTE: Be careful when using these macros.  If the casts are used in code
 *       that reads & writes the addresses to hardware registers, the pointers
 *       will usually need to contain physical, not virtual, addresses.
 */
#define PTR_TO_ADDR(x) ((NvU32)(x))
#define ADDR_TO_PTR(x) ((NvU8*)(x))

#if defined(__cplusplus)
}
#endif

#endif /* #ifndef INCLUDED_NVBOOT_UTIL_H */
