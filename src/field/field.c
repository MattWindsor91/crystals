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
#include "../graphics.h" /* Temporary */

#include "field.h"
#include "map.h"
#include "mapload.h"
#include "mapview.h"
#include "object.h"
#include "object-api.h"


/* -- STATIC GLOBAL VARIABLES -- */

static map_t *sg_map;
static mapview_t *sg_mapview;

/* Test callbacks, woo */

static unsigned char sg_field_held_special_keys[256];

static event_callback_t *sg_field_skeyupcb;
static event_callback_t *sg_field_skeydowncb;
static event_callback_t *sg_field_quitcb;


/* -- DEFINITIONS -- */

/* - Callbacks - */

/* Callback for quit event. */

void
field_on_quit (event_t *event)
{
  event = event;
  set_state (STATE_QUIT);
}


/* Callback for special key up-presses. */

void
field_on_special_key_up (event_t *event)
{
  if (event->skey.code == SK_ESCAPE)
    field_on_quit (event);

  sg_field_held_special_keys[(int) event->skey.code] = FALSE;
}


/* Callback for special key down-presses. */

void
field_on_special_key_down (event_t *event)
{
  sg_field_held_special_keys[(int) event->skey.code] = TRUE;
}


/* Regular functions. */

/* Initialise the field state. */

bool_t
init_field (struct state_functions *function_table)
{
  memset (sg_field_held_special_keys, 0, sizeof (unsigned char) * 256);

  if (field_init_callbacks () == FAILURE)
    {
      fatal ("FIELD - init_field - Could not install event callbacks.");
      return FAILURE;
    }

  sg_map = load_map ("maps/test.map");

  if (sg_map == NULL)
    {
      fatal ("FIELD - init_field - Map initialisation failed.");
      return FAILURE;
    }

  sg_mapview = init_mapview (sg_map);

  if (sg_mapview == NULL)
    {
      fatal ("FIELD - init_field - Map view initialisation failed.");
      return FAILURE;
    }

  if (init_objects () == FAILURE)
    {
      fatal ("FIELD - init_field - Objects initialisation failed.");
      return FAILURE;
    }

  /* TEST DATA */

  {
    add_object ("Player", "null");
    add_object ("Test1", "null");
    add_object ("Test2", "null");
    
    tag_object ("Player", 1);
    tag_object ("Test1", 2);
    tag_object ("Test2", 1);

    change_object_image ("Player", "testobj.png", 32, 0, 48, 48);
    change_object_image ("Test1", "testobj.png", 0, 0, 16, 48);
    change_object_image ("Test2", "testobj.png", 16, 0, 16, 48);

    focus_camera_on_object ("Player");

    position_object ("Player",  200, 200, BOTTOM_LEFT);
    position_object ("Test1", 100, 100, BOTTOM_LEFT);
    position_object ("Test2", 90, 90, BOTTOM_LEFT);
  }

  /* Populate function pointers. */

  function_table->update = update_field;
  function_table->cleanup = cleanup_field;
  function_table->dirty_rect = field_handle_dirty_rect;

  return SUCCESS;
}


/* Retrieve the map view currently in use. */

mapview_t *
get_field_mapview (void)
{
  return sg_mapview;
}


/* Retrieve the boundaries of the map currently in use. */

bool_t
get_field_map_boundaries (int *x0_pointer,
                          int *y0_pointer,
                          int *x1_pointer,
                          int *y1_pointer)
{
  /* Sanity-check pointers. */

  if (x0_pointer == NULL
      || x1_pointer == NULL
      || y0_pointer == NULL
      || y1_pointer == NULL)
    {
      error ("FIELD - get_field_map_boundaries - Passed NULL pointer.");
      return FAILURE;
    }

  *x0_pointer = 0;
  *y0_pointer = 0;
  *x1_pointer = (sg_map->width * TILE_W) - 1;
  *y1_pointer = (sg_map->height * TILE_H) - 1;

  return SUCCESS;
}


/* Check to see if certain keys are held and handle the results. */

void
field_handle_held_keys (void)
{
  if (sg_field_held_special_keys[SK_UP])
    {
      move_object ("Player", 0, -1);
    }
  else if (sg_field_held_special_keys[SK_RIGHT])
    {
      move_object ("Player", 1, 0);
    }
  else if (sg_field_held_special_keys[SK_DOWN])
    {
      move_object ("Player", 0, 1);
    }
  else if (sg_field_held_special_keys[SK_LEFT])
    {
      move_object ("Player", -1, 0);
    }
}


/* Initialise input callbacks. */

bool_t
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


/* De-initialise input callbacks. */

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

bool_t
update_field (void)
{
  field_handle_held_keys ();
  render_map (sg_mapview);
  write_string (5, 5, 0, ALIGN_LEFT, "Test");

  return SUCCESS;
}


/* Handle a dirty rectangle passed from the user interface overlay for
   field. */

bool_t
field_handle_dirty_rect (short x, short y,
                         unsigned short width, unsigned short height)
{
  return mark_dirty_rect (sg_mapview,
                          x + sg_mapview->x_offset, 
                          y + sg_mapview->y_offset,
                          width, height);
}


/* De-initialise the field state. */

bool_t
cleanup_field (void)
{
  free_mapview (sg_mapview);
  free_map (sg_map);
  cleanup_objects ();

  field_cleanup_callbacks ();

  return SUCCESS;
}
