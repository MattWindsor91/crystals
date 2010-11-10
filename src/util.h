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

/** @file    util.h
 *  @author  Matt Windsor
 *  @brief   Miscellaneous utility prototypes and declarations.
 *
 *  This, and its corresponding C file, contain things that are
 *  undeserving of their own code file, but nevertheless useful for
 *  the engine.
 */

#ifndef _UTIL_H
#define _UTIL_H

#include <stdarg.h>

/* -- CONSTANTS -- */

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

/* -- DECLARATIONS -- */

/** Fatal error.
 *
 *  This prints an error message and sets the g_running variable to
 *  FALSE, effectively causing the engine to try gracefully shut down
 *  after the frame in progress.
 * 
 *  @param message  Message to print.  This wrapper automatically
 *  prepends and appends FATAL: and a newline respectively. 
 */

void
fatal (const char message[], ...);


/** Non-fatal error.
 *
 *  This prints an error message only.
 * 
 *  @param message  Message to print.  This wrapper automatically
 *  prepends and appends ERROR: and a newline respectively. 
 */

void
error (const char message[], ...);


/** Get the path to the directory in which all modules are stored.
 *
 *  @param module_path  Pointer in which to store the path string.  If
 *                      this has already been allocated, the function
 *                      will free it.
 */
void
get_module_root_path (char **module_path);


#endif /* not _UTIL_H */
