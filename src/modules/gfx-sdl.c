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


#include "module.h"

#include <SDL.h>
#include <SDL_image.h>

#include "gfx-module.h" /* Module header file. */

#undef G_LOG_DOMAIN
#define G_LOG_DOMAIN "gfx-sdl"

/* -- STATIC GLOBAL VARIABLES -- */

static SDL_Surface *sg_screen; /**< The main screen surface. */

static SDL_Surface *sg_shadow; /**< The screen surface everything is
                                 * drawn onto. */

static GSList *sg_blit_stack; /**< The stack of rectangles to
                                 update on the screen. */

static bool_t sg_update_full_screen; /**< If true then the entire
                                      * screen must be flipped on the
                                      * next frame.
                                      */

static void
update_rect_internal (void *rect, void *ignore);


/* -- DEFINITIONS -- */

/* Initialises the module. */

EXPORT bool_t
init (void)
{
  sg_screen = NULL;
  sg_shadow = NULL;
  sg_blit_stack = NULL;
  sg_update_full_screen = FALSE;

  return SUCCESS;
}


/* Terminates the module, freeing any remaining data dynamically
   allocated by the module. */

EXPORT void
term (void)
{
  if (sg_screen)
    {
      if (sg_shadow)
        {
          SDL_FreeSurface (sg_shadow);
        }

      if (sg_blit_stack)
        {
          g_slist_free_full (sg_blit_stack, free);
          sg_blit_stack = NULL;
        }

      SDL_Quit ();
    }
}


/* Initialises a screen of a given width, height and depth. */

EXPORT bool_t
init_screen_internal (uint16_t width, uint16_t height, uint8_t depth)
{
  if (SDL_Init (SDL_INIT_VIDEO) != 0)
    {
      g_critical ("Could not initialise SDL");
      return FAILURE;
    }


   sg_screen = SDL_SetVideoMode (width, height, depth, SDL_HWSURFACE);

   if (sg_screen == NULL)
     {
       g_critical ("Could not init screen.");
       SDL_Quit ();
       return FAILURE;
     }


   sg_shadow = SDL_DisplayFormat (sg_screen);

   if (sg_shadow == NULL)
     {
       g_critical ("Could not make shadowbuf.");
       SDL_Quit ();
       return FAILURE;
     }


  return SUCCESS;
}


/* Draws a rectangle of colour on-screen. */

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

  SDL_FillRect (sg_shadow, &rect, SDL_MapRGB (sg_screen->format,
                                              red, green, blue));

  return SUCCESS;
}


/* Loads an image and returns its data in the module's native format. */

EXPORT void *
load_image_data (const char filename[])
{
  SDL_Surface *surface;


  surface = IMG_Load (filename);

  if (surface == NULL)
    {
      g_critical ("Couldn't load %s!", filename);
    }

  return (void *) surface;
}


/* Frees image data retrieved by load_image_data. */

EXPORT bool_t
free_image_data (void *data)
{
  if (data)
    SDL_FreeSurface(data);

  return SUCCESS;
}


/* Draws a rectangular portion of an image on-screen. */

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
      g_critical ("Image data is NULL.");
      return FAILURE;
    }


  srcrect.x = image_x;
  srcrect.y = image_y;

  destrect.x = screen_x;
  destrect.y = screen_y;

  srcrect.w = destrect.w = width;
  srcrect.h = destrect.h = height;

  SDL_BlitSurface (ptex, &srcrect, sg_shadow, &destrect);

  return SUCCESS;
}


/* Adds a rectangle to the next update run. */

EXPORT void
add_update_rectangle_internal (int16_t x,
                               int16_t y,
                               uint16_t width,
                               uint16_t height)
{
  SDL_Rect *rect;

  rect = calloc (1, sizeof (SDL_Rect));
  if (rect == NULL)
    {
      g_critical ("Could not allocate an update rectangle.");
      return;
    }
  rect->x = x;
  rect->y = y;
  rect->w = width;
  rect->h = height;

  sg_blit_stack = g_slist_prepend (sg_blit_stack, rect);
}


/* Updates the screen. */

EXPORT bool_t
update_screen_internal (void)
{
  g_slist_foreach (sg_blit_stack, update_rect_internal, NULL);

  if (sg_update_full_screen)
    {
      sg_update_full_screen = FALSE;
      SDL_Flip (sg_screen);
    }
  else
    {
      /* Collate rectangles into an array and UpdateRects them. */
      SDL_Rect *rectlist;
      void *current;
      int i;
      int len;

      len = g_slist_length (sg_blit_stack);
      rectlist = calloc (len, sizeof (SDL_Rect));
      for (i = 0; i < len; i += 1)
        {
          current = g_slist_nth_data (sg_blit_stack, i);
          memcpy (&(rectlist[i]),
                  current,
                  sizeof (SDL_Rect));
        }

      SDL_UpdateRects (sg_screen, len, rectlist);

      free (rectlist);
    }

  g_slist_free_full (sg_blit_stack, free);
  sg_blit_stack = NULL;

  SDL_Delay (15); /* TODO: remove <-- */

  return SUCCESS;
}


/* Updates one rectangle. */

static void
update_rect_internal (void *rect, void *ignore)
{
  SDL_Rect *rectc = (SDL_Rect *) rect;
  SDL_BlitSurface (sg_shadow, rectc, sg_screen, rectc);

  (void) ignore;
}


/* Translates the screen by a co-ordinate pair, leaving damage. */

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
      g_critical ("Coordinate overflow when scrolling.");
      return FAILURE;
    }


  if (source_width > USHRT_MAX
      || source_height > USHRT_MAX
      || source_width < 0
      || source_height < 0)
    {
      g_critical ("Dimension overflow when scrolling.");
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
  /* The whole screen now needs updating! */
  SDL_BlitSurface (sg_shadow, NULL, sg_screen, NULL);
  sg_update_full_screen = TRUE;


  return SUCCESS;
}
