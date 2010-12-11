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

/** @file     events.h
 *  @author   Matt Windsor
 *  @brief    Prototypes and declarations for generic events system.
 */

#ifndef _EVENTS_H
#define _EVENTS_H


/* -- CONSTANTS -- */

enum
  {
    QUIT_EVENT              = (1<<0), /**< Identifier for low-level
                                         quit event. */
    MOUSE_MOTION_EVENT      = (1<<1), /**< Identifier for mouse motion
                                         event. */
    MOUSE_BUTTON_DOWN_EVENT = (1<<2), /**< Identifier for mouse
                                         button down event. */
    MOUSE_BUTTON_UP_EVENT   = (1<<3), /**< Identifier for mouse
                                         button up event. */
    ASCII_KEY_DOWN_EVENT    = (1<<4), /**< Identifier for ASCII
                                         keydown event. */
    ASCII_KEY_UP_EVENT      = (1<<5), /**< Identifier for ASCII
                                         keyup event. */
    SPECIAL_KEY_DOWN_EVENT  = (1<<6), /**< Identifier for special
                                         keydown event. */
    SPECIAL_KEY_UP_EVENT    = (1<<7), /**< Identifier for special
                                         keyup event. */

    CB_FAIL = -1,              /**< Callback ID returned for install failure. */

    LMB = (1<<0),              /**< Left mouse button. */
    MMB = (1<<1),              /**< Middle mouse button. */
    RMB = (1<<2),              /**< Right mouse button. */

    SK_ESCAPE = 0,             /**< Identifier for Escape special key. */
    SK_UP,                     /**< Identifier for up arrow special key. */
    SK_LEFT,                   /**< Identifier for left arrow special key. */
    SK_DOWN,                   /**< Identifier for right arrow special key. */
    SK_RIGHT                   /**< Identifier for down arrow special key. */
  };


/* -- TYPEDEFS -- */

typedef union event_t event_t;                /**< Input event type. */
typedef struct event_callback event_callback; /**< Input callback type. */


/* -- STRUCTURES -- */

/** The input system base structure. */

struct event_base
{
  struct event_callback *callbacks; /**< Linked list of callbacks. */
};


/** A mouse motion input event. */

struct mouse_motion_event
{
  unsigned char type;  /**< The type identifier of the input event. */
  unsigned short x;    /**< New X co-ordinate of the mouse on the
                          screen. */
  unsigned short y;    /**< New Y co-ordinate of the mouse on the
                          screen. */
  unsigned short xraw; /**< Mouse X co-ordinate without
                          transformations. */
  unsigned short yraw; /**< Mouse X co-ordinate without
                          transformations. */
  short deltax; /**< Change in X co-ordinate from previous position. */
  short deltay; /**< Change in Y co-ordinate from previous position. */
};


/** A mouse button input event. */

struct mouse_button_event
{
  unsigned char type;   /**< The type identifier of the input event. */ 
  unsigned char button; /**< Which button was pressed. */
};


/** An ASCII keyboard event. */

struct ascii_key_event
{
  unsigned char type; /**< Whether the key was pressed or released. */
  char code; /**< The ASCII code pressed. */
};


/** A special (non-ASCII) keyboard event. */

struct special_key_event
{
  unsigned char type; /**< Whether the key was pressed or released. */
  char code; /**< The special code pressed (see enum). */
};


/** An input event package. */

union event_t
{
  unsigned char type;   /**< The type identifier of the input event. */
  struct mouse_motion_event motion; /**< A mouse motion event. */
  struct mouse_button_event button; /**< A mouse button event. */
  struct ascii_key_event    ascii;  /**< An ASCII keyboard event. */
  struct special_key_event  skey;   /**< A special keyboard event. */
};


/** A callback node. */

struct event_callback
{
  void
  (*callback) (event_t *event); /**< The callback function
                                     pointer. */

  int types;                    /**< Types of event that will
                                     trigger the callback. */

  struct event_callback *next;  /**< Next callback in the linked
                                     list. */
};


/* -- PROTOTYPES -- */

/** Initialise the events system.
 *
 *  @return SUCCESS for success, FAILURE for failure.
 */

int
init_events (void);


/** Process any events queued in the events driver. */

void
process_events (void);


/** Install a callback.
 *
 *  On the event types specified in types, the callback function will
 *  be triggered with the event_t as its sole parameter. This will
 *  include the information necessary to process the event.
 *
 *  @param callback  A pointer to the callback function to install.
 *  @param types     A bitwise OR'd flag list of events that should
 *                   trigger the callback.
 *
 *  @return a pointer to the installed dm_InputCallback, used for
 *  identifying the callback later (eg for deletion).
 */

struct event_callback *
install_callback (void (*callback) (event_t *event),
                  int types);


/** Unload a callback.
 *
 *  @see install_callback()
 *
 *  @param callback  A pointer to the callback to unload.
 *
 *  @return SUCCESS if the callback was found and deleted,
 *  FAILURE otherwise.
 */

int 
unload_callback (struct event_callback *callback);


/** Release an event package to all relevant callbacks.
 *
 *  @param event  The event to release to callbacks.
 */

void
event_release (event_t *event);


/** De-initialise the events system. */

void
cleanup_events (void);


#endif /* not _EVENTS_H */
