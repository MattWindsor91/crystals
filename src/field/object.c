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
 * @file    src/field/object.c
 * @author  Matt Windsor
 * @brief   Low-level object functions.
 */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <glib.h>

#include "object.h"
#include "object-api.h"
#include "../util.h"


/* -- STATIC GLOBAL VARIABLES -- */

static GHashTable *sg_objects;


/* -- STATIC DECLARATIONS -- */

/**
 * Check to see whether the given object falls within the given dirty
 * rectangle and, if so, mark the object as dirty.
 *
 * @param object        The object to test.
 * @param rect_pointer  Pointer to the dirty rectangle to test.
 */
static void
dirty_object_test_post_check (object_t *object,
                              dirty_rectangle_t *rect);


/* -- DEFINITIONS -- */

/* Initialise the object base. */

bool_t
init_objects (void)
{
  sg_objects = g_hash_table_new_full (g_str_hash,
                                      g_str_equal,
                                      free,
                                      free_object);

  if (sg_objects)
    {
      return SUCCESS;
    }
  else
    {
      return FAILURE;
    }
}


/* Create a new object and add it to the object table. */

object_t *
add_object (const char object_name[],
            const char script_filename[])
{
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
  {
    object_t *object = malloc (sizeof (object_t));
    if (object == NULL)
      {
        error ("OBJECT - add_object - Allocation failed for %s.", 
               object_name);
        return NULL;
      }

    object->image = init_object_image ();
    if (object->image == NULL)
      {
        error ("OBJECT - add_object - Initialisation failed for image of %s.",
               object_name);
        free_object (object);
        return NULL;
      }

    object->name = g_strdup (object_name);
    if (object->name == NULL)
      {
        error ("OBJECT - add_object - Allocation failed for name of %s.", 
               object_name);
        free_object (object);
        return NULL;
      }

    object->script_filename = g_strdup (script_filename);
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

    g_hash_table_insert (sg_objects,
                         g_strdup(object_name),
                         object);
  
    return object;
  }
}


/* Change the tag associated with an object. */
void
set_object_tag (object_t *object, layer_tag_t tag)
{
  g_assert (object != NULL);
  object->tag = tag;
}


/* Get the graphic associated with an object. */

object_image_t *
get_object_image (object_t *object)
{
  g_assert (object != NULL);
  return object->image;
}


/* Change the graphic associated with an object. */

bool_t
set_object_image (object_t *object,
                  const char filename[],
                  int16_t image_x,
                  int16_t image_y,
                  uint16_t width,
                  uint16_t height)
{
  g_assert (object && object->image && filename);

  if (object->image->filename != NULL)
    {
      free (object->image->filename);
    }

  object->image->filename = g_strdup (filename);
  if (object->image->filename == NULL)
    {
      error ("OBJECT - set_object_image - Couldn't alloc image FN for %s.",
             object->name);
      return FAILURE;
    }

  object->image->image_x = image_x;
  object->image->image_y = image_y;
  object->image->width = width;
  object->image->height = height;

  return SUCCESS;
}


/* Retrieve the object's co-ordinates on-map. */

bool_t
get_object_coordinates (object_t *object,
                        int32_t *x_pointer,
                        int32_t *y_pointer,
                        reference_t reference)
{
  g_assert (object && object->image);

  *x_pointer = object->image->map_x;
  *y_pointer = object->image->map_y;

  if (reference == BOTTOM_LEFT)
    *y_pointer += (object->image->height - 1);

  return SUCCESS;
}


/* Set the object's co-ordinates on map. */
bool_t
set_object_coordinates (object_t *object,
                        int32_t x,
                        int32_t y,
                        reference_t reference)
{
  g_assert (object && object->image);

  /* No point setting coordinates if they're the same. */

  if (object->image->map_x == x
      && object->image->map_y == y)
    return SUCCESS;

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


/* Mark an object as being dirty on the given map view. */
bool_t
set_object_dirty (object_t *object,
                  mapview_t *mapview)
{
  g_assert (object && mapview);

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
    }

  return SUCCESS;
}


void
free_object (void *object)
{
  object_t *objectc = (object_t *) object;
  if (objectc)
    {
      if (objectc->name)
        free (objectc->name);

      if (objectc->script_filename)
        free (objectc->script_filename);

      if (objectc->image)
        free_object_image (objectc->image);

      free (objectc);
    }
}


/* Remove an object from the object table. */
bool_t
delete_object (const char object_name[])
{
  return g_hash_table_remove (sg_objects, object_name);
}


/* Retrieve an object. */
object_t *
get_object (const char object_name[])
{
  return g_hash_table_lookup (sg_objects, object_name);
} 


/* Check to see whether the given object falls within the given dirty
 * rectangle and, if so, mark the object as dirty (if it has not been
 * marked before).
 */
void
dirty_object_test (gpointer key_ptr,
                   gpointer object_ptr,
                   gpointer rect_ptr)
{
  object_t *object = (object_t *) object_ptr;
  dirty_rectangle_t *rect = (dirty_rectangle_t *) rect_ptr;

  (void) key_ptr; /* Avoid unused warnings */

  g_assert (object && rect);

  if (object->is_dirty)
    {
      /* No need to dirty an object twice. */
      return;
    }
    
  dirty_object_test_post_check (object, rect);
}


/* Check to see whether the given object falls within the given dirty
 * rectangle and, if so, mark the object as dirty.
 */
static void
dirty_object_test_post_check (object_t *object,
                              dirty_rectangle_t *rect)
{
  mapview_t *mapview = rect->parent;
      
  int rect_left = rect->start_x;
  int rect_top = rect->start_y;
  int rect_right = rect->start_x + rect->width - 1;
  int rect_bottom = rect->start_y + rect->height - 1;
      
  int object_left = object->image->map_x;
  int object_top = object->image->map_y;
  int object_right = object->image->map_x + object->image->width - 1;
  int object_bottom = object->image->map_y + object->image->height - 1;
      
  /* Use separating axis theorem, sort of, to decide whether the
   * object rect and the dirty rect intersect.
   */
  if ((object_left <= rect_right && object_right > rect_left)
      && (object_top <= rect_bottom && object_bottom > rect_top))
    {
      set_object_dirty (object, mapview);
    }      
}


/** Apply the given function to all objects. */

void
apply_to_objects (GHFunc function,
                  void *data)
{
  g_hash_table_foreach (sg_objects, 
                        function, 
                        data);
}


/** Clean up the objects subsystem. */

void
cleanup_objects (void)
{
  g_hash_table_destroy (sg_objects);
}
