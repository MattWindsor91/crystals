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


#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "main.h"   /* g_config */
#include "types.h"  /* Integer types */
#include "parser.h"
#include "events.h"
#include "module.h"
#include "util.h"


/* -- STATIC GLOBAL VARIABLES -- */

static struct event_base *sg_event_base; /**< Event base. */


/* -- DEFINITIONS -- */

bool_t
init_events (void)
{
  if (load_module_event (cfg_get_str ("modules", "event_module", g_config), &g_modules) == FAILURE)
    {
      g_critical ("EVENTS - init_events - Could not load events module.");
      return FAILURE;
    }

  (*g_modules.event.register_release_handle) (event_release);

  sg_event_base = malloc (sizeof (struct event_base));

  if (sg_event_base == NULL)
    {
      g_critical ("EVENTS - init_events - Could not allocate events base.");
      return FAILURE;
    }

  sg_event_base->callbacks = NULL;

  return SUCCESS;
}


/* Process any events queued in the events driver. */

void
process_events (void)
{
  (*g_modules.event.process_events_internal) ();
}


/* Install a callback. */

event_callback_t *
install_callback (void (*callback) (event_t *event), event_type_t types)
{   
  event_callback_t *pnew;
  event_callback_t **p;
  
  pnew = malloc (sizeof (event_callback_t));

  if (pnew == NULL)
    {
      g_critical ("EVENTS - install_callback - Could not allocate callback node.");
      return NULL;
    }


  pnew->callback = callback;
  pnew->types = types;
  pnew->next = NULL;

  /* Link to list */

  for (p = &(sg_event_base->callbacks); *p != NULL; p = &(*p)->next)
    ; /* Do nothing until *p is NULL. */

  pnew->next = *p;
  *p = pnew;

  return pnew;
}


/* Unload a callback. */

bool_t
unload_callback (event_callback_t *callback)
{
  event_callback_t **p;


  /* Sanity checking. */

  if (callback == NULL)
    {
      g_critical ("EVENTS - unload_callback - Given a NULL callback to delete.");
      return FAILURE;
    }

  /* End sanity checking. */


  for (p = &(sg_event_base->callbacks); *p != NULL; p = &(*p)->next)
    {
      if (*p == callback)
        {
          /* Replace callback with the next entry then delete it. */

          *p = (*p)->next;
          free (callback);

          return SUCCESS;
        }
    }


  /* No matches found. */

  return FAILURE;
}


/* Release an event package to all relevant installed callbacks. */

void
event_release (event_t *event)
{
  event_callback_t *p;

  for (p = sg_event_base->callbacks; p != NULL; p = p->next)
    {
      /* Trigger all callbacks with the relevant type. */

      if (p->types & event->type)
        p->callback (event);
    }
}


/* De-initialise the events system. */

void
cleanup_events (void)
{
  if (sg_event_base)
    {
      free (sg_event_base);
      sg_event_base = NULL;
    }
}
