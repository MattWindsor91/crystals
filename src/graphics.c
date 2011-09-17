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

/* Width of the screen (in pixels). */
const uint16_t SCREEN_W = 640;


/* Height of the screen (in pixels). */
const uint16_t SCREEN_H = 480;


/**
 * Default root path for graphics, to be invoked if the root path
 * cannot be found in the configuration file.
 */
static const char DEFGFXPATH[] = "gfx/";


/**
 * Filename of the default font.
 */
static const char FONT_FILENAME[] = "font.png";


/**
 * Font character width, in pixels.
 */
const uint16_t FONT_W = 10;


/**
 * Font character height, in pixels.
 */
const uint16_t FONT_H = 10;


/* -- STATIC GLOBAL VARIABLES -- */

static GHashTable *sg_images;


/* -- STATIC DECLARATIONS -- */

/**
 * Attempts to load an image from the image cache.
 *
 * @param filename  The filename of the image to load, relative from
 *                  the graphics path.
 *
 * @return A pointer to the raw data of the image if
 *         successfully loaded and stored in the image cache;
 *         NULL otherwise.
 */
static image_t *load_image_from_cache (const char filename[]);


/**
 * Loads an image from its resource file.
 *
 * @param filename  The filename of the image to load, relative from
 *                  the graphics path.
 *
 * @return A pointer to the raw data of the image if
 *         successfully loaded and stored in the image cache;
 *         NULL otherwise.
 */
static image_t *load_image_from_file (const char filename[]);


/* -- DEFINITIONS -- */

/* Initialise the graphics subsystem. */

void
init_graphics (void)
{
  if (!load_module_gfx (cfg_get_str ("modules",
                                    "graphics_module",
                                    g_config),
                        &g_modules))
    {
      fatal ("GRAPHICS - init_graphics - Could not load graphics module.");
    }


  /* Call on the graphics backend to create a screen. */
  if (!(*g_modules.gfx.init_screen_internal)
      (SCREEN_W, SCREEN_H, SCREEN_D))
    {
      fatal ("GRAPHICS - init_graphics - Could not init screen.");
    }


  /* Initialise the image list. */
  sg_images = g_hash_table_new_full(g_str_hash,
                                    g_str_equal,
                                    free,
                                    free_image);
}


/* Given a relative path to an image file, appends the graphics root
   path to it and returns a pointer to the created string. */
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


/* Writes a string on the screen, using the standard font and given
   alignment. */
void
write_string_aligned (int16_t x,
                      int16_t y,
                      uint16_t box_width,
                      alignment_t alignment,
                      const char string[])
{
  size_t slength;    /* Length of the string, in characters. */
  int16_t x1;
  uint16_t length;   /* Length of the string on-screen, in pixels. */

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


  write_string (x1, y, string);
}


/* Writes a string on the screen, using the standard font and given
   alignment. */
void
write_string (int16_t x,
              int16_t y,
              const char string[])
{
  uint8_t chr;
  int16_t current_x = x;
  size_t i;
  size_t slength = strlen (string);
  image_t *font = load_image (FONT_FILENAME);

  if (font == NULL)
    {
      error ("Couldn't load font!");
      return;
    }

  for (i = 0; i < slength; i+= 1)
    {
      chr = (uint8_t) string[i];
      draw_image_direct (font,
                         long_to_int16 ((chr % 16) * FONT_W),
                         long_to_int16 (((chr - (chr % 16))/16) * FONT_H),
                         current_x,
                         y,
                         FONT_W,
                         FONT_H);

      current_x += FONT_W;
    }


  /* Instruct the current state to update the screen. */
  add_update_rectangle (x, y, current_x - x + 1, FONT_H);
  state_handle_dirty_rect (x, y, current_x - x + 1, FONT_H);
}


