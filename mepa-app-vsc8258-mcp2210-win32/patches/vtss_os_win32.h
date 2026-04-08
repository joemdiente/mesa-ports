// Copyright (c) 2004-2026 Microchip Technology Inc. and its subsidiaries.
// SPDX-License-Identifier: MIT

/**
 * \file
 * \brief Windows OS API
 * \details This header file describes OS functions for Windows
 * Note: 
 *  1. All Windows are little-endian
 */

#ifndef _VTSS_OS_WIN32_H_
#define _VTSS_OS_WIN32_H_

#define WIN32_LEAN_AND_MEAN
#define UNICODE     //Defining this prevents error "{ before ;" in compiling
#define NOMINMAX
#include <windows.h>
#include <winsock.h>

#define VTSS_OS_NTOHL(x) ntohl(x) /**< Convert a 32-bit value from network to host order */

#define uint unsigned int    /**< Define API uint type - unsigned 16 bits */
#define ulong unsigned long  /**< Define API ulong - unsigned 32 bits */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include <string.h>             /* For memset and friend (not in stdlib) */

#include <stdbool.h>
#include <errno.h>
#include <time.h>
#include <sys/time.h>

/** Sleep for \param nsec milliseconds */
#define VTSS_NSLEEP(nsec) (Sleep(0))
// Windows cannot do nanosecond sleep
// Maybe explore in the future https://www.geisswerks.com/ryan/FAQS/timing.html QueryPerformanceCounter
/** Sleep for \param msec milliseconds */
#define VTSS_MSLEEP(msec) (Sleep(msec))

/** \brief Timer structure */
typedef struct {
    struct timeval timeout;   /**< Timeout */
    struct timeval now;       /**< Time right now */
} vtss_mtimer_t;

#define VTSS_MTIMER_START(timer,msec) { \
    (void) gettimeofday(&((timer)->timeout),NULL);   \
    (timer)->timeout.tv_usec+=msec*1000; \
    if ((timer)->timeout.tv_usec>=1000000) { (timer)->timeout.tv_sec+=(timer)->timeout.tv_usec/1000000; (timer)->timeout.tv_usec%=1000000; } \
} /**< Start timer */

#define VTSS_MTIMER_TIMEOUT(timer) {(gettimeofday(&((timer)->now),NULL)==0 && timercmp(&((timer)->now),&((timer)->timeout),>))} /**< Timer timeout */

#define VTSS_MTIMER_CANCEL(timer) /**< No action in this implementation. */

/**< Time of day struct */
typedef struct {
    time_t sec; /**< Time of day in seconds */
} vtss_timeofday_t;

#define VTSS_TIME_OF_DAY(tod) { \
    struct timeval tve; \
    (void)gettimeofday(&tve,NULL);  \
    tod.sec = tve.tv_sec; \
} /**< Time of day macro */

// vtss_jaguar_1.c requires these defines. When compiling for the
// Genie board we're not in kernel mode and the application is 
// single-threaded, so we can live with not doing anything in these functions.
/**
 * VTSS_OS_SCHEDULER_FLAGS
 * VTSS_OS_SCHEDULER_LOCK(flags)
 * VTSS_OS_SCHEDULER_UNLOCK(flags)
 *   These functions are called by API code that consists of a user-level part
 *   and a deferred interrupt handler part.
 *   Only the user-level part will call the VTSS_OS_SCHEDULER_LOCK()/UNLOCK()
 *   functions, since it is assumed that the deferred interrupt handler will
 *   have atomic access throughout its execution.
 *   Each module within the API that contains such functionality will have
 *   an option to call either the VTSS_OS_SCHEDULER_(UN)LOCK() functions
 *   or the VTSS_OS_INTERRUPT_DISABLE()/RESTORE() functions.
 */
#define VTSS_OS_SCHEDULER_FLAGS         int
#define VTSS_OS_SCHEDULER_LOCK(flags)   do {flags = flags;} while (0); /**< Lock scheduler */
#define VTSS_OS_SCHEDULER_UNLOCK(flags) do {flags = flags;} while (0); /**< Unlock scheduler */

