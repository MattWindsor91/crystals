#include <SDL.h>
#include <SDL/SDL_image.h>

#include "main.h"
#include "graphics.h"

static SDL_Surface *s_screen;

int
init_graphics (void)
{
  if (SDL_Init (SDL_INIT_VIDEO) == 0)
    {
      s_screen = SDL_SetVideoMode (SCREEN_W,
                                   SCREEN_H,
                                   SCREEN_D,
                                   SDL_SWSURFACE);

      if (s_screen)
        {
          return 1;
        }
      else
        {
          fprintf (stderr, "ERROR: Couldn't get screen!\n");
          cleanup_graphics ();
        }
    } 
  else 
    {
      fprintf (stderr, "ERROR: Could not init SDL!\n");
    }

  return 0;
}

void
fill_screen (unsigned char red, 
             unsigned char green, 
             unsigned char blue)
{
  SDL_FillRect (s_screen, NULL, SDL_MapRGB (s_screen->format, red, green, blue));
}

void
draw_image (const char filename[], 
            int image_x,
            int image_y, 
            int screen_x, 
            int screen_y,
            unsigned int width, 
            unsigned int height)
{
  SDL_Surface *image;

  image = IMG_Load(filename);

  if (image)
    {
      SDL_Rect source, dest;
      
      source.x = image_x;
      source.y = image_y;
      dest.x = screen_x;
      dest.y = screen_y;

      source.w = dest.w = width;
      source.h = dest.h = height;

      SDL_BlitSurface (image, &source, s_screen, &dest); 
    }

  SDL_FreeSurface (image);
}

void
update_screen (void)
{
  SDL_Flip (s_screen);
  SDL_Delay (30);
}

void
scroll_screen (unsigned int direction)
{
  SDL_Rect source, dest;

  source.x = dest.x = source.y = dest.y = 0;

  source.w = dest.w = SCREEN_W - 1;
  source.h = dest.h = SCREEN_H - 1;

  switch (direction)
    {
    case NORTH:
      source.y = 1;
      break;
    case EAST:
      dest.x = 1;
      break;
    case SOUTH:
      dest.y = 1;
      break;
    case WEST:
      source.x = 1;
      break;
    }

  SDL_BlitSurface (s_screen, &source, s_screen, &dest);
}

void
cleanup_graphics (void)
{
  SDL_Quit ();
}

