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

/** @file    w32-types.h
 *  @author  Matt Windsor
 *  @brief   Type definitions for Windows.
 *
 *  This provides an implementation of stdint.h-style types in terms of
 *  Windows's own types, as well as other generic types such as bool_t.
 */

#ifndef _W32_TYPES_H
#define _W32_TYPES_H

#include <windows.h>

/* Defer to stdint.h, if it's known to be available
 * (for example on mingw32).
 */

#ifdef USE_STDINT

#include <stdint.h>

#else /* not USE_STDINT */

/* Mock stdint.h up, using Windows standard types where possible. */


/* These are standard Windows types and should scale properly: */

typedef BOOL          bool_t;    /**< Boolean (TRUE/FALSE or SUCCESS/FAILURE) type. */
typedef BYTE          uint8_t;   /**< Unsigned 8-bit integer. */
typedef WORD          uint16_t;  /**< Unsigned 16-bit integer. */
typedef DWORD         uint32_t;  /**< Unsigned 32-bit integer. */


/* These are not and may need fine-tuning later, but if we're running on
 * Windows then it's mostly safe to assume the system has a 32- or 64-bit standard
 * data type set.
 */

typedef signed char   int8_t;    /**< Signed 8-bit integer. */
typedef signed short  int16_t;   /**< Signed 16-bit integer. */
typedef signed int    int32_t;   /**< Signed 32-bit integer. */


#endif /* not USE_STDINT */

#endif /* not _W32_TYPES_H */
