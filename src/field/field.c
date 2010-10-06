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

/** @file     src/field/field.c
 *  @author   Matt Windsor
 *  @brief    Field state.
 */

#include <stdlib.h>
#include <string.h>

#include "../util.h"
#include "../events.h"
#include "../state.h"

#include "field.h"
#include "map.h"
#include "mapview.h"
#include "object.h"

/* -- STATIC GLOBAL VARIABLES -- */

static struct map *sg_map;
static struct map_view *sg_mapview;

/* Test callbacks, woo */

static unsigned char sg_field_held_special_keys[256];

static event_callback *sg_field_skeyupcb;
static event_callback *sg_field_skeydowncb;
static event_callback *sg_field_quitcb;

/* -- DEFINITIONS -- */

/* Callbacks */

void
field_on_quit (event_t *event)
{
  event = event;
  set_state (STATE_QUIT);
}


void
field_on_special_key_up (event_t *event)
{
  if (event->skey.code == SK_ESCAPE)
    field_on_quit (event);

  sg_field_held_special_keys[(int) event->skey.code] = 0;
}


void
field_on_special_key_down (event_t *event)
{
  sg_field_held_special_keys[(int) event->skey.code] = 1;
}


/* Regular functions. */

int
init_field (void)
{
  memset (sg_field_held_special_keys, 0, sizeof (unsigned char) * 256);

  /* Test callbacks, woo */

  if (field_init_callbacks () == FAILURE)
    {
      fatal ("FIELD - init_field - Could not install event callbacks.");
      return FAILURE;
    }

  sg_map = init_test_map ();

  if (sg_map == NULL)
    {
      fatal ("FIELD - init_field - Map initialisation failed.");
      return FAILURE;
    }

  if (init_objects () == FAILURE)
    {
      fatal ("FIELD - init_field - Objects initialisation failed.");
      return FAILURE;
    }

  sg_mapview = init_mapview (sg_map);

  if (sg_mapview == NULL)
    {
      fatal ("FIELD - init_field - Map view initialisation failed.");
      return FAILURE;
    }

  return SUCCESS;
}


void
field_handle_held_keys (void)
{
  struct object_t *player;

  player = get_object ("Player", NULL);

  if (player == NULL)
    {
      error ("FIELD - field_handle_held_keys - Couldn't grab player.");
      return;
    }

  if (sg_field_held_special_keys[SK_UP])
    {
      scroll_map (sg_mapview, NORTH);
      move_object (player, sg_mapview, 0, -1);
    }
  else if (sg_field_held_special_keys[SK_RIGHT])
    {
      scroll_map (sg_mapview, EAST);
      move_object (player, sg_mapview, 1, 0);
    }
  else if (sg_field_held_special_keys[SK_DOWN])
    {
      scroll_map (sg_mapview, SOUTH);
      move_object (player, sg_mapview, 0, 1);
   }
  else if (sg_field_held_special_keys[SK_LEFT])
    {
      scroll_map (sg_mapview, WEST);
      move_object (player, sg_mapview, -1, 0);
    }
}


int
field_init_callbacks (void)
{
  sg_field_skeyupcb = install_callback (field_on_special_key_up, SPECIAL_KEY_UP_EVENT);
  sg_field_skeydowncb = install_callback (field_on_special_key_down, SPECIAL_KEY_DOWN_EVENT);
  sg_field_quitcb = install_callback (field_on_quit, QUIT_EVENT);

  if (sg_field_skeyupcb
      && sg_field_skeydowncb
      && sg_field_quitcb)
    return SUCCESS;
  else
    {
      /* Clean up callbacks if any failed. */
      field_cleanup_callbacks ();
      return FAILURE;
    }
}


void
field_cleanup_callbacks (void)
{ 
  if (sg_field_skeyupcb)
    unload_callback (sg_field_skeyupcb);

  if (sg_field_skeydowncb)
    unload_callback (sg_field_skeydowncb);

  if (sg_field_quitcb)
    unload_callback (sg_field_quitcb);

  sg_field_skeyupcb = sg_field_skeydowncb = sg_field_quitcb = NULL;
}

/* Perform per-frame updates for field. */

void
update_field (void)
{
  field_handle_held_keys ();
  render_map (sg_mapview);

}



/* De-initialise the field state. */

int
cleanup_field (void)
{
  cleanup_mapview (sg_mapview);
  cleanup_map (sg_map);
  cleanup_objects ();

  field_cleanup_callbacks ();

  return SUCCESS;
}
