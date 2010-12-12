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

/** @file    src/state.h
 *  @author  Matt Windsor
 *  @brief   Prototypes and declarations for game state machine.
 *
 *  The game's master finite state machine directs the engine to
 *  perform certain per-frame tasks depending on the current game
 *  "state".  For example, if the game is in the state STATE_FIELD,
 *  the engine will task itself with drawing the field map.
 *  Similarly, in STATE_BATTLE the battle engine's operations will be
 *  run instead.
 */

#ifndef _STATE_H
#define _STATE_H

/* -- TYPE DEFINITIONS -- */

typedef unsigned char state_t; /**< State identifier type. */


/* -- CONSTANTS -- */

enum
  {
    STATE_NULL  = 0, /**< FSM null state. */
    STATE_FIELD = 1, /**< FSM field map state. */
    STATE_QUIT  = 42 /**< FSM quit state. */
  };


/* -- PROTOTYPES -- */

/** Retrieve the current game state.
 *
 *  @return  the current game state. 
 */

state_t
get_state (void);


/** Change the current game state.
 *
 *  State changes are actually "lazy" - in order to ensure the
 *  integrity of code executing directly after the state change, this
 *  function does not actually immediately change the state but
 *  instead enqueues a state change (facilitated by the function
 *  change_to_enqueued_state) to occur at the beginning of the next
 *  frame.
 *
 *  @param new_state  The state to attempt to change to.
 *
 *  @return  SUCCESS if no errors occurred, FAILURE otherwise (for
 *  example, trying to enqueue a state change during a quit state).
 */

int
set_state (state_t new_state);


/** Process an enqueued state change, if any.
 *
 *  @return  The current state after any state changes, if
 *           successful.  If there were errors, STATE_NULL (which can
 *           never be changed to) will be returned, and can be trapped
 *           as an error.
 */

int
update_state (void);


/** Call the initialising function for a given state.
 *
 *  @param state  The state to attempt to initialise.
 *
 *  @return  SUCCESS if no errors occurred, FAILURE otherwise.
 */

int
init_state (state_t state);


/** Perform per-frame updates for the current state. */

void
state_frame_updates (void);


/** Call the cleanup function for a given state.
 *
 *  @param state  The state to attempt to de-initialise.
 *
 *  @return  SUCCESS if no errors occurred, FAILURE otherwise.
 */

int
cleanup_state (state_t state);


#endif /* not _STATE_H */

/* vim: set ts=2 sw=2 softtabstop=2: */