#include <SDL.h>

#include "main.h"

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
            default:
              break;
            }
          break;
        default:
          break;
        }
    }
}
