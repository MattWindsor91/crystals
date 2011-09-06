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
 * @file    src/bindings/lua.c
 * @author  Alexander Preisinger
 * @brief   Lua module for scripting. Contains to most basic functions for 
 *          embedding the lua interpreter and testing.
 */

#include <stdio.h>
#include <string.h>

#include "lua.h" /* includes ../util.h */
#include "bindings.h"

/* -- LUA PROTOTYPES -- */

/** 
 * Run a test
 * @todo Remove tests.
 */

static int
power_test (lua_State *L);

static int
crystals_test (lua_State *L);


/* -- INTERNAL DEFINITIONS -- */

bool_t
init_bindings (void)
{
  g_lua = lua_open ();
  luaL_openlibs (g_lua);
  
  lua_pushcfunction (g_lua, crystals_test); /* push the c function on the lua stack*/
  lua_setglobal (g_lua, "crystals_test"); /* make the c function visible for lua */
  
  lua_register (g_lua, "power_test", power_test); /* optional way */
  
  return SUCCESS;
}

void
cleanup_bindings (void)
{
  lua_close (g_lua);
}

bool_t
run_script (const char *path)
{
  if (luaL_loadfile (g_lua, path) == L_SUCCESS)
    {
      lua_pcall (g_lua, 0, 0, 0);
      return SUCCESS;
    }
  else
    {
      error ("Couldn't open %s\n", path);
      return FAILURE;
    }
}

/* -- LUA DEFINITIONS -- */

static int
power_test (lua_State *L)
{
  lua_Integer x;
  lua_Integer y;
  lua_Integer res;
  
  /** 
   * @note  if you call the c function in lua the parameters are on the 
   *        lua stack thats why we need to check if they are of the right type 
   */
  if (lua_parameter_check (L, "power_test", "dd") == FAILURE)
    return L_FAILURE;
    
  y = lua_tointeger (L, 2); /* get the first parameter from the lua stack*/
  x = lua_tointeger (L, 1); /* get the second parameter */
  
  lua_getfield (L, LUA_GLOBALSINDEX, "power"); /* move the function lua power
    to the lua stack */
  lua_pushinteger (L, x); /* push first parameter to the stack*/
  lua_pushinteger (L, y); /* push second parameter to the stack */
  lua_call (L, 2, 1); /* call the function with 2 parameters and 1 result */
  res = lua_tointeger (L, -1); /* get the result from the stack and convert it
    to an integer */
  

  /* Write the result. */

  lua_getfield (L, LUA_GLOBALSINDEX, "io");
  lua_getfield (L, -1, "write");
  lua_pushinteger (L, x);
  lua_pushstring (L, " ^ ");
  lua_pushinteger (L, y);
  lua_pushstring (L, " = ");
  lua_pushinteger (L, res);
  lua_pushstring (L, "\n");
  lua_call (L, 6, 0);
  /* in summary "io.write(x, " ^ ", y, " = ", res ,"\n")" */
  
  return L_SUCCESS;
}

static int
crystals_test (lua_State *L)
{
  if (lua_parameter_check (L, "crystals_test", "s") == FAILURE)
    return L_FAILURE;

  printf ("%s\n", lua_tostring (L, 1));
  return L_SUCCESS;
}

bool_t
lua_parameter_check (lua_State *L, const char *func_name, const char sig[])
{
  int i;
  int parameters;
  int passed_parameters;

  /* to prevent type conversation warnings on 64 bit systems */
  parameters = (int) strlen (sig);
  passed_parameters = lua_gettop (L);

  if (parameters != passed_parameters)
    {
      error ("LUA: %s: Takes %d argument, %d given.", func_name, parameters,
        passed_parameters);
      return FAILURE;
    }

  for (i = 1; i <= parameters; ++i)
    {
      switch (sig[i-1])
        {
          case 's':
            if (!lua_isstring (L, i))
              {
                error ("LUA: %s: Parameter %d has to be a string.",
                  func_name, i);
                return FAILURE;
              }
            break;
          case 'd':
            if (!lua_isnumber (L, i))
              {
                error ("LUA: %s: Parameter %d has to be a number.",
                  func_name, i);
                return FAILURE;
              }
            break;
          case 'b':
            if (!lua_isboolean (L, i))
              {
                error ("LUA: %s: Parameter %d has to be a boolean.",
                  func_name, i);
                return FAILURE;
              }
          case 't':
            if (!lua_istable (L, i))
              {
                error ("LUA: %s: Parameter %d has to be a table.",
                  func_name, i);
                return FAILURE;
              }
            break;
          default:
              error ("LUA: parameter_check: Uknown sig character %c", sig[i]);
            return FAILURE;
            break;
        }
    }
  return SUCCESS; 
}

/* vim: set et ts=2 sw=2 softtabstop=2: */
