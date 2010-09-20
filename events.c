#include <SDL.h>

#include "main.h"
#include "events.h"
#include "graphics.h"
#include "mapview.h"

static unsigned char sg_held_keys[256];

void
init_events (void)
{
  memset (sg_held_keys, 0, sizeof (unsigned char) * 256);
}

void
handle_events (void)
{
  SDL_Event event;
  
  while (SDL_PollEvent (&event)) 
    {
      switch (event.type)
        {
        case SDL_QUIT:
          g_running = 0;
          break;
        case SDL_KEYDOWN:
          sg_held_keys[event.key.keysym.sym] = 1;

          switch (event.key.keysym.sym)
            {
            case SDLK_ESCAPE:
              g_running = 0;
              break;
            case SDLK_r:
              fill_screen (0, 0, 0);
              render_map (g_mapview);
              break;
            default:
              break;
            }
          break;
        case SDL_KEYUP:
          sg_held_keys[event.key.keysym.sym] = 0;
          break;
        default:
          break;
        }
    }

  if (sg_held_keys[SDLK_UP])
    scroll_map (g_mapview, NORTH);

  if (sg_held_keys[SDLK_RIGHT])
    scroll_map (g_mapview, EAST);

  if (sg_held_keys[SDLK_DOWN])
    scroll_map (g_mapview, SOUTH);

  if (sg_held_keys[SDLK_LEFT])
    scroll_map (g_mapview, WEST);
}
