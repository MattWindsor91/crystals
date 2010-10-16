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

/** @file    modules/event-sdl.c
 *  @author  Matt Windsor
 *  @brief   SDL implementation of event system backend.
 */

#include <stdio.h>
#include <SDL.h>
#include <SDL/SDL_image.h>

#include "../util.h"
#include "../events.h"
#include "../graphics.h"

/* -- STATIC GLOBAL VARIABLES -- */

static void
(*sg_event_release) (event_t *event); /**< Event release function
                                         pointer. */


/* -- PROTOTYPES -- */

/** Initialise the events module. */

int
init (void);


/** Terminate the events module, freeing any remaining data
    dynamically allocated by the module. */

void
term (void);


/** Register a function for handling event releases.
 *
 *  @param handle  The function to send event releases to. 
 *                 This should be event.c's event_release;
 */

void
register_release_handle (void (*handle) (event_t *event));


/** Process one frame of input.
 *
 *  This function calls the platform-specific input routines to handle 
 *  any pending input events, and trigger any relevant callbacks.
 *
 */

void
process_events_internal (void);


/** Handle an SDL mouse motion event.
 *
 *  @param event     Event union to populate with information.
 *  @param sdlevent  SDL event to extract motion data from.
 */

static void
mouse_motion (event_t *event, SDL_Event *sdlevent);


/* -- DEFINITIONS -- */

/* Initialise the events module. */

int
init (void)
{
  if (SDL_Init (SDL_INIT_VIDEO) != 0)
    {
      fprintf (stderr, "event-sdl: ERROR: Could not init SDL!\n");
      return FAILURE;
    }

  SDL_EnableUNICODE (1);
  return SUCCESS;
}


/* Terminate the events module, freeing any remaining data
   dynamically allocated by the module. */

void
term (void)
{
  SDL_Quit ();
}


/* Register a function for handling event releases. */

void
register_release_handle (void (*handle) (event_t *event))
{
  sg_event_release = handle;
}


/* Process one frame of input. */

void
process_events_internal (void)
{
  SDL_Event sdlevent;
  event_t event;

  while (SDL_PollEvent (&sdlevent))
    {
      /* Null out the event. */
      event.type = 0;

      switch(sdlevent.type)
        {
        case SDL_QUIT:
          /* Quit event (eg window close attempted). */
          event.type = QUIT_EVENT;
          break;
        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP:

          /* Mouse button events.
           * This of course assumes SDL mice will only ever have 3 buttons.
           */

          if (sdlevent.button.button == SDL_BUTTON_LEFT)
            event.button.button = LMB;
          else if (sdlevent.button.button == SDL_BUTTON_MIDDLE)
            event.button.button = MMB;
          else
            event.button.button = RMB;

          if (sdlevent.type == SDL_MOUSEBUTTONDOWN)
            event.type = MOUSE_BUTTON_DOWN_EVENT;
          else
            event.type = MOUSE_BUTTON_UP_EVENT;

          break;
        case SDL_MOUSEMOTION:
          /* Mouse motion events. */
          mouse_motion (&event, &sdlevent);
          break;
        case SDL_KEYDOWN:
        case SDL_KEYUP:
          /* Keyboard events. */
      
          /* First, check for special keys. */

          /* Special key? */
          if (sdlevent.key.type == SDL_KEYDOWN)
            event.skey.type = SPECIAL_KEY_DOWN_EVENT;
          else
            event.skey.type = SPECIAL_KEY_UP_EVENT;
          
          switch (sdlevent.key.keysym.sym)
            {
            case SDLK_ESCAPE:
              event.skey.code = SK_ESCAPE;
              break;
            case SDLK_UP:
              event.skey.code = SK_UP;
              break;
            case SDLK_RIGHT:
              event.skey.code = SK_RIGHT;
              break;
            case SDLK_DOWN:
              event.skey.code = SK_DOWN;
              break;
            case SDLK_LEFT:
              event.skey.code = SK_LEFT;
              break;
            default:
              /* Not a special key after all. */
              event.type = 0;
              break;
            }
         
          /* Use SDL's unicode support to check for an ASCII key, if the 
             special key check failed. (It works!) */

          if (sdlevent.key.keysym.unicode < 0x80
              && sdlevent.key.keysym.unicode > 0
              && event.type == 0)
            {
              printf ("Key! %u\n", sdlevent.key.keysym.unicode);
              /* ASCII key */
              if (sdlevent.key.type == SDL_KEYDOWN) 
                event.ascii.type = ASCII_KEY_DOWN_EVENT;
              else
                event.ascii.type = ASCII_KEY_UP_EVENT;
           
              event.ascii.code = (char) sdlevent.key.keysym.unicode;
            }

          break;
        default:
          break;
        }

      /* If there was a proper event, release it to callbacks. */
      if (event.type != 0)
        (*sg_event_release) (&event);
    }
}


/* Handle an SDL mouse motion event. */

void
mouse_motion (event_t *event, SDL_Event *sdlevent)
{
  /* We need to check to see if the raw X and Y are within the
     translated screen boundaries. */
  unsigned short left, top, right, bottom;

  left = top = 0;
  right = SCREEN_W;  /**@todo FIXME: make this less hardcoded. */
  bottom = SCREEN_H; /**@todo FIXME: make this less hardcoded. */

  if (sdlevent->motion.x >= left && 
      sdlevent->motion.x < right &&
      sdlevent->motion.y >= top &&
      sdlevent->motion.y < bottom)
    {
      event->motion.type = MOUSE_MOTION_EVENT;
      event->motion.xraw = event->motion.x = sdlevent->motion.x;
      event->motion.yraw = event->motion.y = sdlevent->motion.y;
      event->motion.deltax = sdlevent->motion.xrel;
      event->motion.deltay = sdlevent->motion.yrel;
    }
}