/**
 * VTSS_DIV64 - perform 64/32 bit division yelding 32 bit (at least) output
 */
#define VTSS_DIV64(dividend, divisor) ((dividend) / (divisor))
/**
 * VTSS_MOD64 - perform 64/32 bit modulus yelding 32 bit (at least) output
 */
#define VTSS_MOD64(dividend, divisor) ((dividend) % (divisor))
/**
 * VTSS_LABS - perform abs() on long
 */
#define VTSS_LABS(arg)                labs(arg)

/**
 * VTSS_LLABS - perform abs() on long long
 */
#define VTSS_LLABS(arg)               llabs(arg)

/**
 * VTSS_OS_CTZ(val32)
 *
 * Count trailing zeros of a 32-bit unsigned.
 * Requirements/examples:
 *   VTSS_OS_CTZ(0x00000001) =  0
 *   VTSS_OS_CTZ(0x80000000) = 31
 *   VTSS_OS_CTZ(0x00000000) >= 32 (if result is taken as unsigned; Most implementations return -1, and (u32)(-1) >= 32).
 *
 * \param val32 The value to decode
 *
 * \return Number of trailing zeroes - or - the bit index of the
 * lowest bit set in the input given.
 * 
 * \note __builtin_ctz() is included in GCC 3.2.2 and later according
 * to http://en.wikipedia.org/wiki/Find_first_set.
 *
 * Note: __builtin_ctzl() is undefined for zero input values.
 */
#define VTSS_OS_CTZ(val32) ((val32) == 0 ? 32 : __builtin_ctzl((unsigned long)val32))

/**
 * Count trailing zeros of a 64-bit unsigned.
 * Requirements/examples:
 *   VTSS_OS_CTZ64(0x00000000_00000001) =  0
 *   VTSS_OS_CTZ64(0x00000000_80000000) = 31
 *   VTSS_OS_CTZ64(0x00000001_00000000) = 32
 *   VTSS_OS_CTZ64(0x80000000_00000000) = 63
 *   VTSS_OS_CTZ64(0x00000000_00000000) >= 64 (if result is taken as unsigned; Most implementations return -1, and (u32)(-1) >= 64).
 */
#define VTSS_OS_CTZ64(val64) ({                            \
    u32 _r = VTSS_OS_CTZ((u32)(val64));                    \
    (val64) == 0 ? 64 :                                    \
    _r < 32 ? _r : 32 + VTSS_OS_CTZ((u32)((val64) >> 32)); \
})

#if defined(VTSS_NO_CTZL) || (defined(__GNUC__) && __GNUC__ < 3)
static inline int __builtin_ctzl(unsigned long val32)
{
    unsigned int i, mask = 1;
    for (i = 0; i < 32; i++) {
        if (val32 & mask) {
            return i;
        }
        mask <<= 1;
    }
    return i;
}
#endif

/**
 * Request OS to allocate size bytes of memory.
 *
 * The first argument is the number of bytes that must
 * be allocated. Type is size_t.
 *
 * The second argument is a mask of flags that the
 * implementation must obey. Type is vtss_mem_flags_t.
 *
 * The returned pointer should be at least 8-byte aligned,
 * to make it suitable for a struct.
 */

#define VTSS_OS_MALLOC(size, flags) malloc(size)

/**
 * Request OS to free memory previously allocated with
 * VTSS_OS_MALLOC().
 *
 * The first argument is the pointer previously
 * obtained with a call to VTSS_OS_MALLOC(). Type is
 * void *.
 *
 * The second argument is a mask of flags identical to
 * those passed to VTSS_OS_MALLOC() when the memory
 * was requested.
 */

#define VTSS_OS_FREE(ptr, flags) free(ptr)

/**
 * Wrap of call to rand() defined in stdlib.h
 */
#define VTSS_OS_RAND() rand()

#endif /* _VTSS_OS_WIN32_H_ */

