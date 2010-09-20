/* test.c - a simple module to test the dynamic module-loader */

#include <stdio.h>
#include <SDL.h>
#include <SDL/SDL_image.h>

/* TODO: have these and their main.h counterparts moved to one code file. */

enum
  {
    TRUE = 1,
    FALSE = 0,
    NORTH = 0,
    EAST,
    SOUTH,
    WEST
  };

static SDL_Surface *sg_screen;

/* We use a shadow buffer for things like copying the screen onto itself. */

static SDL_Surface *sg_shadow;

/* Prototypes - we need to decide whether to use a separate header file for modules or not.
   Perhaps for multi-file modules. */

void
init (void);

void
term (void);

int
init_screen (unsigned short width,
             unsigned short height,
             unsigned char depth);

void
draw_rect (short x,
           short y, 
           unsigned short width,
           unsigned short height, 
           unsigned char red,
           unsigned char green,
           unsigned char blue);

void *
load_image_data (const char filename[]);

void
free_image_data (void *data);

int
draw_image (void *image, 
            short image_x,
            short image_y,
            short screen_x,
            short screen_y,
            unsigned short width,
            unsigned short height);

void
update_screen (void);

void
scroll_screen (unsigned int direction);

/* Definitions follow. */


void
init (void)
{
  sg_screen = NULL;
}

void
term (void)
{
  if (sg_screen)
    {
      if (sg_shadow)
        SDL_FreeSurface (sg_shadow);

      SDL_Quit ();
    }
}

int
init_screen (unsigned short width,
             unsigned short height,
             unsigned char depth)
{
  if (SDL_Init (SDL_INIT_VIDEO) == 0)
    {
      sg_screen = SDL_SetVideoMode (width,
                                   height,
                                   depth,
                                   SDL_SWSURFACE);
      if (sg_screen)
        {
          sg_shadow = SDL_DisplayFormat (sg_screen);

          if (sg_shadow)
            return TRUE;
          else
            {
              fprintf (stderr, "GFX-SDL: Error: could not make shadowbuf.\n");
              SDL_Quit ();
            }
        }
      else
        {
          fprintf (stderr, "GFX-SDL: Error: could not init screen.\n");
          SDL_Quit ();
        }
    }
  else
    fprintf (stderr, "GFX-SDL: Error: could not init SDL.\n");

  return FALSE;
}

void
draw_rect (short x,
           short y, 
           unsigned short width,
           unsigned short height, 
           unsigned char red,
           unsigned char green,
           unsigned char blue)
{
  SDL_Rect rect;

  rect.x = x;
  rect.y = y;
  rect.w = width;
  rect.h = height;

  SDL_FillRect (sg_screen, &rect, SDL_MapRGB (sg_screen->format,
                                             red, green, blue));
}

void *
load_image_data (const char filename[])
{ 
  SDL_Surface *surf;

  surf = IMG_Load (filename);

  if (surf == NULL)
    fprintf (stderr, "GFX-SDL: Error: Couldn't load %s!\n", filename);

  return (void*) surf;
}

void
free_image_data (void *data)
{
  if (data)
    SDL_FreeSurface(data);
}

int
draw_image (void *image,
            short image_x,
            short image_y,
            short screen_x,
            short screen_y,
            unsigned short width,
            unsigned short height)
{
  SDL_Rect srcrect, destrect;
  SDL_Surface *ptex;

  ptex = (SDL_Surface*) image;

  srcrect.x = image_x;
  srcrect.y = image_y;

  destrect.x = screen_x;
  destrect.y = screen_y;

  srcrect.w = destrect.w = width;
  srcrect.h = destrect.h = height;

  if (ptex)
    {
      SDL_BlitSurface (ptex, &srcrect, sg_screen, &destrect);
      return 1;
    }
  else
    return 0;
}

void
update_screen (void)
{
  SDL_Flip (sg_screen);
  SDL_Delay (15);
}

void
scroll_screen (unsigned int direction)
{
  SDL_Rect source, dest;

  source.x = dest.x = source.y = dest.y = 0;

  source.w = dest.w = (Uint16) sg_screen->w;
  source.h = dest.h = (Uint16) sg_screen->h;

  switch (direction)
    {
    case NORTH:
      source.y = 1;
      source.h -= 1;
      break;
    case EAST:
      dest.x = 1;
      source.w -= 1;
      break;
    case SOUTH:
      dest.y = 1;
      source.h -= 1;
      break;
    case WEST:
      source.x = 1;
      source.w -= 1;
      break;
    }

  SDL_BlitSurface (sg_screen, &source, sg_shadow, &dest);
  SDL_BlitSurface (sg_shadow, &dest, sg_screen, &dest);
}
