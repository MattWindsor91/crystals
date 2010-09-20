#include <SDL.h>

#include "main.h"
#include "map.h"

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
          switch (event.key.keysym.sym)
            {
            case SDLK_ESCAPE:
              g_running = 0;
              break;
            case SDLK_UP:
              scroll_map (NORTH);
              break;
            case SDLK_RIGHT:
              scroll_map (EAST);
              break;
            case SDLK_DOWN:
              scroll_map (SOUTH);
              break;
            case SDLK_LEFT:
              scroll_map (WEST);
              break;
            default:
              break;
            }
          break;
        default:
          break;
        }
    }
}
