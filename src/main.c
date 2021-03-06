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
 * @file     src/main.c
 * @author   Matt Windsor
 * @brief    Main functions.
 */

#include "crystals.h"


/* -- CONSTANTS -- */

const char *DEFAULT_CONFIG_PATH = "config/default.cfg";


/* -- GLOBAL VARIABLES -- */

dict_t *g_config = NULL;


/* -- DEFINITIONS -- */

/* NB: For the Windows code entry point, see platform/w32-main.c. */

/* The main function. */

int
main (int argc, char **argv)
{
  /* Placeholder for command line stuff. */

  (void) argc;
  (void) argv;

  init ();
  main_loop ();

  cleanup ();
  return 0;
}


/* Initialise all engine subsystems. */

void
init (void)
{
  char *module_path = NULL;


  /* -- Configuration -- */

  g_config = init_config (DEFAULT_CONFIG_PATH);

  /* -- Module loader ("kernel") -- */

  get_module_root_path (&module_path);

  init_modules (module_path);
  init_graphics ();
  init_bindings ();
  init_events ();
  init_timer ();

  set_state (STATE_FIELD);

  run_script ("tests/lua.lua");
}


/* Execute the main loop of the program. */

void
main_loop (void)
{
  uint32_t delta;

  while (update_state () != STATE_QUIT)
    {
      delta = timer_get_delta ();
      state_frame_updates (delta);
      update_screen (delta);
      process_events (delta);
    }
}


/* Clean up all initialised subsystems. */

void
cleanup (void)
{
  if (get_state () != STATE_QUIT)
    cleanup_state ();

  cleanup_events ();
  cleanup_graphics ();
  cleanup_bindings ();
  cleanup_modules ();
  cfg_free (g_config);
}


/* vim: set et ts=2 sw=2 softtabstop=2: */
