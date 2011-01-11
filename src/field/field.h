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

/** @file    src/field/field.h
 *  @author  Matt Windsor
 *  @brief   Prototypes and declarations for field state.
 *
 *  The field state handles the main game map.  It is composed of a
 *  map rendering engine, an object system, and a map loader.
 */

#ifndef _FIELD_H
#define _FIELD_H

#include "mapview.h"   /* mapview_t */
#include "../events.h" /* event_t */
#include "../state.h"  /* state_functions */
#include "../types.h"  /* bool_t */


/* -- PROTOTYPES -- */

/* - Callbacks - */

/** Callback for quit event.
 *
 *  @param event  The event produced by the quit.
 */


void
field_on_quit (event_t *event);


/** Callback for special key up-presses. 
 *
 *  @param event The event produced by the key press.
 */


void
field_on_special_key_up (event_t *event);


/** Callback for special key down-presses. 
 *
 *  @param event The event produced by the key press.
 */

void
field_on_special_key_down (event_t *event);


/* Regular functions */


/** Initialise input callbacks.
 *
 *  @return SUCCESS for success, FAILURE otherwise.
 */

bool_t
field_init_callbacks (void);


/** De-initialise input callbacks. */

void
field_cleanup_callbacks (void);


/** Initialise the field state.
 *
 *  @param function_table  Pointer to the function pointer to populate.
 *
 *  @return  SUCCESS if no errors were encountered; FAILURE otherwise. 
 */

bool_t
init_field (struct state_functions *function_table);


/** Retrieve the map view currently in use.
 *
 *  @return  Pointer to the current field map view.
 *
 */

mapview_t *
get_field_mapview (void);


/** Retrieve the boundaries of the map currently in use, in pixels.
 *
 *  @param  x0_pointer  Pointer to the variable in which to store the
 *                      X co-ordinate of the left edge of the map.
 *
 *  @param  y0_pointer  Pointer to the variable in which to store the
 *                      Y co-ordinate of the top edge of the map.
 *
 *  @param  x1_pointer  Pointer to the variable in which to store the
 *                      X co-ordinate of the right edge of the map.
 *
 *  @param  y1_pointer  Pointer to the variable in which to store the
 *                      Y co-ordinate of the bottom edge of the map.
 *
 *  @return  SUCCESS if no errors were encountered; FAILURE
 *           otherwise. 
 */

bool_t
get_field_map_boundaries (int *x0_pointer,
                          int *y0_pointer,
                          int *x1_pointer,
                          int *y1_pointer);


/** Check to see if certain keys are held and handle the results. */

void
field_handle_held_keys (void);


/** Perform per-frame updates for field.
 *
 *  @return  SUCCESS if no errors occurred, FAILURE otherwise.
 */

bool_t
update_field (void);


/** Handle a dirty rectangle passed from the user interface overlay for
 *  field.
 *
 *  @param x       X co-ordinate of the left edge of the rectangle.
 *  @param y       Y co-ordinate of the right edge of the rectangle.
 *  @param width   Width of the rectangle, in pixels.
 *  @param height  Height of the rectangle, in pixels.
 *
 *  @return  SUCCESS if no errors occurred, FAILURE otherwise.
 */

bool_t
field_handle_dirty_rect (short x, short y,
                         unsigned short width, unsigned short height);


/** De-initialise the field state.
 *
 *  @return  SUCCESS if no errors were encountered; FAILURE otherwise. 
 */

bool_t
cleanup_field (void);

#endif /* not _FIELD_H */
