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

/** @file   src/bindings/lua.h
 *  @author Alexander Preisinger
 *  @brief  Lua header file.
 */
 
#ifndef _LUA_B_H
#define _LUA_B_H

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "../util.h"

enum {
  L_SUCCESS,  /**< Lua return value if success. */
  L_FAILURE   /**< Lua return value if failure. */
};


lua_State *g_lua; /**< Main Lua state which holds the main stack. */


/* -- PROTOTYPES -- */

/** Generic parameter check function.
 *
 * Check if the parameters passed within lua matches the parameters needed in C.
 *
 * @param lua_State lua state that holds the stack.
 * @param func_name The name of the function. Used for error messages.
 * @param sig       A string of chars which defines how many and of which types
 * the paramaters should be. Add 's' for string, 'd' for number, 't' for a table
 * and 'b' for boolean.
 * (e.g.: "sdd" = 3 paramters, 1th stirng, 2nd numebr, 3rd number)
 *
 * @return Return SUCCESS if the values in the stack match the sig[] array,
 * else return FAILURE. (defined in "util.h")
 *
 * @todo Add more type checks if nessecary.
 */

bool_t
lua_parameter_check (lua_State *L, const char *func_name, const char sig[]);

#endif /* _LUA_B_H */
