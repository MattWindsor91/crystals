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
 * @file    src/modules/event-dummy.c
 * @author  Alexander Preisinger
 * @brief   Dummy event module.
 */

#include "../util.h"
#include "../events.h"

/* -- STATIC GLOBAL VARIABLES -- */

static void
(*sg_event_release) (event_t *event); /**< Event release function
                                         pointer. */

/* Workaround for Windows DLL symbol load failures.
 *
 * All outward-facing functions MUST be preceded with
 * EXPORT so that the DLL loader can see them.
 */

#ifdef PLATFORM_WINDOWS
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif /* PLATFORM_WINDOWS */

/* -- PROTOTYPES -- */

/** 
 * Initialise the events module. 
 */

EXPORT bool_t
init (void);


/** 
 * Terminate the events module, freeing any remaining data
 * dynamically allocated by the module. 
 */

EXPORT void
term (void);


/** 
 * Register a function for handling event releases.
 *
 * @param handle  The function to send event releases to. 
 *                This should be event.c's event_release;
 */

EXPORT void
register_release_handle (void (*handle) (event_t *event));


/** 
 * Process one frame of input.
 *
 * This function calls the platform-specific input routines to handle 
 * any pending input events, and trigger any relevant callbacks.
 *
 */

EXPORT void
process_events_internal (void);


/* -- DEFINITIONS -- */

/* Initialise the events module. */

EXPORT bool_t
init (void)
{
  return SUCCESS;
}


/* Terminate the events module, freeing any remaining data
   dynamically allocated by the module. */

EXPORT void
term (void)
{
}


/* Register a function for handling event releases. */

EXPORT void
register_release_handle (void (*handle) (event_t *event))
{
  sg_event_release = handle;
}


/* Process one frame of input. */

void
process_events_internal (void)
{
  event_t event;
  event.type = QUIT_EVENT;
  (*sg_event_release) (&event);
}

/* vim: set ts=2 sw=2 softtabstop=2 cinoptions=>4,n-2,{2,^-2,:2,=2,g0,h2,p5,t0,+2,(0,u0,w1,m1: */

