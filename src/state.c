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

/** @file     src/state.c
 *  @author   Matt Windsor
 *  @brief    Game state machine.
 */

#include <stdlib.h>

#include "util.h"
#include "state.h"

#include "field/field.h"


/* -- STATIC GLOBAL VARIABLES -- */

static state_t sg_state = STATE_NULL;          /**< Current state. */
static state_t sg_enqueued_state = STATE_NULL; /**< Enqueued state. */
static struct state_functions sg_functions = {NULL,
                                              NULL, 
                                              NULL};  /**< Function table. */


/* -- DEFINITIONS -- */

/* Retrieve the current game state. */

state_t
get_state (void)
{
  return sg_state;
}


/* Change the current state. */

bool_t
set_state (state_t new_state)
{
  /* Can't change state if we're already quitting, can't change state
     to NULL,  and if we're trying to change to the current state then
     there must be a logic error about. */

  g_assert (sg_state  != STATE_QUIT);
  g_assert (new_state != STATE_NULL);
  g_assert (sg_state  != new_state);

  sg_enqueued_state = new_state;

  return SUCCESS;
}


/* Process an enqueued state change, if any, and return the current state. */

state_t
update_state (void)
{
  /* Check to see if there is an enqueued (non-null) state. */
  if (sg_enqueued_state == STATE_NULL)
    return sg_state;

  cleanup_state ();

  init_state (sg_enqueued_state);

  sg_state = sg_enqueued_state;
  sg_enqueued_state = STATE_NULL;

  return sg_state;
}


/* Initialise a state. */

bool_t
init_state (state_t state)
{
  g_assert (state == STATE_FIELD || state == STATE_QUIT);

  switch (state)
    {
    case STATE_FIELD:
      return init_field (&sg_functions);
      break;
    case STATE_QUIT:
      break;
    }

  return SUCCESS;
}


/* Perform frame updates for the current state. */

bool_t
state_frame_updates (void)
{
  if (sg_functions.update != NULL)
    return sg_functions.update ();
  else
    return FAILURE;
}



/* Instruct the current state to handle a dirty rectangle. */

bool_t
state_handle_dirty_rect (short x, short y,
                         unsigned short width, unsigned short height)
{
  if (sg_functions.dirty_rect != NULL)
    return sg_functions.dirty_rect (x, y, width, height);
  else
    return FAILURE;
}


/* Clean up a state. */

bool_t
cleanup_state (void)
{
  bool_t ret;

  if (sg_functions.cleanup != NULL)
    {
      ret = sg_functions.cleanup ();

      g_assert (ret != FAILURE);

      /* Clean up the function pointers. */

      sg_functions.cleanup    = NULL;
      sg_functions.update     = NULL;
      sg_functions.dirty_rect = NULL;
    }

  return SUCCESS;
}
