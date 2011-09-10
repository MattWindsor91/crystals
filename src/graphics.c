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
 * @file    graphics.c
 * @author  Matt Windsor
 * @brief   Generic graphics system.
 *
 * @todo FIXME: Add graphics location to graphics paths at run-time
 *              instead of hard-coding it.
 */

#include "crystals.h"


/* -- CONSTANTS -- */

const char DEFGFXPATH[] = "gfx/";
const char FONT_FILENAME[] = "font.png";

const uint16_t FONT_W = 10;
const uint16_t FONT_H = 10;


/* -- STATIC GLOBAL VARIABLES -- */

static GHashTable *sg_images = NULL;


/* -- DEFINITIONS -- */

/* Initialise the graphics subsystem. */

void
init_graphics (void)
{
  if (load_module_gfx (cfg_get_str ("modules", "graphics_module", g_config), &g_modules) == FAILURE)
    {
      fatal ("GRAPHICS - init_graphics - Could not load graphics module.");
    }


  /* Call on the graphics backend to create a screen. */

  if ((*g_modules.gfx.init_screen_internal)
      (SCREEN_W, SCREEN_H, SCREEN_D) == FAILURE)
    {
      fatal ("GRAPHICS - init_graphics - Could not init screen.");
    }


  /* Initialise the image list. */
 
  sg_images = g_hash_table_new_full(g_str_hash,
                                    g_str_equal,
                                    free,
                                    free_image);
}


/* Given a relative path to an image file, append the graphics root
   path to it and store it in the given pointer. */

char *
get_absolute_path (const char path[])
{
  char *root_path;
  char *absolute_path;

  /* Get the root path from the configuration, if it exists. */

  root_path = cfg_get_str ("gfx", "graphics_path", g_config);

  if (root_path == NULL)
    {
      /* Using default path (this is a fallback). */

      error ("GRAPHICS - get_absolute_path - No graphics_path in config.");
      error ("Using default path instead.");

      absolute_path = xcalloc (strlen (path) + strlen (DEFGFXPATH) + 1, 
                               sizeof (char));

      strncat (absolute_path, DEFGFXPATH, strlen (DEFGFXPATH));
    }
  else
    {
      /* Using configuration path. */

      absolute_path = xcalloc (strlen (path) + strlen (root_path) + 1, 
                               sizeof (char));

      strncat (absolute_path, root_path, strlen (root_path));
    }

  strncat (absolute_path, path, strlen (path));

  return absolute_path;
}


/* Write a string on the screen, using the standard font. */

void
write_string (int16_t x, int16_t y,
              uint16_t box_width, alignment_t alignment,
              const char string[])
{
  size_t slength;    /* Length of the string, in characters. */
  int16_t x1;
  uint16_t length;   /* Length of the string on-screen, in pixels. */
  char chr;
  uint16_t i;

  slength = strlen (string);
  length = ulong_to_uint16 (FONT_W * slength);

  /* Use the given alignment to position text. */
  switch (alignment)
    {
    default:
    case ALIGN_LEFT:
      x1 = x;
      break;
    case ALIGN_RIGHT:
      x1 = ulong_to_int16 (box_width - slength);
      break;
    case ALIGN_CENTRE:
      {
        uint16_t midpoint = long_to_uint16 (x + (box_width / 2));
        x1 = long_to_int16 (midpoint - (length / 2));
      }
      break;
    }


  /* Draw each character using the font image. */
  for (i = 0; i < slength; i++)
    {
      chr = string[i];
      draw_image (FONT_FILENAME, 
                  long_to_int16 ((chr % 16) * FONT_W),
                  long_to_int16 (((chr - (chr % 16))/16) * FONT_H),
                  long_to_int16 (x1),
                  long_to_int16 (y),
                  FONT_W,
                  FONT_H);
      
      x1 = long_to_int16 (x1 + FONT_W);
    }  


  /* Instruct the current state to update the screen. */

  add_update_rectangle (x, y, length, FONT_H);
  state_handle_dirty_rect (x, y, length, FONT_H);
}


/* Update the screen. */

bool_t
update_screen (void)
{
  return (*g_modules.gfx.update_screen_internal) ();
}


/* Fill the screen with the given colour. */

bool_t
fill_screen (uint8_t red, uint8_t green, uint8_t blue)
{
  return (*g_modules.gfx.draw_rect_internal)
    (0, 0, SCREEN_W, SCREEN_H, red, green, blue);
}


/* Translate the screen by a co-ordinate pair, leaving damage. */

bool_t
scroll_screen (int16_t x_offset, int16_t y_offset)
{
  return (*g_modules.gfx.scroll_screen_internal)
    (x_offset, y_offset);
}


/* Load an image. */

void *
load_image (const char filename[])
{
  void *get_try;
  image_t *data;
  char *path;

  g_assert (filename != NULL);

  /* Try look image up in cache. */
  get_try = g_hash_table_lookup (sg_images, filename);
  if (get_try != NULL)
    {
      return get_try;
    }

  /* Cache miss - load image from disk. */
  path = get_absolute_path (filename);

  data = (*g_modules.gfx.load_image_data) (path);
  free (path);

  if (data == NULL)
    {
      fatal ("GFX - load_image - Couldn't load image data for %s.", 
             filename);
      return NULL;
    }

  g_hash_table_insert(sg_images, g_strdup(filename), data);
  return data;
}


/* Free image data. */

void
free_image (image_t *image)
{
  g_assert (image != NULL);

  (*g_modules.gfx.free_image_data) (image);
}



/* Draw a rectangular portion of an image on-screen. */

bool_t
draw_image (const char filename[], int16_t image_x, int16_t image_y,
            int16_t screen_x, int16_t screen_y, uint16_t width,
            uint16_t height)
{
  image_t *img = load_image (filename);

  if (img == NULL)
    {
      error ("GFX - draw_image - Image load failure for %s.", 
             filename);
      return FAILURE;
    }

  return draw_image_direct (img, 
                            image_x, 
                            image_y, 
                            screen_x, 
                            screen_y,
                            width,
                            height);
}


/* Draw a rectangular portion of an image on-screen, using a direct
   pointer to the driver-specific image data. */

bool_t
draw_image_direct (void *data, int16_t image_x, int16_t image_y,
                   int16_t screen_x, int16_t screen_y, uint16_t width,
                   uint16_t height)
{
  return (*g_modules.gfx.draw_image_internal) (data,
                                               image_x,
                                               image_y,
                                               screen_x,
                                               screen_y,
                                               width,
                                               height);
}


/* Delete an image previously loaded into the image cache. */

bool_t
delete_image (const char filename[])
{
  return g_hash_table_remove (sg_images, filename);
}


/* Delete all images in the image cache. */

void
clear_images (void)
{
  g_hash_table_destroy (sg_images);
}


/* Retrieve an image from the image cache. */

image_t *
find_image (const char filename[])
{
  return g_hash_table_lookup (sg_images, filename);
}


/* Adds a rectangle to the next update run. */

void
add_update_rectangle (uint16_t x,
                      uint16_t y,
                      int16_t width,
                      int16_t height)
{
  (*g_modules.gfx.add_update_rectangle_internal) (x,
                                                  y,
                                                  width,
                                                  height);
}


/* Clean up the graphics subsystem. */

void
cleanup_graphics (void)
{
  clear_images ();
}

