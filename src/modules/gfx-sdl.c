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

/** @file    modules/gfx-sdl.c
 *  @author  Matt Windsor
 *  @brief   SDL implementation of graphics backend.
 */

#include <stdio.h>
#include <limits.h>
#include <SDL.h>
#include <SDL_image.h>

#include "../util.h" /* Constants. */

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

/* -- STATIC GLOBAL VARIABLES -- */

static SDL_Surface *sg_screen; /**< The main screen surface. */

static SDL_Surface *sg_shadow; /**< The shadow screen surface, used
                                  for copying a screen onto itself etc.*/

/* -- PROTOTYPES -- */

/** Initialise the module. */

EXPORT int
init (void);

/** Terminate the module, freeing any remaining data dynamically
    allocated by the module. */

EXPORT void
term (void);

/** Initialise a screen of a given width, height and depth.
 *
 *  The screen's data will automatically be deleted, if not sooner,
 *  when the module is unloaded (ie, via the module's term
 *  function).
 *
 *  @todo Fullscreen option?
 *
 *  @param width   Width of the screen, in pixels.
 *  @param height  Height of the screen, in pixels.
 *  @param depth   Colour depth of the screen, in bits per pixel.
 *
 *  @return  SUCCESS for success; FAILURE otherwise.
 */

EXPORT int
init_screen_internal (unsigned short width,
                      unsigned short height,
                      unsigned char depth);

/** Draw a rectangle of colour on-screen.
 *
 *  Depending on the graphics module, the colour displayed on screen
 *  may not exactly match the desired colour.
 *
 *  @param x       X co-ordinate of the left edge of the rectangle.
 *
 *  @param y       Y co-ordinate of the top edge of the rectangle.
 *
 *  @param width   The width of the rectangle, in pixels.
 *
 *  @param height  The height of the rectangle, in pixels.
 *
 *  @param red     The red component of the fill colour (0-255).
 *
 *  @param green   The green component of the fill colour (0-255).
 *
 *  @param blue    The blue component of the fill colour (0-255).
 */

EXPORT int
draw_rect_internal (short x,
                    short y, 
                    unsigned short width,
                    unsigned short height, 
                    unsigned char red,
                    unsigned char green,
                    unsigned char blue);

/** Load an image and return its data in the module's native
 *  format.
 *
 *  As the exact format returned varies from module to module, you
 *  will likely only want to use this function through the graphics
 *  subsystem's wrapper function, load_image, which also stores the
 *  data into a cache.
 *
 *  @param filename  The path to the file to load.
 *
 *  @return  a pointer to a memory location containing image data
 *  which can eventually be passed to the module's draw_image
 *  function.
 */

EXPORT void *
load_image_data (const char filename[]);

/** Free image data retrieved by load_image_data.
 *
 *  Since the nature of the image data in question varies from
 *  module to module, simply freeing the data directly may not be
 *  sufficient to unload the image from memory, hence the existence
 *  of this function.
 *
 *  @param data  A pointer to a memory location containing image
 *               data (in the module's native format) to be freed.
 */

EXPORT int
free_image_data (void *data);


/** Draw a rectangular portion of an image on-screen.
 *
 *  This should not be called directly, but instead accessed through
 *  the graphics subsystem's draw_image function (see graphics.h).
 *
 *  @param image     The image data, in the graphics module-specific
 *                   format returned by load_image_data.
 *
 *  @param image_x   The X-coordinate of the left edge of the
 *                   on-image rectangle to display.
 *
 *  @param image_y   The Y-coordinate of the top edge of the
 *                   on-image rectangle to display.
 *
 *  @param screen_x  The X-coordinate of the left edge of the
 *                   on-screen rectangle to place the image in.
 *
 *  @param screen_y  The Y-coordinate of the top edge of the
 *                   on-screen rectangle to place the image in.
 *
 *  @param width     The width of the rectangle.
 *
 *  @param height    The height of the rectangle.
 *
 *  @return  SUCCESS for success, FAILURE otherwise. In most
 *           cases, a failure will simply cause the image to not appear.
 */

EXPORT int
draw_image_internal (void *image, 
                     short image_x,
                     short image_y,
                     short screen_x,
                     short screen_y,
                     unsigned short width,
                     unsigned short height);


/** Update the screen. */

EXPORT int
update_screen_internal (void);


