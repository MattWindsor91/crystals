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

/** @file    graphics.c
 *  @author  Matt Windsor
 *  @brief   Generic graphics system.
 *
 *  @todo FIXME: Add graphics location to graphics paths at run-time 
 *               instead of hard-coding it.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

#include "main.h"      /* g_config */
#include "parser.h"    /* Configuration */
#include "state.h"     /* State dirty rectangles handling */
#include "graphics.h"
#include "module.h"
#include "util.h"


/* -- CONSTANTS -- */

const char DEFGFXPATH[] = "gfx/";
const char FONT_FILENAME[] = "font.png";

const unsigned short FONT_W = 10;
const unsigned short FONT_H = 10;


/* -- STATIC GLOBAL VARIABLES -- */

static struct hash_object *sg_images[HASH_VALS];


/* -- DEFINITIONS -- */

/* Initialise the graphics subsystem. */

int
init_graphics (void)
{
  unsigned int i;

  if (load_module_gfx (cfg_get ("graphics_module", g_config), &g_modules) == FAILURE)
    {
      error ("GRAPHICS - init_graphics - Could not load graphics module.");

      return FAILURE;
    }

  if ((*g_modules.gfx.init_screen_internal)
      (SCREEN_W, SCREEN_H, SCREEN_D) == FAILURE)
    {
      error ("GRAPHICS - init_graphics - Could not init screen.");
      return FAILURE;
    }
 
  for (i = 0; i < HASH_VALS; i++)
    sg_images[i] = NULL;

  return SUCCESS;
}


/* Given a relative path to an image file, append the graphics root
   path to it and store it in the given pointer. */

char *
get_absolute_path (const char path[])
{
  char *root_path;
  char *absolute_path;

  /* Get the root path from the configuration, if it exists. */

  root_path = cfg_get ("graphics_path", g_config);

  if (root_path == NULL)
    {
      /* Using default path. */

      error ("GRAPHICS - get_absolute_path - No graphics_path in config.");
      error ("Using default path instead.");

      absolute_path = calloc (strlen (path) + strlen (DEFGFXPATH) + 1, 
                               sizeof (char));

      if (absolute_path == NULL)
        {
          error ("GRAPHICS - get_absolute path - Memory allocation failure.");
          return NULL;
        }

      strcat (absolute_path, DEFGFXPATH);
    }
  else
    {
      /* Using configuration path. */

      absolute_path = calloc (strlen (path) + strlen (root_path) + 1, 
                               sizeof (char));

      if (absolute_path == NULL)
        {
          error ("GRAPHICS - get_absolute path - Memory allocation failure.");
          return NULL;
        }

      strcat (absolute_path, root_path);
    }

  strcat (absolute_path, path);

  return absolute_path;
}

/* Write a string on the screen, using the standard font. */

void
write_string (short x, short y,
              unsigned short box_width, unsigned char alignment,
              const char string[])
{
  char chr;
  int x1;
  unsigned short i, length, midpoint;
  size_t slength;

  slength = strlen (string);
  length = unsigned_to_unsigned_short (FONT_W * slength);
  
  /* Set up alignment. */

  switch (alignment)
    {
    default:
    case ALIGN_LEFT:
      x1 = x;
      break;
    case ALIGN_RIGHT:
      x1 = unsigned_to_short (box_width - slength);
      break;
    case ALIGN_CENTRE:   
      midpoint = to_unsigned_short (x + (box_width / 2));
      x1 = to_short (midpoint - (length / 2));
      break;
    }


  /* Draw each character using the font image. */
  for (i = 0; i < slength; i++)
    {
      chr = string[i];
      draw_image (FONT_FILENAME, 
                  to_short ((chr % 16) * FONT_W), 
                  to_short (((chr - (chr % 16))/16) * FONT_H), 
                  to_short (x1),
                  to_short (y),
                  FONT_W,
                  FONT_H);

      x1 += FONT_W;
    }  


  /* Instruct the current state to update the screen. */

  state_handle_dirty_rect (x, y, length, FONT_H);
}


