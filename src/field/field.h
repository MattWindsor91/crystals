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
 */

#ifndef _FIELD_H
#define _FIELD_H

#include "../events.h" /* event_t */

/* -- PROTOTYPES -- */

/** Initialise input callbacks.
 *
 *  @return SUCCESS for success, FAILURE otherwise.
 */

int
field_init_callbacks (void);

/** De-initialise input callbacks. */

void
field_cleanup_callbacks (void);

/** Callback for quit. 
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


/** Check to see if certain keys are held and handle the results. */

void
field_handle_held_keys (void);


/** Initialise the field state.
 *
 *  @return  SUCCESS if no errors were encountered; FAILURE otherwise. 
 */

int
init_field (void);


/** Perform per-frame updates for field. */

void
update_field (void);


/** De-initialise the field state.
 *
 *  @return  SUCCESS if no errors were encountered; FAILURE otherwise. 
 */

int
cleanup_field (void);

#endif /* not _FIELD_H */
