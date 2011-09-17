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
 * @file     events.h
 * @author   Matt Windsor
 * @brief    Prototypes and declarations for generic events system.
 */

#ifndef _EVENTS_H
#define _EVENTS_H

/* -- CONSTANTS -- */

enum
  {
    /* -- Event type identifiers (for use with event_type_t) -- */

    QUIT_EVENT              = (1<<0),  /**< ID for low-level quit event.    */
    MOUSE_MOTION_EVENT      = (1<<1),  /**< ID for mouse motion event.      */
    MOUSE_BUTTON_DOWN_EVENT = (1<<2),  /**< ID for mouse button down event. */
    MOUSE_BUTTON_UP_EVENT   = (1<<3),  /**< ID for mouse button up event.   */
    ASCII_KEY_DOWN_EVENT    = (1<<4),  /**< ID for ASCII keydown event.     */
    ASCII_KEY_UP_EVENT      = (1<<5),  /**< ID for ASCII keyup event.       */
    SPECIAL_KEY_DOWN_EVENT  = (1<<6),  /**< ID for special keydown event.   */
    SPECIAL_KEY_UP_EVENT    = (1<<7),  /**< ID for special keyup event.     */


    /* -- Mouse buttons (for use with event_button_t) -- */

    LMB                     = (1<<0),  /**< Left mouse button.              */
    MMB                     = (1<<1),  /**< Middle mouse button.            */
    RMB                     = (1<<2),  /**< Right mouse button.             */


    /* -- Special keys (for use with event_key_t) -- */

    SK_ESCAPE               = 0,       /**< ID for Escape special key.      */
    SK_UP                   = 1,       /**< ID for up arrow special key.    */
    SK_LEFT                 = 2,       /**< ID for left arrow special key.  */
    SK_DOWN                 = 3,       /**< ID for right arrow special key. */
    SK_RIGHT                = 4,       /**< ID for down arrow special key.  */


    /* -- Miscellaneous -- */

    CB_FAIL                 = -1       /**<
                                        * Callback ID returned for install
                                        * failure.
                                        */
  };


/* -- TYPEDEFS -- */

typedef uint8_t event_type_t;    /**< Type for event type identifier. */
typedef uint8_t event_button_t;  /**< Type for event mouse button.    */
typedef uint8_t event_key_t;     /**< Type for special key code.      */

/* -- STRUCTURES -- */

/** The input system base structure. */

typedef struct event_base
{
  GSList *callbacks; /**< Linked list of callbacks. */
} event_base_t;


/** A mouse motion input event. */

typedef struct mouse_motion_event
{
  event_type_t type;  /**< The type identifier of the input event. */

  uint16_t x;         /**<
                       * New X co-ordinate of the mouse cursor on the
                       * screen.
                       */

  uint16_t y;         /**<
                       * New Y co-ordinate of the mouse cursor on the
                       * screen.
                       */

  int16_t deltax;     /**<
                       * Change in X co-ordinate from the previous
                       * location of the mouse cursor.
                       */

  int16_t deltay;     /**<
                       * Change in Y co-ordinate from the previous
                       * location of the mouse cursor.
                       */

} mouse_motion_event_t;


/** A mouse button input event. */

typedef struct mouse_button_event
{
  event_type_t type;      /**< The type identifier of the input event. */
  event_button_t button;  /**< Which button was pressed. */
} mouse_button_event_t;


/**
 * An ASCII keyboard event.
 *
 * This structure is used both for key-down (ASCII_KEY_DOWN_EVENT)
 * and key-up (ASCII_KEY_UP_EVENT) events.
 */

typedef struct ascii_key_event
{
  event_type_t type;   /**< The type identifier of the input event. */
  char code;           /**< The ASCII code pressed. */
} ascii_key_event_t;


/**
 * A special (non-ASCII) keyboard event.
 *
 * This structure is used both for key-down (SPECIAL_KEY_DOWN_EVENT)
 * and key-up (SPECIAL_KEY_UP_EVENT) events.
 */

typedef struct special_key_event
{
  event_type_t type;  /**< The type identifier. */
  event_key_t code;   /**< The special code pressed (see enum). */
} special_key_event_t;


/** An input event package. */

typedef union event
{
  event_type_t type;                /**< The type identifier of the input event. */
  struct mouse_motion_event motion; /**< A mouse motion event. */
  struct mouse_button_event button; /**< A mouse button event. */
  struct ascii_key_event    ascii;  /**< An ASCII keyboard event. */
  struct special_key_event  skey;   /**< A special keyboard event. */
} event_t;


/** A callback node. */

typedef struct event_callback
{
  void
  (*function) (event_t *event); /**< The callback function pointer. */

  event_type_t types;           /**<
                                 * Bitfield of types of event that will
                                 * trigger the callback.
                                 */

  struct event_callback *next;  /**< Next callback in the linked list. */
} event_callback_t;


/* -- DECLARATIONS -- */

/**
 * Initialise the events system.
 *
 * @return SUCCESS for success; FAILURE otherwise.
 */

void
init_events (void);


/**
 * Process any events queued in the events driver.
 *
 * @param useconds  Elapsed microseconds.
 */
void process_events (uint32_t useconds);


/**
 * Install a callback.
 *
 * On the event types specified in types, the callback function will
 * be triggered with the event_t as its sole parameter. This will
 * include the information necessary to process the event.
 *
 * @param callback  A pointer to the callback function to install.
 * @param types     A bitwise OR'd flag list of events that should
 *                  trigger the callback.
 *
 * @return          a pointer to the installed callback, used for
 *                  identifying the callback later (eg for deletion).
 */

event_callback_t *
install_callback (void (*callback) (event_t *event),
                  event_type_t types);


/**
 * Unload a callback.
 *
 * @see             install_callback()
 *
 * @param callback  A pointer to the callback to unload.
 */

void
unload_callback (event_callback_t *callback);


/**
 * Release an event package to all relevant callbacks.
 *
 * @param event  The event to release to callbacks.
 */

void
event_release (event_t *event);


/** De-initialise the events system. */

void
cleanup_events (void);


#endif /* not _EVENTS_H */
