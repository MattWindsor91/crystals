/* event-sdl.c - SDL implementation of events backend. */

#include <stdio.h>
#include <SDL.h>
#include <SDL/SDL_image.h>

#include "../events.h"
#include "../graphics.h"

/** Initialise the input module. */

int
init (void);

/** De-initialise the input module. */

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
process_events (void);

/** Handle a SDL mouse motion event.
 *
 *  @param event     Event union to populate with information.
 *  @param sdlevent  SDL event to extract motion data from.
 */

static void
mouse_motion (event_t *event, SDL_Event *sdlevent);

/* Definitions follow. */

static void
(*sg_event_release) (event_t *event);

int
init (void)
{
  if (SDL_Init (SDL_INIT_VIDEO) != 0)
    {
      fprintf (stderr, "event-sdl: ERROR: Could not init SDL!\n");
      return 0;
    }

  SDL_EnableUNICODE (1);
  return 1;
}

void
term (void)
{
  SDL_Quit ();
}

void
register_release_handle (void (*handle) (event_t *event))
{
  sg_event_release = handle;
}

void
process_events (void)
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