/* Updates the screen. */
void
update_screen (uint32_t delta)
{
  static uint32_t total_useconds;

  total_useconds += delta;

  if (total_useconds >= USECONDS_PER_FRAME)
    {
      (*g_modules.gfx.update_screen_internal) ();
      total_useconds = 0;
    }
}


/* Draws a rectangle on the screen of the given colour. */
void
draw_rectangle (int16_t x,
                int16_t y,
                uint16_t width,
                uint16_t height,
                uint8_t red,
                uint8_t green,
                uint8_t blue)
{
  (*g_modules.gfx.draw_rect_internal)
    (x, y, width, height, red, green, blue);
}


/* Fills the screen with the given colour. */
void
fill_screen (uint8_t red, uint8_t green, uint8_t blue)
{
  draw_rectangle (0, 0, SCREEN_W, SCREEN_H, red, green, blue);
}


/* Translates the screen by a co-ordinate pair, leaving damage. */
void
scroll_screen (int16_t x_offset, int16_t y_offset)
{
  (*g_modules.gfx.scroll_screen_internal)
    (x_offset, y_offset);
}


/* Loads an image. */
void *
load_image (const char filename[])
{
  image_t *cache_data;
  image_t *file_data;

  g_assert (filename != NULL);

  cache_data = load_image_from_cache (filename);
  if (cache_data != NULL)
    return cache_data;

  /* Cache miss */
  file_data = load_image_from_file (filename);
  if (file_data != NULL)
    return file_data;

  fatal ("GFX - load_image - Couldn't load image data for %s.",
         filename);
  return NULL;
}


/* Attempts to load an image from the image cache. */
static image_t *
load_image_from_cache (const char filename[])
{
  g_assert (filename != NULL);
  return g_hash_table_lookup (sg_images, filename);
}


/* Loads an image from its resource file. */
static image_t *
load_image_from_file (const char filename[])
{
  char *path;
  image_t *data;

  g_assert (filename != NULL);

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


/* Frees image data. */
void
free_image (image_t *image)
{
  g_assert (image != NULL);
  (*g_modules.gfx.free_image_data) (image);
}


/* Draws a rectangular portion of an image on-screen. */
void
draw_image (const char filename[],
            int16_t image_x,
            int16_t image_y,
            int16_t screen_x,
            int16_t screen_y,
            uint16_t width,
            uint16_t height)
{
  image_t *img = load_image (filename);
  if (img == NULL)
    {
      error ("GFX - draw_image - Image load failure for %s.",
             filename);
    }

  draw_image_direct (img,
                     image_x,
                     image_y,
                     screen_x,
                     screen_y,
                     width,
                     height);
}


/* Draws a rectangular portion of an image on-screen, using a direct
   pointer to the driver-specific image data. */
void
draw_image_direct (void *data, int16_t image_x, int16_t image_y,
                   int16_t screen_x, int16_t screen_y, uint16_t width,
                   uint16_t height)
{
  (*g_modules.gfx.draw_image_internal) (data,
                                        image_x,
                                        image_y,
                                        screen_x,
                                        screen_y,
                                        width,
                                        height);
}


/* Deletes an image previously loaded into the image cache. */
bool
delete_image (const char filename[])
{
  gboolean success = g_hash_table_remove (sg_images, filename);
  if (success == TRUE)
    return true;
  else
    return false;
}


/* Deletes all images in the image cache. */
void
clear_images (void)
{
  g_hash_table_destroy (sg_images);
}


/* Retrieves an image from the image cache. */
image_t *
find_image (const char filename[])
{
  return g_hash_table_lookup (sg_images, filename);
}


/* Adds a rectangle to the next update run. */
void
add_update_rectangle (int16_t x,
                      int16_t y,
                      uint16_t width,
                      uint16_t height)
{
  (*g_modules.gfx.add_update_rectangle_internal) (x,
                                                  y,
                                                  width,
                                                  height);
}


/* Cleans up the graphics subsystem. */
void
cleanup_graphics (void)
{
  clear_images ();
}

