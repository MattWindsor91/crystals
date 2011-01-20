/*
 * Crystals (working title) 
 *
 * Copyright (c) 2010 Matt Windsor, Michael Walker and Alexander
 *                    Preisinger.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *
 *   * The names of contributors may not be used to endorse or promote
 *     products derived from this software without specific prior
 *     written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * AFOREMENTIONED COPYRIGHT HOLDERS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file    util.h
 * @author  Matt Windsor
 * @brief   Miscellaneous utility prototypes and declarations.
 *
 * This, and its corresponding C file, contain things that are
 * undeserving of their own code file, but nevertheless useful for
 * the engine.
 */


#ifndef _UTIL_H
#define _UTIL_H

#include <stdarg.h>

#include "types.h"  /* Standard types as used in crystals. */


/* -- CONSTANTS -- */

/* Windows likes to define these already. */

#ifdef TRUE
#undef TRUE
#endif /* TRUE */

#ifdef FALSE
#undef FALSE
#endif /* FALSE */

enum
  {
    SUCCESS = 1, /**< Value raised by functions upon success. */
    FAILURE = 0, /**< Value raised by functions upon failure. */
    TRUE = 1,    /**< Boolean true value. */
    FALSE = 0,   /**< Boolean false value. */
    NORTH = 0,   /**< North (up) cardinal direction. */
    EAST,        /**< East (right) cardinal direction. */
    SOUTH,       /**< South (down) cardinal direction. */
    WEST,        /**< West (left) cardinal direction. */
    NULLC = '\0' /**< Null character (necessary?) */
  };


/* -- FUNCTION MACROS -- */

#define MAX(x, y) ((x) > (y) ? (x) : (y)) /**< Get the maximum of two
                                             values. */

#define MIN(x, y) ((x) < (y) ? (x) : (y)) /**< Get the minimum of two
                                             values. */


/* - PLATFORM-DEPENDENCY FUNCTION MACROS - */

/* ~~ Windows */

#ifdef PLATFORM_WINDOWS
#define UTIL_PLATFORM_MACROS  /**< If set, platform macros have been set. */

#include "platform/w32-util.h" /* Windows error procedures */

/** The procedure to use for outputting an error message. */
#define ERROR_PROCEDURE w32_error

#endif /* PLATFORM_WINDOWS */


/* ~~ Generic/standard */

#ifndef UTIL_PLATFORM_MACROS
#define UTIL_PLATFORM_MACROS  /**< If set, platform macros have been set. */

/** The procedure to use for outputting an error message. */
#define ERROR_PROCEDURE std_error

#endif /* not UTIL_PLATFORM_MACROS */


/* -- DECLARATIONS -- */

/* ~~ Path retrieval */

/**
 * Get the path to the directory in which all modules are stored.
 *
 * @param module_path  Pointer in which to store the path string.  If
 *                     this has already been allocated, the function
 *                     will free it.
 */

void
get_module_root_path (char **module_path);


/* ~~ Safe type conversions */

/** Safely convert a long integer to an unsigned short. 
 *
 *  This will raise an error and return a truncated value if the
 *  integer lies outside the bounds of the unsigned short range.
 *
 *  @param integer  The integer to attempt to convert.
 *
 *  @return  The converted integer in unsigned short form.
 */

unsigned short
long_to_uint16 (long integer);


/** Safely convert a long integer to signed short. 
 *
 *  This will raise an error and return a truncated value if the
 *  integer lies outside the bounds of the signed short range.
 *
 *  @param integer  The integer to attempt to convert.
 *
 *  @return  The converted integer in signed short form.
 */

short
long_to_int16 (long integer);


/** Safely convert an unsigned long integer to an unsigned short. 
 *
 *  This will raise an error and return a truncated value if the
 *  integer lies outside the bounds of the unsigned short range.
 *
 *  @param integer  The integer to attempt to convert.
 *
 *  @return  The converted integer in unsigned short form.
 */

unsigned short
ulong_to_uint16 (unsigned long integer);


/** Safely convert an unsigned long integer to signed short. 
 *
 *  This will raise an error and return a truncated value if the
 *  integer lies outside the bounds of the signed short range.
 *
 *  @param integer  The integer to attempt to convert.
 *
 *  @return  The converted integer in signed short form.
 */

short
ulong_to_int16 (unsigned long integer);


/* ~~ Error reporting */

/**
 * Fatal error.
 *
 * This shows an error message and attempts to immediately shut down
 * the game.
 * 
 * @param message  Message to print.  This function automatically
 *                 prepends and appends FATAL: and a newline respectively. 
 */

void
fatal (const char message[], ...);


/** 
 * Non-fatal error.
 *
 * This shows an error message only.
 * 
 * @param message  Message to print.  This function automatically
 *                 prepends and appends ERROR: and a newline respectively. 
 */

void
error (const char message[], ...);


/**
 * Standard error reporting procedure. 
 *
 * This prints the error message to standard error.
 *
 * Alternative error procedures are defined wherever more suitable alternatives
 * are available (eg GUI message boxes).
 *
 * @param message   The error message/format string.
 * @param ap        The variadic arguments list passed from fatal or error.
 * @param is_fatal  Whether or not the error is fatal.  If the error is 
 *                  fatal, the program will be halted.
 */

void
std_error (const char message[], va_list ap, int is_fatal);


#endif /* not _UTIL_H */
