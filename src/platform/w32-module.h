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

/** @file     src/platform/w32-module.h
 *  @author   Matt Windsor
 *  @brief    Declarations for Windows-specific module loader functions.
 */


#ifndef _W32_MODULE_H
#define _W32_MODULE_H


#include <windows.h>


/* -- TYPEDEFS -- */

typedef HMODULE dll_handle;                  /**< Handle type used for DLL modules. */
typedef FARPROC mod_function_ptr;            /**< Function pointer type used for DLLs. */


/* -- FUNCTION MACROS -- */

#define DLLOPEN(x) LoadLibrary(x)            /**< Function used for opening DLLs. */
#define DLLLOOKUP(x,y) GetProcAddress(x,y)   /**< Function used for looking up DLL symbols. */
#define DLLCLOSE(x) FreeLibrary(x)           /**< Function used for closing DLLs. */
#define DLLERROR(x) w32_get_dll_error(x)     /**< Function used for getting DLL errors. */
#define MODULESUFFIX ".dll"                  /**< Suffix used for DLLs. */


/* -- DECLARATIONS -- */

/**
 * Raise the last DLL-acquisition error message, if any.
 *
 * This is the Windows API version of this function.
 *
 * @see std_get_dll_error
 *
 * @param function_name  The name of the function calling get_dll_error,
 *                       to display in the message.
 *
 */

void
w32_get_dll_error (const char function_name[]);


#endif /* _W32_MODULE_H */
