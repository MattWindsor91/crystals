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

/** @file   src/modules/bindings-lua.h
 *  @author Alexander Preisinger
 *  @brief  Lua module for scripting.
 */

#ifndef _H_BINDINGS_LUA
#define _H_BINDINGS_LUA

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

/* Workaround for Windows DLL symbol load failures.
 *
 * All prototypes of outward-facing functions MUST be preceded with
 * EXPORT so that the DLL loader can see them.
 */

#ifdef PLATFORM_WINDOWS
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif /* PLATFORM_WINDOWS */


enum {
  L_SUCCESS,  /**< Lua return value if success. */
  L_FAILURE   /**< LUa return value if failure. */
};


/* GLOBAL VARIABLES */

lua_State *g_lua; /**< Main Lua state which holds the main stack. */


/* FUNCTION PROTOTYPES */

/** Initialise the lua module. */

EXPORT void
init (void);


/** Terminate the lua module. */

EXPORT void
term (void);


/** Execute the file
 *  The file has to be a valid lua source.
 *
 *  @param path Path to the file.
 *
 *  @return Returns SUCCESS if success else FAILURE.
 * */

EXPORT int
run_file (const char* path);


/** Generic parameter check function.
 *
 * Check if the parameters that are passed to the function in a lua script are
 * of the you need them and how many arguments.
 *
 * @param lua_State Pass the the lua state that holds the stack.
 * @param func_name The name of the function. To be displayed in error messages.
 * @param sig       A string of chars which defines how many and of which types
 * the paramaters should be. Add 's' for string, 'd' for number and 'b' for
 * boolean. (e.g.: "sdd" = 3 paramters, 1th stirng, 2nd numebr, 3rd number)
 *
 * @return Return SUCCESS if the values in the stack match the sig, eles return
 * FAILURE. (defined in "util.h")
 *
 * @todo Add a table type check if nessecary.
 */

static int
parameter_check (lua_State *L, const char *func_name, const char *sig);


/** Run a test
 * @todo Remove tests.
 */

static int
crystals_test (lua_State *L);

#endif /* _H_BINDINGS_LUA */

/* vim: set ts=2 sw=2 softtabstop=2: */
