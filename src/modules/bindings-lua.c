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

#include <string.h>

#include "bindings-lua.h"
#include "../util.h"

void
init (void)
{
  g_lua = lua_open ();
  luaL_openlibs (g_lua);
  lua_pushcfunction (g_lua, crystals_test);
  lua_setglobal (g_lua, "crystals_test");
}

void
term (void)
{
  lua_close (g_lua);
}

int
run_file (const char *path)
{
  if (luaL_loadfile (g_lua, path) == L_SUCCESS)
    {
      lua_pcall (g_lua, 0, 0, 0);
      return SUCCESS;
    }
  else
    {
      fprintf (stderr, "Couldn't open %s\n", path);
      return FAILURE;
    }
}

int
crystals_test (lua_State *L)
{
  if (parameter_check (L, "crystals_test", "s") == FAILURE)
    return 0;

  printf("%s\n", lua_tostring (L, 1));
  return 1;
}

static int
parameter_check (lua_State *L, const char *func_name, const char sig[])
{
  int i;
  int pars;
  int pas_pars;

  pars = strlen (sig);
  pas_pars = lua_gettop (L);

  if (pars != pas_pars)
    {
      fprintf (stderr, "LUA: %s: Only takes %d argument, %d given.\n",
        func_name, pars, pas_pars);
      return 0;
    }

  for (i = 1; i <= pars; ++i)
    {
      switch (sig[i-1])
        {
          case 's':
            if (lua_type (L, i) != LUA_TSTRING)
              {
                fprintf (stderr, "LUA: %s: Parameter %d has to be a string.\n",
                  func_name, i);
                return 0;
              }
            break;
          case 'd':
            if (lua_type (L, i) != LUA_TNUMBER)
              {
                fprintf (stderr, "LUA: %s: Parameter %d has to be a number.\n",
                  func_name, i);
                return 0;
              }
            break;
          case 'b':
            if (lua_type (L, i) != LUA_TBOOLEAN)
              {
                fprintf (stderr, "LUA: %s: Parameter %d has to be a boolean.\n",
                  func_name, i);
                return 0;
              }
            break;
          default:
            fprintf (stderr, "LUA: parameter_check: Uknown sig character %c\n",
              sig[i]);
            return 0;
            break;
        }
    }
  return 1;
}

/* vim: set st=2 sw=2 softtabstop=2: */