/** Translate the screen by a co-ordinate pair, leaving damage.
 *
 *  @param x_offset  The X co-ordinate offset in which to scroll the 
 *                   screen.
 *
 *  @param y_offset  The Y co-ordinate offset in which to scroll the 
 *                   screen.
 */

EXPORT int
scroll_screen_internal (short x_offset, short y_offset);


/* -- DEFINITIONS -- */

/* Initialise the module. */

EXPORT int
init (void)
{
  sg_screen = NULL;

  return SUCCESS;
}


/* Terminate the module, freeing any remaining data dynamically
   allocated by the module. */

EXPORT void
term (void)
{
  if (sg_screen)
    {
      if (sg_shadow)
        SDL_FreeSurface (sg_shadow);

      SDL_Quit ();
    }
}


/* Initialise a screen of a given width, height and depth. */

EXPORT int
init_screen_internal (unsigned short width,
                      unsigned short height,
                      unsigned char depth)
{
  if (SDL_Init (SDL_INIT_VIDEO) == 0)
    {
      sg_screen = SDL_SetVideoMode (width,
                                   height,
                                   depth,
                                   SDL_HWSURFACE);
      if (sg_screen)
        {
          sg_shadow = SDL_DisplayFormat (sg_screen);

          if (sg_shadow)
            return SUCCESS;
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

  return FAILURE;
}


/* Draw a rectangle of colour on-screen. */

EXPORT int
draw_rect_internal (short x,
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

  return SUCCESS;
}

/* Load an image and return its data in the module's native format. */

EXPORT void *
load_image_data (const char filename[])
{ 
  SDL_Surface *surf;

  surf = IMG_Load (filename);

  if (surf == NULL)
    fprintf (stderr, "GFX-SDL: Error: Couldn't load %s!\n", filename);

  return (void*) surf;
}


/* Free image data retrieved by load_image_data. */

EXPORT int
free_image_data (void *data)
{
  if (data)
    SDL_FreeSurface(data);

  return SUCCESS;
}


/* Draw a rectangular portion of an image on-screen. */

EXPORT int
draw_image_internal (void *image,
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
      return SUCCESS;
    }
  else
    return FAILURE;
}


/* Update the screen. */

EXPORT int
update_screen_internal (void)
{
  SDL_Flip (sg_screen);
  SDL_Delay (15);

  return SUCCESS;
}


/* Translate the screen by a co-ordinate pair, leaving damage. */

EXPORT int
scroll_screen_internal (short x_offset, short y_offset)
{
  SDL_Rect source, dest;

  /* These are all temporary variables, which are 
   * used for boundary-checking before populating the SDL rects.
   */

  int sx;
  int sy;
  int sw;
  int sh;
  int dx;
  int dy;

  sx = dx = sy = dy = 0;

  sw = dest.w = (Uint16) sg_screen->w;
  sh = dest.h = (Uint16) sg_screen->h;

  /* Set up the offsets. */

  if (x_offset < 0)
    {
      sx -= x_offset;
      sw += x_offset;
    }
  else
    {
      dx += x_offset;
      sw -= x_offset;
    }

  if (y_offset < 0)
    {
      sy -= y_offset;
      sh += y_offset;
    }
  else
    {
      dy += y_offset;
      sh -= y_offset;
    }

  /* Now check for overflows. */

  if (sx > SHRT_MAX
      || sy > SHRT_MAX
      || dx > SHRT_MAX
      || dy > SHRT_MAX
      || sx < SHRT_MIN
      || sy < SHRT_MIN
      || dx < SHRT_MIN
      || dy < SHRT_MIN)
    {
      fprintf (stderr, "GFX-SDL: Coordinate overflow when scrolling.\n");
      return FAILURE;
    }

  if (sw > USHRT_MAX
      || sh > USHRT_MAX
      || sw < 0
      || sh < 0)
    {
      fprintf (stderr, "GFX-SDL: Dimension overflow when scrolling.\n");
      return FAILURE;
    }

  /* Convert down. */

  source.x = (short) sx;
  source.y = (short) sy;
  dest.x = (short) dx;
  dest.y = (short) dy;
  source.w = (unsigned short) sw;
  source.h = (unsigned short) sh;

  SDL_BlitSurface (sg_screen, &source, sg_shadow, &dest);
  SDL_BlitSurface (sg_shadow, &dest, sg_screen, &dest);

  return SUCCESS;
}
