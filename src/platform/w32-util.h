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

/** @file    w32-util.h
 *  @author  Matt Windsor
 *  @brief   Miscellaneous utility prototypes and declarations for Windows.
 *
 *  This, and its corresponding C file, contain things that are
 *  undeserving of their own code file, but nevertheless useful for
 *  the engine.
 */

#ifndef _W32_UTIL_H
#define _W32_UTIL_H

#include <stdarg.h>

/* -- MACROS -- */

/* mingw32 does not have strsafe.h, so use snprintf and vsnprintf instead of
 * StringCchPrintf and StringCchVPrintf.
 */

#ifdef NO_STRSAFE

#include <stdio.h>

#define W32_SAFE_SPRINTF snprintf
#define W32_SAFE_VSPRINTF vsnprintf

#else /* not NO_STRSAFE */

#include <strsafe.h>

#define W32_SAFE_SPRINTF StringCchPrintf
#define W32_SAFE_VSPRINTF StringCchVPrintf

#endif /* not NO_STRSAFE */


/* -- DECLARATIONS -- */


/**
 * Windows error reporting procedure. 
 *
 * This displays a message box with the error.
 *
 * This prints the error message to standard error.
 *
 * @param message   The error message/format string.
 * @param ap        The variadic arguments list passed from fatal or error.
 * @param is_fatal  Whether or not the error is fatal.  If the error is 
 *                  fatal, the program will be halted.
 */

void
w32_error (const char message[], va_list ap, int is_fatal);


#endif /* not _W32_UTIL_H */
