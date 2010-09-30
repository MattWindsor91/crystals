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

/** @file    object.c
 *  @author  Matt Windsor
 *  @brief   Low-level object functions.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "object.h"
#include "main.h"
#include "util.h"

struct hash_object *g_objects[HASH_VALS];

int
init_objects (void)
{
  unsigned int i;

  for (i = 0; i < HASH_VALS; i++)
    g_objects[i] = NULL;

  /* TEST DATA */

  {
    struct object_t *test;
    struct object_t *test2;

    test = add_object ("Test1", "null");
    test2 = add_object ("Test2", "null");
    
    set_object_tag (test, 1);
    set_object_tag (test2, 1);
    set_object_image (test, "gfx/testobj.png", 0, 0, 60, 60, 16, 48);
    set_object_image (test2, "gfx/testobj.png", 16, 0, 70, 70, 16, 48);
  }

  return SUCCESS;
}

struct object_t *
add_object (const char object_name[],
            const char script_filename[])
{
  struct object_t *object;
  struct hash_object *result;

  /* Sanity-check passed strings. */

  if (object_name == NULL)
    {
      fprintf (stderr, "OBJECT: Error: Object name is NULL.\n");
      return NULL;
    }

  if (script_filename == NULL)
    {
      fprintf (stderr, "OBJECT: Error: Script filename is NULL.\n");
      return NULL;
    }

  /* Try to allocate an object. */

  object = malloc (sizeof (struct object_t));

  if (object == NULL)
    {
      fprintf (stderr, "OBJECT: Error: Allocation failed for %s.\n", 
               object_name);
      return NULL;
    }

  /* Try to allocate and initialise an object image. */

  object->image = malloc (sizeof (struct object_image));

  if (object->image == NULL)
    {
      fprintf (stderr, "OBJECT: Error: Allocation failed for rnode of %s.\n", 
               object_name);
      free_object (object);
      return NULL;
    }

  /* Try to copy the object name over. */

  object->name = malloc (sizeof (char) * (strlen (object_name) + 1));

  if (object->name == NULL)
    {
      fprintf (stderr, "OBJECT: Error: ON allocation failed for %s.\n", 
               object_name);
      free_object (object);
      return NULL;
    }

  strncpy (object->name, object_name,
           sizeof (char) * strlen (object_name) + 1);

  /* Try to copy the filename over. */

  object->script_filename = malloc (sizeof (char)
                                    * (strlen (script_filename) + 1));

  if (object->script_filename == NULL)
    {
      fprintf (stderr, "OBJECT: Error: FN allocation failed for %s.\n", 
               object_name);
      free_object (object);
      return NULL;
    }

  /* Finally, nullify everything else. */

  object->tag = 0;
  object->is_dirty = 0;

  init_object_image (object->image, object);

  /* Try to store the object. */

  result = create_hash_object (g_objects, 
                               object_name,
                               DATA_OBJECT, 
                               object);

  if (result == NULL)
    {
      fprintf (stderr, "OBJECT: Error: Store failed for %s.\n", 
               object_name);
      free_object (object);
      return NULL;
    }

  return (struct object_t*) result->data;
}

int
set_object_tag (struct object_t *object, 
                layer_t tag)
{
  if (object == NULL)
    {
      fprintf (stderr, "OBJECT: Error: Tried to set tag of null object.\n");
      return FAILURE;
    }

  object->tag = tag;
  
  return SUCCESS;
}

int
set_object_image (struct object_t *object, 
                  const char filename[],
                  short image_x,
                  short image_y,
                  int map_x,
                  int map_y,
                  unsigned short width,
                  unsigned short height)
{
  /* Sanity checking. */

  if (object == NULL)
    {
      fprintf (stderr, "OBJECT: Error: Tried to set image of null object.\n");
      return FAILURE;
    }

  if (object->image == NULL)
    {
      fprintf (stderr, "OBJECT: Error: Object %s has no image dataset.\n", 
               object->name);
      return FAILURE;
    }

  if (filename == NULL)
    {
      fprintf (stderr, "OBJECT: Error: Tried to set image FN to null.\n");
      return FAILURE;
    }

  /* Set up the filename. */

  if (object->image->filename != NULL)
    free (object->image->filename);

  object->image->filename = malloc (sizeof (char) * (strlen (filename) + 1));

  if (object->image->filename == NULL)
    {
      fprintf (stderr, "OBJECT: Error: Couldn't alloc image FN for %s.\n",
               object->name);
      return FAILURE;
    }

  strncpy (object->image->filename, filename, (strlen (filename) + 1));

  /* Copy everything else. */

  object->image->image_x = image_x;
  object->image->image_y = image_y;
  object->image->map_x = map_x;
  object->image->map_y = map_y;
  object->image->width = width;
  object->image->height = height;

  return SUCCESS;
}

