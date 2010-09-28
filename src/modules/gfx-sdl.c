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
 *  @brief   SDL implementation of graphics backend..
 */

#include <stdio.h>
#include <SDL.h>
#include <SDL/SDL_image.h>

#include "../util.h" /* Constants. */

/* -- STATIC GLOBAL VARIABLES -- */

static SDL_Surface *sg_screen; /**< The main screen surface. */

/* We use a shadow buffer for things like copying the screen onto itself. */

static SDL_Surface *sg_shadow; /**< The shadow screen surface. */

/* -- PROTOTYPES -- */

void
init (void);

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
