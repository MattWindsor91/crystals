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

/** @file    src/field/object.c
 *  @author  Matt Windsor
 *  @brief   Low-level object functions.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "object.h"
#include "object-api.h"
#include "../util.h"

static struct hash_object *sg_objects[HASH_VALS];


int
init_objects (void)
{
  unsigned int i;

  for (i = 0; i < HASH_VALS; i++)
    sg_objects[i] = NULL;

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
      error ("OBJECT - add_object - Object name is NULL.");
      return NULL;
    }

  if (script_filename == NULL)
    {
      error ("OBJECT - add_object - Script filename is NULL.");
      return NULL;
    }

  /* Try to allocate an object. */

  object = malloc (sizeof (struct object_t));

  if (object == NULL)
    {
      error ("OBJECT - add_object - Allocation failed for %s.", 
             object_name);
      return NULL;
    }

  /* Try to allocate and initialise an object image. */

  object->image = malloc (sizeof (struct object_image));

  if (object->image == NULL)
    {
      error ("OBJECT - add_object - Allocation failed for image of %s.", 
             object_name);
      free_object (object);
      return NULL;
    }

  /* Try to copy the object name over. */

  object->name = malloc (sizeof (char) * (strlen (object_name) + 1));

  if (object->name == NULL)
    {
      error ("OBJECT - add_object - Allocation failed for name of %s.", 
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
      error ("OBJECT - add_object - Allocation failed for filename of %s.", 
               object_name);
      free_object (object);
      return NULL;
    }

  /* Finally, nullify everything else. */

  object->tag = 0;
  object->is_dirty = FALSE;

  init_object_image (object->image, object);

  /* Try to store the object. */

  result = create_hash_object (sg_objects, 
                               object_name,
                               DATA_OBJECT, 
                               object);

  if (result == NULL)
    {
      error ("OBJECT - add_object - Store failed for %s.", 
               object_name);
      free_object (object);
      return NULL;
    }

  return (struct object_t*) result->data;
}


/* Change the tag associated with an object. */

int
set_object_tag (struct object_t *object, 
                layer_t tag)
{
  if (object == NULL)
    {
      error ("OBJECT - set_object_tag - Tried to set tag of null object.");
      return FAILURE;
    }

  object->tag = tag;
  
  return SUCCESS;
}


/* Get the graphic associated with an object. */

struct object_image *
get_object_image (struct object_t *object)
{
  if (object == NULL)
    {
      error ("OBJECT - get_object_image - Tried to get image of null object.");
      return NULL;
    }

  return object->image;
}


/* Change the graphic associated with an object. */

int
set_object_image (struct object_t *object, 
                  const char filename[],
                  short image_x,
                  short image_y,
                  unsigned short width,
                  unsigned short height)
{
  /* Sanity checking. */

  if (object == NULL)
    {
      error ("OBJECT - set_object_image - Tried to set image of null object.");
      return FAILURE;
    }

  if (object->image == NULL)
    {
      error ("OBJECT - set_object_image - Object %s has no image dataset.", 
             object->name);
      return FAILURE;
    }

  if (filename == NULL)
    {
      error ("OBJECT - set_object_image - Tried to set image FN to null.");
      return FAILURE;
    }

  /* Set up the filename. */

  if (object->image->filename != NULL)
    free (object->image->filename);

  object->image->filename = malloc (sizeof (char) * (strlen (filename) + 1));

  if (object->image->filename == NULL)
    {
      error ("OBJECT - set_object_image - Couldn't alloc image FN for %s.",
             object->name);
      return FAILURE;
    }

  strncpy (object->image->filename, filename, (strlen (filename) + 1));

  /* Copy everything else. */

  object->image->image_x = image_x;
  object->image->image_y = image_y;
  object->image->width = width;
  object->image->height = height;

  return SUCCESS;
}