int
set_object_coordinates (struct object_t *object, 
                        int x, 
                        int y,
                        unsigned short reference)
{
  /* Sanity checking. */

  if (object == NULL)
    {
      fprintf (stderr, "OBJECT: Error: Tried to set coords on NULL object.\n");
      return FAILURE;
    }

  if (object->image == NULL)
    {
      fprintf (stderr, "OBJECT: Error: Object %s has no image dataset.\n", 
               object->name);
      return FAILURE;
    }

  /* Set the coordinates. */

  object->image->map_x = x;
  object->image->map_y = y;

  if (reference == BOTTOM_LEFT)
    {
      /* Check to see if the offset will send the object off the map. */

      if (object->image->map_y < object->image->height - 1)
        {
          fprintf (stderr, "OBJECT: Error: Object %s has bad coords.\n", 
                   object->name);
          return FAILURE;
        }

      object->image->map_y -= (object->image->height - 1);
    }

  return SUCCESS;
}

int
set_object_dirty (struct object_t *object, 
                  struct map_view *mapview)
{
  /* Sanity checking. */

  if (object == NULL)
    {
      fprintf (stderr, "OBJECT: Error: Tried to set a NULL object dirty.\n");
      return FAILURE;
    }

  if (mapview == NULL)
    {
      fprintf (stderr, "OBJECT: Error: Tried dirtying on a NULL mapview.\n");
      return FAILURE;
    }

  /* If we're already dirty, no need to run this again. */

  if (object->is_dirty == TRUE)
    return SUCCESS;

  /* Ensure the object's co-ordinates don't go over the map
     width/height! */

  if ((object->image->map_x + object->image->width
       > mapview->map->width * TILE_W)
      || (object->image->map_y + object->image->height 
          > mapview->map->height * TILE_H))
    {
      fprintf (stderr, "OBJECT: Error: Object %s out of bounds.\n", 
               object->name);
      return FAILURE;
    }

  /* And now, the business end. */

  if (object->tag != 0)
    {
      object->is_dirty = TRUE;
      return add_object_image (mapview, object->tag, object->image);
    }

  return FAILURE;
}

void
free_object (struct object_t *object)
{
  if (object)
    {
      if (object->name)
        free (object->name);

      if (object->script_filename)
        free (object->script_filename);

      if (object->image)
        free_object_image (object->image);

      free (object);
    }
}

int
delete_object (const char object_name[])
{
  return delete_hash_object (g_objects, object_name);
}


struct object_t *
get_object (const char object_name[], struct hash_object *add_pointer)
{
  struct hash_object *result;

  result = get_hash_object (g_objects, object_name, add_pointer);

  if (result == NULL)
    return NULL;
  else
    return (struct object_t*) result->data;
} 

int
dirty_object_test (struct hash_object *hash_object, va_list ap)
{
  struct object_t *object;
  struct map_view *mapview;
  int start_x;
  int start_y;
  int width;
  int height;

  /* Sanity-check the hash object. */

  if (hash_object == NULL)
    {
      fprintf (stderr, "OBJECT: Error: Given hash object is NULL.\n");
      return FAILURE;
    }

  if (hash_object->data == NULL)
    {
      fprintf (stderr, "OBJECT: Error: Hash object has no data.\n");
      return FAILURE;
    }

  object = (struct object_t *) hash_object->data;

 /* If an object is already dirty, don't bother checking. */

  if (object->is_dirty == TRUE)
    return SUCCESS;

  mapview = va_arg (ap, struct map_view *);
  start_x = va_arg (ap, int);
  start_y = va_arg (ap, int);
  width   = va_arg (ap, int);
  height  = va_arg (ap, int);

  /* Use separating axis theorem, sort of, to decide whether the
     object rect and the dirty rect intersect. */

  if ((object->image->map_x <= (start_x + width) * TILE_W)
      && (object->image->map_x
          + object->image->width >= start_x * TILE_W)
      && (object->image->map_y <= (start_y + height) * TILE_H)
      && (object->image->map_y
          + object->image->height >= start_y * TILE_H))
    {
      set_object_dirty (object, mapview);

      /* Mark the nearby tiles. */

      mark_dirty_rect (mapview,
                       (object->image->map_x / TILE_W) - 1, 
                       (object->image->map_y / TILE_H) - 1, 
                       MAX (3, object->image->width / TILE_W), 
                       MAX (3, object->image->height / TILE_H));
    }
  return SUCCESS;
}

void
cleanup_objects (void)
{
  clear_hash_objects (g_objects);
}
