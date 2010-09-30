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

#include "graphics.h"
#include "util.h"
#include "module.h"

/* -- STATIC GLOBAL VARIABLES -- */

static struct hash_object *sg_images[HASH_VALS];

/* -- DEFINITIONS -- */

/* Initialise the graphics subsystem. */

int
init_graphics (void)
{
  unsigned int i;

  if (load_module_gfx ("gfx-sdl", &g_modules) == FAILURE)
    {
      fprintf (stderr, "ERROR: Could not load graphics module.\n");
      return FAILURE;
    }

  if ((*g_modules.gfx.init_screen) (SCREEN_W, SCREEN_H, SCREEN_D) == FAILURE)
    {
      fprintf (stderr, "ERROR: Could not init screen.!\n");
      return FAILURE;
    }
 
  for (i = 0; i < HASH_VALS; i++)
    sg_images[i] = NULL;

  return SUCCESS;
}

/* Fill the screen with the given colour. */

void
fill_screen (unsigned char red, 
             unsigned char green, 
             unsigned char blue)
{
  (*g_modules.gfx.draw_rect) (0, 0, SCREEN_W, SCREEN_H, red, green, blue);
}

/* Load an image. */

struct hash_object *
load_image (const char filename[])
{
  struct hash_object *image;
  struct hash_object *get_try;
  void *data;

  /* First, check to see if the image isn't already there. */

  get_try = find_hash_object (sg_images, filename);

  if (get_try != NULL)
    return get_try;

  /* At this point in execution, we have deduced that the image isn't
     there, so try to load it. */

  /* Sanity-check the filename. */

  if (filename == NULL)
    {
      fprintf (stderr, "GFX: Error: Filename is NULL.\n");
      return NULL;
    }

  /* First, try to load the image data. */

  data = (*g_modules.gfx.load_image_data) (filename);

  if (data == NULL)
    {
      fprintf (stderr, "GFX: Error: Couldn't load image data for %s.\n", 
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
      fprintf (stderr, "GFX: Error: Hash object create failed for %s.\n", 
               filename);
      free_image (data);
      return NULL;
    }

  return image;
}

/* Free image data. */

void
free_image (void *image)
{
  if (image == NULL)
    {
      fprintf (stderr, "GRAPHICS: Error: Tried to free NULL image.\n");
    }

  (*g_modules.gfx.free_image_data) (image);
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
      fprintf (stderr, "GFX: Error: Draw-time image load failure for %s.\n", 
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
  return (*g_modules.gfx.draw_image) (data,
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