int
get_object_coordinates (struct object_t *object, 
                        int *x_pointer,
                        int *y_pointer,
                        unsigned short reference)
{
  /* Sanity checking. */

  if (object == NULL)
    {
      error ("OBJECT - get_object_coordinates - Tried to get coords of NULL object.");
      return FAILURE;
    }

  if (object->image == NULL)
    {
      error ("OBJECT - get_object_coordinates - Object %s has no image dataset.", 
             object->name);
      return FAILURE;
    }

  *x_pointer = object->image->map_x;
  *y_pointer = object->image->map_y;

  if (reference == BOTTOM_LEFT)
    *y_pointer += (object->image->height - 1);

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
      error ("OBJECT - set_object_coordinates - Tried to get coords of NULL object.");
      return FAILURE;
    }

  if (object->image == NULL)
    {
      error ("OBJECT - set_object_coordinates - Object %s has no image dataset.", 
             object->name);
      return FAILURE;
    }

  /* No point setting coordinates if they're the same. */

  if (object->image->map_x == x
      && object->image->map_y == y)
    return SUCCESS;

  /* Set the coordinates. */

  object->image->map_x = x;
  object->image->map_y = y;

  if (reference == BOTTOM_LEFT)
    {
      /* Check to see if the offset will send the object off the map. */

      if (object->image->map_y < object->image->height - 1)
        {
          fatal ("OBJECT - set_object_coordinates - Object %s has bad coords.", 
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
      error ("OBJECT - set_object_dirty - Tried to set a NULL object dirty.");
      return FAILURE;
    }

  if (mapview == NULL)
    {
      error ("OBJECT - set_object_dirty - Tried dirtying on a NULL mapview.");
      return FAILURE;
    }

  /* If we're already dirty, no need to run this again. */

  if (object->is_dirty == TRUE)
    return SUCCESS;

  /* If the object has no image (the filename is NULL) then ignore the
     dirty request. */
  if (object->image->filename == NULL)
    {
      return SUCCESS;
    }

  /* Ensure the object's co-ordinates don't go over the map
     width/height! */

  if ((object->image->map_x + object->image->width
       > mapview->map->width * TILE_W)
      || (object->image->map_y + object->image->height 
          > mapview->map->height * TILE_H))
    {
      error ("OBJECT - set_object_dirty - Object %s out of bounds.", 
               object->name);
      return FAILURE;
    }

  /* And now, the business end. */

  if (object->tag != 0)
    {
      if (add_object_image (mapview, object->tag, object)
          == FAILURE)
        return FAILURE;

      object->is_dirty = TRUE;

      /* Mark the nearby tiles. */

      mark_dirty_rect (mapview,
                       object->image->map_x, 
                       object->image->map_y, 
                       object->image->width, 
                       object->image->height);

    }

  return SUCCESS;
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
  return delete_hash_object (sg_objects, object_name);
}


struct object_t *
get_object (const char object_name[], struct hash_object *add_pointer)
{
  struct hash_object *result;

  result = get_hash_object (sg_objects, object_name, add_pointer);

  if (result == NULL)
    return NULL;
  else
    return (struct object_t*) result->data;
} 


int
dirty_object_test (struct hash_object *hash_object, void *rect_pointer)
{
  struct object_t *object;
  struct map_view *mapview;
  struct dirty_rectangle *rect;

  int start_x;
  int start_y;
  int width;
  int height;

  /* Sanity-check the hash object and dirty rectangle data. */

  if (hash_object == NULL)
    {
      error ("OBJECT - dirty_object_test - Given hash object is NULL.\n");
      return FAILURE;
    }

  if (hash_object->data == NULL)
    {
      error ("OBJECT - dirty_object_test - Hash object has no data.\n");
      return FAILURE;
    }

  if (rect_pointer == NULL)
    {
      error ("OBJECT - dirty_object_test - Given dirty rect pointer is NULL.\n");
      return FAILURE;
    }

  object = (struct object_t *) hash_object->data;
  rect = (struct dirty_rectangle *) rect_pointer;

 /* If an object is already dirty, don't bother checking. */

  if (object->is_dirty == TRUE)
    return SUCCESS;

  mapview = rect->parent;
  start_x = rect->start_x;
  start_y = rect->start_y;
  width   = rect->width;
  height  = rect->height;

  /* Use separating axis theorem, sort of, to decide whether the
     object rect and the dirty rect intersect. */

  if ((object->image->map_x <= (start_x + width - 1))
      && (object->image->map_x
          + object->image->width >= start_x)
      && (object->image->map_y <= (start_y + height - 1))
      && (object->image->map_y
          + object->image->height >= start_y))
    {
      set_object_dirty (object, mapview);
    }

  return SUCCESS;
}


int
apply_to_objects (int (*function) (struct hash_object *object, 
                                   void *data),
                   void *data)
{
  return apply_to_hash_objects (sg_objects, 
                                function, 
                                data);
}


void
cleanup_objects (void)
{
  clear_hash_objects (sg_objects);
}