/* Update the screen. */

int
update_screen (void)
{
  return (*g_modules.gfx.update_screen_internal) ();
}


/* Fill the screen with the given colour. */

int
fill_screen (unsigned char red, 
             unsigned char green, 
             unsigned char blue)
{
  return (*g_modules.gfx.draw_rect_internal)
    (0, 0, SCREEN_W, SCREEN_H, red, green, blue);
}


/* Translate the screen by a co-ordinate pair, leaving damage. */

int
scroll_screen (short x_offset, short y_offset)
{
  return (*g_modules.gfx.scroll_screen_internal)
    (x_offset, y_offset);
}


/* Load an image. */

struct hash_object *
load_image (const char filename[])
{
  struct hash_object *image;
  struct hash_object *get_try;
  void *data;
  char *path;

  /* Sanity-check the filename. */

  if (filename == NULL)
    {
      fatal ("GFX - load_image - Filename is NULL.");
      return NULL;
    }

  /* First, check to see if the image isn't already there. */

  get_try = find_hash_object (sg_images, filename);

  if (get_try != NULL)
    {
      /* It is. */
      return get_try;
    }

  /* At this point in execution, we have deduced that the image isn't
     there, so try to load it. */

  /* First, expand the filename into an absolute path that we can use
     to find the image on the filesystem. */

  path = get_absolute_path (filename);

  if (path == NULL)
    {
      fatal ("GFX - load_image - Couldn't get absolute path for %s.", 
             filename);
      return NULL;
    }

  /* Next, try to load the image data. */

  data = (*g_modules.gfx.load_image_data) (path);

  /* We no longer need the absolute path, so free it. */

  free (path);

  if (data == NULL)
    {
      fatal ("GFX - load_image - Couldn't load image data for %s.", 
             filename);
      return NULL;
    }

  /* Then try to allocate a hash object. */

  image = create_hash_object (sg_images, 
                              filename, 
                              DATA_IMAGE, 
                              data);

  if (image == NULL)
    {
      fatal ("GFX - load_image - Hash object create failed for %s.", 
             filename);
      free_image (data);
      return NULL;
    }

  return image;
}


/* Free image data. */

int
free_image (void *image)
{
  if (image == NULL)
    {
      error ("GFX - free_image - Tried to free NULL image.");
      return FAILURE;
    }

  return (*g_modules.gfx.free_image_data) (image);
}



/* Draw a rectangular portion of an image on-screen. */

int
draw_image (const char filename[],
            short image_x,
            short image_y,
            short screen_x,
            short screen_y,
            unsigned short width,
            unsigned short height)
{
  struct hash_object *img;

  /* Try to load the image if it isn't already loaded. */

  img = load_image (filename);

  if (img == NULL)
    {
      error ("GFX - draw_image - Image load failure for %s.", 
             filename);

      return FAILURE;
    }

  return draw_image_direct (img->data, 
                            image_x, 
                            image_y, 
                            screen_x, 
                            screen_y,
                            width,
                            height);
}


/* Draw a rectangular portion of an image on-screen, using a direct
   pointer to the driver-specific image data. */

int
draw_image_direct (void *data,
                   short image_x,
                   short image_y,
                   short screen_x,
                   short screen_y,
                   unsigned short width,
                   unsigned short height)
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

int
delete_image (const char filename[])
{
  return delete_hash_object (sg_images, filename);
}


/* Delete all images in the image cache. */

void
clear_images (void)
{
  clear_hash_objects (sg_images);
}


/* Retrieve an image from the image cache. */

struct hash_object *
find_image (const char filename[])
{
  return get_hash_object (sg_images, filename, NULL);
}


/* Clean up the graphics subsystem. */

void
cleanup_graphics (void)
{
  clear_images ();
}

