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
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "graphics.h"
#include "util.h"
#include "module.h"

static struct hash_object *sg_images[HASH_VALS];

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

void
fill_screen (unsigned char red, 
             unsigned char green, 
             unsigned char blue)
{
  (*g_modules.gfx.draw_rect) (0, 0, SCREEN_W, SCREEN_H, red, green, blue);
}

struct hash_object *
load_image (const char filename[])
{
  struct hash_object *image;
  void *data;

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

void
free_image (void *image)
{
  if (image)
    {
      (*g_modules.gfx.free_image_data) (image);
    }
}


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

  img = find_hash_object (sg_images, filename);

  /* Image not preloaded - try to load it now. */

  if (img == NULL)
    {
      img = load_image (filename);

      if (img == NULL)
        {
          fprintf (stderr, "GFX: Cannot load non-preloaded image.");
          return 0;
        }
    }

  return (*g_modules.gfx.draw_image) (img->data,
                                      image_x,
                                      image_y,
                                      screen_x,
                                      screen_y,
                                      width,
                                      height);
}

int
delete_image (const char filename[])
{
  return delete_hash_object (sg_images, filename);
}

/* Delete all images. */
void
clear_images (void)
{
  clear_hash_objects (sg_images);
}

struct hash_object *
get_image (const char filename[], struct hash_object *add_pointer)
{
  return get_hash_object (sg_images, filename, add_pointer);
}

void
cleanup_graphics (void)
{
  clear_images ();
}

