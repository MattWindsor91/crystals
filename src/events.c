/*
 * Crystals (working title)
 *
 * Copyright (c) 2010 Matt Windsor, Michael Walker and Alexander
 * Preisinger.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following
 * disclaimer in the documentation and/or other materials provided
 * with the distribution.
 *
 * * The names of contributors may not be used to endorse or promote
 * products derived from this software without specific prior
 * written permission.
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
 * @file   src/events.c
 * @author Matt Windsor
 * @brief  Generic portion of events system.
 *
 * The events system is based on the concept of callbacks.  Other
 * parts of the code register their functions as callbacks attached
 * to event types, and will have them triggered as those events are
 * detected and handled by the drivers.
 */


#include "crystals.h"


/* -- STATIC GLOBAL VARIABLES -- */

static struct event_base *sg_event_base; /**< Event base. */


/* -- STATIC DECLARATIONS -- */

/**
 * Release an event package to one callback, providing it is of the
 * correct type.
 *
 * @param callback  Pointer to the callback.
 * @param event     Pointer to the event package.
 */

static void
event_release_callback (gpointer callback, gpointer event);


/* -- DEFINITIONS -- */

void
init_events (void)
{
  if (load_module_event (cfg_get_str ("modules", "event_module", g_config), &g_modules) == FAILURE)
    {
      fatal ("EVENTS - init_events - Could not load events module.");
    }

  (*g_modules.event.register_release_handle) (event_release);

  sg_event_base = xcalloc (1, sizeof (struct event_base));
}


/* Process any events queued in the events driver. */

void
process_events (void)
{
  (*g_modules.event.process_events_internal) ();
}


/* Install a callback. */

event_callback_t *
install_callback (void (*function) (event_t *event), event_type_t types)
{   
  event_callback_t *callback = xcalloc (1, sizeof (event_callback_t));

  callback->function = function;
  callback->types = types;
  callback->next = NULL;

  /* Link to list */
  sg_event_base->callbacks = g_slist_prepend (sg_event_base->callbacks,
                                              callback);

  return callback;
}


/* Unload a callback. */

void
unload_callback (event_callback_t *callback)
{
  g_assert (callback != NULL);

  sg_event_base->callbacks = g_slist_remove (sg_event_base->callbacks,
                                             callback);
}


/* Release an event package to all relevant installed callbacks. */

void
event_release (event_t *event)
{
  g_slist_foreach (sg_event_base->callbacks,
                   event_release_callback,
                   event);
}


/* Release an event package to one callback, providing it is of the
 * correct type.
 */

static void
event_release_callback (gpointer callback, gpointer event)
{
  event_callback_t *callbackc = (event_callback_t *) callback;
  event_t *eventc = (event_t *) event;

  if (callbackc->types & eventc->type)
    {
        callbackc->function (eventc);
    }
}


/* De-initialise the events system. */

void
cleanup_events (void)
{
  if (sg_event_base)
    {
      g_slist_free_full (sg_event_base->callbacks, free);
      free (sg_event_base);
      sg_event_base = NULL;
    }
}
