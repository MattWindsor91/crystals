/*
* Crystals (working title)
*
* Copyright (c) 2010 Matt Windsor, Michael Walker and Alexander
* Preisinger.
*
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
*
* * Redistributions of source code must retain the above copyright
* notice, this list of conditions and the following disclaimer.
*
* * Redistributions in binary form must reproduce the above
* copyright notice, this list of conditions and the following
* disclaimer in the documentation and/or other materials provided
* with the distribution.
*
* * The names of contributors may not be used to endorse or promote
* products derived from this software without specific prior
* written permission.
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

/** @file bindings-lua.c
 * @author Alexander Preisinger
 * @brief Lua module for scripting
 */

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include <stdio.h>
#include "../util.h"

lua_State *g_lua;

enum {
  L_SUCCESS,
  L_FAILURE
};


/** Initialise the lua module. */

void
init (void);


/** Terminate the lua module. */

void
term (void);


/** Execute the file
 *  The file has to be a valid lua source
 *
 *  @param path Path to the file.
 *
 *  @return Returns SUCCESS if success else FAILURE
 * */

int
run_file (const char* path);

/** Run a test */

int
crystals_test (lua_State *L);

/* -- DEFINITIONS -- */

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
  lua_close(g_lua);
}

int
run_file (const char *path)
{
  if (luaL_loadfile(g_lua, path) == L_SUCCESS)
    {
      lua_pcall(g_lua, 0, 0, 0);
      return SUCCESS;
    }
  else
    {
      fprintf(stderr, "Couldn't open %s\n", path);
      return FAILURE;
    }
}

int
crystals_test (lua_State *L)
{
  /* Uff, I have to implement a generic argument check function. */
  if (lua_gettop(L) != 1)
    {
      fprintf(stderr, "LUA: crystals_tests: Only takes 1 argument.\n");
      return 0;
    }
  if (lua_type(L, 1) != LUA_TSTRING)
    {  
      fprintf(stderr, "LUA: crystals_tests: Argument has to be a string.\n");
      return 0;
    }
  /* END ARGUMENT CHECK */

  printf("%s\n", lua_tostring(L, 1));
  return 1;
}

/* vim: set st=2 sw=2 softtabstop=2: */
