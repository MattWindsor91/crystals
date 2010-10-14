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
#include <SDL.h>
#include <SDL/SDL_image.h>

#include "../util.h" /* Constants. */

/* -- STATIC GLOBAL VARIABLES -- */

static SDL_Surface *sg_screen; /**< The main screen surface. */

static SDL_Surface *sg_shadow; /**< The shadow screen surface, used
                                  for copying a screen onto itself etc.*/

/* -- PROTOTYPES -- */

/** Initialise the module. */

void
init (void);

/** Terminate the module, freeing any remaining data dynamically
    allocated by the module. */

void
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

int
init_screen (unsigned short width,
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

void
draw_rect (short x,
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

void *
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

void
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

int
draw_image (void *image, 
            short image_x,
            short image_y,
            short screen_x,
            short screen_y,
            unsigned short width,
            unsigned short height);


/** Update the screen. */

void
update_screen (void);


/** Translate the screen by a co-ordinate pair, leaving damage.
 *
 *  @param x_offset  The X co-ordinate offset in which to scroll the 
 *                   screen.
 *
 *  @param y_offset  The Y co-ordinate offset in which to scroll the 
 *                   screen.
 */

void
scroll_screen (short x_offset, short y_offset);


/* -- DEFINITIONS -- */

/* Initialise the module. */

void
init (void)
{
  sg_screen = NULL;
}


/* Terminate the module, freeing any remaining data dynamically
   allocated by the module. */

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


/* Initialise a screen of a given width, height and depth. */

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
                                   SDL_HWSURFACE);
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


/* Draw a rectangle of colour on-screen. */

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

/* Load an image and return its data in the module's native format. */

void *
load_image_data (const char filename[])
{ 
  SDL_Surface *surf;

  surf = IMG_Load (filename);

  if (surf == NULL)
    fprintf (stderr, "GFX-SDL: Error: Couldn't load %s!\n", filename);

  return (void*) surf;
}


/* Free image data retrieved by load_image_data. */

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
scroll_screen (short x_offset, short y_offset)
{
  SDL_Rect source, dest;

  source.x = dest.x = source.y = dest.y = 0;

  source.w = dest.w = (Uint16) sg_screen->w;
  source.h = dest.h = (Uint16) sg_screen->h;

  /* Set up the offsets. */

  if (x_offset < 0)
    {
      source.x -= x_offset;
      source.w += x_offset;
    }
  else
    {
      dest.x += x_offset;
      dest.w -= x_offset;
    }

  if (y_offset < 0)
    {
      source.y -= y_offset;
      source.h += y_offset;
    }
  else
    {
      dest.y += y_offset;
      source.h -= y_offset;
    }

  SDL_BlitSurface (sg_screen, &source, sg_shadow, &dest);
  SDL_BlitSurface (sg_shadow, &dest, sg_screen, &dest);
}
