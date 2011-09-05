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
 * @file    modules/gfx-sdl.c
 * @author  Matt Windsor
 * @brief   SDL implementation of graphics backend.
 */


#include <stdio.h>
#include <limits.h>
#include <SDL.h>
#include <SDL_image.h>

#include "gfx-module.h" /* Module header file. */
#include "../util.h"    /* Constants. */
#include "../types.h"    /* Type definitions. */


/* -- STATIC GLOBAL VARIABLES -- */

static SDL_Surface *sg_screen; /**< The main screen surface. */

static SDL_Surface *sg_shadow; /**< The shadow screen surface, used
                                  for copying a screen onto itself etc.*/


/* -- DEFINITIONS -- */

/* Initialise the module. */

EXPORT bool_t
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

EXPORT bool_t
init_screen_internal (uint16_t width, uint16_t height, uint8_t depth)
{
  if (SDL_Init (SDL_INIT_VIDEO) != 0)
    {
      fprintf (stderr, "GFX-SDL: Error: could not initialise SDL.\n");
      return FAILURE;
    }


   sg_screen = SDL_SetVideoMode (width, height, depth, SDL_HWSURFACE);

   if (sg_screen == NULL)
     {
       fprintf (stderr, "GFX-SDL: Error: could not init screen.\n");
       SDL_Quit ();
       return FAILURE;
     }


   sg_shadow = SDL_DisplayFormat (sg_screen);

   if (sg_shadow == NULL)
     {
       fprintf (stderr, "GFX-SDL: Error: could not make shadowbuf.\n");
       SDL_Quit ();
       return FAILURE;
     }


  return SUCCESS;
}


/* Draw a rectangle of colour on-screen. */

EXPORT bool_t
draw_rect_internal (int16_t x,
                    int16_t y,
                    uint16_t width,
                    uint16_t height,
                    uint8_t red,
                    uint8_t green,
                    uint8_t blue)
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
  SDL_Surface *surface;


  surface = IMG_Load (filename);

  if (surface == NULL)
    fprintf (stderr, "GFX-SDL: Error: Couldn't load %s!\n", filename);

  return (void*) surface;
}


/* Free image data retrieved by load_image_data. */

EXPORT bool_t
free_image_data (void *data)
{
  if (data)
    SDL_FreeSurface(data);

  return SUCCESS;
}


/* Draw a rectangular portion of an image on-screen. */

EXPORT bool_t
draw_image_internal (void *image,
                     int16_t image_x,
                     int16_t image_y,
                     int16_t screen_x,
                     int16_t screen_y,
                     uint16_t width,
                     uint16_t height)
{
  SDL_Rect srcrect, destrect;
  SDL_Surface *ptex;


  ptex = (SDL_Surface*) image;

  if (ptex == NULL)
    {
      fprintf (stderr, "GFX-SDL: Error: Image data is NULL.\n");
      return FAILURE;
    }


  srcrect.x = image_x;
  srcrect.y = image_y;

  destrect.x = screen_x;
  destrect.y = screen_y;

  srcrect.w = destrect.w = width;
  srcrect.h = destrect.h = height;

  SDL_BlitSurface (ptex, &srcrect, sg_screen, &destrect);

  return SUCCESS;
}


/* Update the screen. */

EXPORT bool_t
update_screen_internal (void)
{
  SDL_Flip (sg_screen);
  SDL_Delay (15);

  return SUCCESS;
}


/* Translate the screen by a co-ordinate pair, leaving damage. */

EXPORT bool_t
scroll_screen_internal (int16_t x_offset, int16_t y_offset)
{
  SDL_Rect source, dest;


  /* These are all temporary variables, which are 
   * used for boundary-checking before populating the SDL rects.
   */

  int32_t source_x;
  int32_t source_y;
  int32_t source_width;
  int32_t source_height;

  int32_t dest_x;
  int32_t dest_y;

  source_x = dest_x = source_y = dest_y = 0;

  source_width = dest.w = (Uint16) sg_screen->w;
  source_height = dest.h = (Uint16) sg_screen->h;

  /* Set up the offsets. */

  if (x_offset < 0)
    {
      source_x -= x_offset;
      source_width += x_offset;
    }
  else
    {
      dest_x += x_offset;
      source_width -= x_offset;
    }

  if (y_offset < 0)
    {
      source_y -= y_offset;
      source_height += y_offset;
    }
  else
    {
      dest_y += y_offset;
      source_height -= y_offset;
    }


  /* Now check for overflows. */

  if (source_x > SHRT_MAX
      || source_y > SHRT_MAX
      || dest_x > SHRT_MAX
      || dest_y > SHRT_MAX
      || source_x < SHRT_MIN
      || source_y < SHRT_MIN
      || dest_x < SHRT_MIN
      || dest_y < SHRT_MIN)
    {
      fprintf (stderr, "GFX-SDL: Coordinate overflow when scrolling.\n");
      return FAILURE;
    }


  if (source_width > USHRT_MAX
      || source_height > USHRT_MAX
      || source_width < 0
      || source_height < 0)
    {
      fprintf (stderr, "GFX-SDL: Dimension overflow when scrolling.\n");
      return FAILURE;
    }


  /* Convert down. */

  source.x = (int16_t) source_x;
  source.y = (int16_t) source_y;
  dest.x   = (int16_t) dest_x;
  dest.y   = (int16_t) dest_y;
  source.w = (uint16_t) source_width;
  source.h = (uint16_t) source_height;


  SDL_BlitSurface (sg_screen, &source, sg_shadow, &dest);
  SDL_BlitSurface (sg_shadow, &dest, sg_screen, &dest);


  return SUCCESS;
}
