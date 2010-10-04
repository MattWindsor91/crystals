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

/** @file     src/main.c
 *  @author   Matt Windsor
 *  @brief    Main functions.
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "main.h"
#include "util.h"
#include "state.h"
#include "field/object.h"
#include "parser.h"
#include "module.h"
#include "events.h"
#include "graphics.h"
#include "bindings.h"
#include "field/mapview.h"
#include "field/map.h"

dict_t *g_config;

int
main (int argc, char **argv)
{
  /* Placeholder for command line stuff. */
  argc = argc;
  argv = argv;

  if (init () == SUCCESS)
    main_loop ();

  cleanup ();
  return 0;
}

int
init (void)
{
  char *module_path;
  
  g_config = config_dict_init();

  /* yeah I know that needs someting better */
  if (config_parse_file ("config/default.cfg", g_config) == SUCCESS)
    {
      module_path = config_get_value ("module_path", g_config);
    }
  else
    {
      module_path = malloc (sizeof (char) * strlen (DEFMODPATH) + 1);

      if (module_path == NULL)
        {
          fatal ("MAIN - init - Module path could not be allocated.");
          return FAILURE;
        }

      strncpy(module_path, DEFMODPATH, strlen (DEFMODPATH) + 1);
    }

  if (init_modules (module_path) == FAILURE)
    {
      fatal ("MAIN - init - Module initialisation failed.");
      return FAILURE;
    }

  if (init_graphics () == FAILURE)
    {
      fatal ("MAIN - init - Graphics initialisation failed.");
      return FAILURE;
    }

  if (init_objects () == FAILURE)
    {
      fatal ("MAIN - init - Object system initialisation failed.");
      return FAILURE;
    }

  init_bindings ();
  run_file ("tests/lua.lua");
  init_events ();

  /* We need to manually "flush" the state so the first state is set
     immediately instead of after a main loop frame - otherwise, the
     current state would be null and the engine would crash. */

  if (set_state (STATE_FIELD == FAILURE))
    {
      fatal ("MAIN - init - Couldn't enqueue state.");
      return FAILURE;
    }

  return SUCCESS;
}


void
main_loop (void)
{
  while (update_state () != STATE_QUIT)
    {
      state_frame_updates ();
      (*g_modules.gfx.update_screen) ();
      (*g_modules.event.process_events) ();
    }
}

void
cleanup (void)
{
  if (get_state () != STATE_QUIT)
    cleanup_state (get_state ());

  cleanup_objects ();
  cleanup_events ();
  cleanup_graphics ();
  cleanup_bindings ();
  cleanup_modules ();
  config_free_dict (g_config);
}

/* vim: set ts=2 sw=2 softtabstop=2: */
