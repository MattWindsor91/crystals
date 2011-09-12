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


#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "main.h"
#include "util.h"
#include "state.h"
#include "parser.h"
#include "dialog.h"
#include "module.h"
#include "events.h"
#include "graphics.h"
#include "bindings/bindings.h"


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


  if (init () == SUCCESS)
    main_loop ();


  cleanup ();
  return 0;
}

void
test_dialog (void)
{
  req_t *req = NULL;
  dlg_t *dlg = NULL;
  dlg_content_t *con = NULL;

  dlg = dlg_parse_file ("duster_event_test.dlg.xml");

  printf("Requirements\n");
  while ((req = dlg_requirement_next (dlg)) != NULL)
    {
      printf("%d\n", req->type);
    }

  while ((con = dlg_content_next (dlg, 1)) != NULL)
    {
      if (con->type == TEXT)
        printf("%s: %s\n", con->action.text.actor_id, con->action.text.text);
    }

  dlg_free (dlg);

  cleanup_xml ();
}
/* Initialise all engine subsystems. */

bool_t
init (void)
{
  char *module_path = NULL;

  test_dialog ();
  /* -- Configuration -- */

  g_config = init_config (DEFAULT_CONFIG_PATH);

  if (g_config == NULL)
    {
      fatal ("MAIN - init - Config parser initialisation failed.");
      return FAILURE;
    }


  /* -- Module loader ("kernel") -- */

  get_module_root_path (&module_path);

  if (module_path == NULL)
    {
      fatal ("MAIN - init - Failed to retrieve module path.");
      return FAILURE;
    }

  if (init_modules (module_path) == FAILURE)
    {
      fatal ("MAIN - init - Module initialisation failed.");
      return FAILURE;
    }


  /* -- Graphics subsystem -- */

  if (init_graphics () == FAILURE)
    {
      fatal ("MAIN - init - Graphics initialisation failed.");
      return FAILURE;
    }


  /* -- Bindings subsystem -- */

  if (init_bindings () == FAILURE)
    {
      fatal ("MAIN - init - Bindings initialisation failed.");
      return FAILURE;
    }

  run_script ("tests/lua.lua");


  /* -- Events subsystem -- */

  if (init_events () == FAILURE)
    {
      fatal ("MAIN - init - Events initialisation failed.");
      return FAILURE;
    }


  /* -- State machine -- */

  if (set_state (STATE_FIELD) == FAILURE)
    {
      fatal ("MAIN - init - Couldn't enqueue state.");
      return FAILURE;
    }


  /* -- Finish -- */

  return SUCCESS;
}


/* Execute the main loop of the program. */

void
main_loop (void)
{
  while (update_state () != STATE_QUIT)
    {
      state_frame_updates ();
      update_screen ();
      process_events ();
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
