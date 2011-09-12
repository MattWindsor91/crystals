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


#include "../crystals.h"


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

void
init_objects (void)
{
  sg_objects = g_hash_table_new_full (g_str_hash,
                                      g_str_equal,
                                      g_free,
                                      free_object);

  g_assert (sg_objects);
}


/* Create a new object and add it to the object table. */

object_t *
add_object (const char object_name[],
            const char script_filename[])
{
  object_t *object = xcalloc (1, sizeof (object_t));

  g_assert (object_name != NULL);
  g_assert (script_filename != NULL);

  /* Try to allocate an object. */
  object->image = init_object_image ();
  g_assert (object->image != NULL);
  
  object->name = g_strdup (object_name);
  g_assert (object->name != NULL);
  
  object->script_filename = g_strdup (script_filename);
  g_assert (object->script_filename != NULL);

  /* Finally, nullify everything else. */
  object->tag = 0;
  object->is_dirty = FALSE;

  g_hash_table_insert (sg_objects,
                       g_strdup(object_name),
                       object);
  
  return object;
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

void
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
  g_assert (object->image->filename != NULL);

  object->image->image_x = image_x;
  object->image->image_y = image_y;
  object->image->width = width;
  object->image->height = height;
}


/* Retrieve the object's co-ordinates on-map. */

void
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
}


/* Set the object's co-ordinates on map. */
void
set_object_coordinates (object_t *object,
                        int32_t x,
                        int32_t y,
                        reference_t reference)
{
  g_assert (object && object->image);

  /* No point setting coordinates if they're the same. */

  if (object->image->map_x == x
      && object->image->map_y == y)
    return;

  object->image->map_x = x;
  object->image->map_y = y;

  if (reference == BOTTOM_LEFT)
    {
      /* Check to see if the offset will send the object off the map. */

      g_assert (object->image->map_y >= object->image->height - 1);

      object->image->map_y -= (object->image->height - 1);
    }
}


/* Mark an object as being dirty on the given map view. */
void
set_object_dirty (object_t *object,
                  mapview_t *mapview)
{
  g_assert (object && mapview);

  /* If we're already dirty, no need to run this again. */

  if (object->is_dirty == TRUE)
    return;

  /* If the object has no image (the filename is NULL) then ignore the
     dirty request. */
  if (object->image->filename == NULL)
    return;

  /* Ensure the object's co-ordinates don't go over the map
     width/height! */

  g_assert ((object->image->map_x + object->image->width <= mapview->map->width * TILE_W)
            && (object->image->map_y + object->image->height <= mapview->map->height * TILE_H));

  /* And now, the business end. */

  if (object->tag != 0)
    {
      add_object_image (mapview, object->tag, object);
      object->is_dirty = TRUE;
    }
}


void
free_object (void *object)
{
  object_t *objectc = (object_t *) object;
  if (objectc)
    {
      if (objectc->name)
        g_free (objectc->name);

      if (objectc->script_filename)
        g_free (objectc->script_filename);

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
    return;
    
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
