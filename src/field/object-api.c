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

/** @file    src/field/object-api.c
 *  @author  Matt Windsor
 *  @brief   Externally exposed API for object manipulation.
 */

#include <stdlib.h>

#include "../util.h"
#include "../graphics.h"

#include "field.h"
#include "object.h"
#include "mapview.h"
#include "object-api.h"


/* -- STATIC GLOBAL VARIABLES -- */

static object_t *sg_camera_focus = NULL; /** Current object with camera focus. */


/* -- DEFINITIONS -- */

/* Set an object as the camera focus point. */

bool_t
focus_camera_on_object (const char object_name[])
{
  object_t *object;

  /* Sanity checking. */

  if (object_name == NULL)
    {
      g_critical ("OBJECT-API - focus_camera_on_object - Null object name.");
      return FAILURE;
    }

  object = get_object (object_name);

  if (object == NULL)
    {
      g_critical ("OBJECT-API - focus_camera_on_object - Couldn't get object %s.", 
             object_name);
      return FAILURE;
    }

  sg_camera_focus = object;

  return SUCCESS;
}


/* Move an object by an offset from its current co-ordinates. */

bool_t
move_object (const char object_name[], int32_t dx, int32_t dy)
{
  object_t *object;
  mapview_t *mapview;


  /* These four ints are used for boundary checking later. */
  
  int32_t start_x;
  int32_t start_y;
  int32_t end_x;
  int32_t end_y;


  /* Sanity checking. */

  if (object_name == NULL)
    {
      g_critical ("OBJECT-API - move_object - Null object name.");
      return FAILURE;
    }

  /* End sanity checking. */


  object = get_object (object_name);

  if (object == NULL)
    {
      g_critical ("OBJECT-API - move_object - Couldn't get object %s.", 
             object_name);
      return FAILURE;
    }

  if (object->image == NULL)
    {
      g_critical ("OBJECT-API - move_object - Object %s has no image dataset.", 
             object->name);
      return FAILURE;
    }

  mapview = get_field_mapview ();

  if (mapview == NULL)
    {
      g_critical ("OBJECT-API - move_object - No field map view.");
      return FAILURE;
    }

  /* No point moving by (0, 0). */

  if (dx == 0 && dy == 0)
    return SUCCESS;

  /* Check we're not moving the object out of bounds. */

  if (get_field_map_boundaries (&start_x, 
                                &start_y,
                                &end_x,
                                &end_y) == FAILURE)
    {
      g_critical ("OBJECT-API - move_object - Couldn't get map boundaries.");
      return FAILURE;
    }

  if (object->image->map_x + dx < start_x
      || object->image->map_y + dy < start_y
      || object->image->map_x + object->image->width + dx >= end_x
      || object->image->map_y + object->image->height + dy >= end_y)
    {
      g_critical ("OBJECT-API - move_object - Tried to move out of bounds.");
      return FAILURE;
    }

  /* Mark old location as dirty. */

  mark_dirty_rect (mapview,
                   object->image->map_x, 
                   object->image->map_y, 
                   object->image->width, 
                   object->image->height);

  /* Try to move object. */

  if (set_object_coordinates (object, 
                              object->image->map_x + dx,
                              object->image->map_y + dy, TOP_LEFT) 
      == FAILURE)
    return FAILURE;

  /* Set object as newly dirty. */

  set_object_dirty (object, mapview);

  /* Finally, if the object is the camera focus, scroll the map (if
     the new co-ordinates are near enough) or, when full map updates
     are ready, centre the camera and redraw the map. */

  if (object == sg_camera_focus)
    {
      /* Assert SCREEN_W and SCREEN_H are always int16_t */
      if (dx <= SCREEN_W 
          || dy <= SCREEN_H
          || dx >= -SCREEN_W
          || dy >= -SCREEN_H)
        scroll_map (mapview, (int16_t) dx, (int16_t) dy);
    }

  return SUCCESS;
}


/* Move an object to a new absolute position. */

bool_t
position_object (const char object_name[], int32_t x, int32_t y,
                 reference_t reference)
{
  object_t *object;
  mapview_t *mapview;

  /* These four ints are used for boundary checking later. */
  
  int start_x;
  int start_y;
  int end_x;
  int end_y;


  /* Sanity checking. */

  if (object_name == NULL)
    {
      g_critical ("OBJECT-API - position_object - Null object name.");
      return FAILURE;
    }

  /* End sanity checking. */


  object = get_object (object_name);

  if (object == NULL)
    {
      g_critical ("OBJECT-API - position_object - Couldn't get object %s.", 
             object_name);
      return FAILURE;
    }

  if (object->image == NULL)
    {
      g_critical ("OBJECT-API - position_object - Object %s has no image dataset.", 
             object->name);
      return FAILURE;
    }

  mapview = get_field_mapview ();

  if (mapview == NULL)
    {
      g_critical ("OBJECT-API - position_object - No field map view.");
      return FAILURE;
    }


  /* Check we're not moving the object out of bounds. */

  if (get_field_map_boundaries (&start_x, 
                                &start_y,
                                &end_x,
                                &end_y) == FAILURE)
    {
      g_critical ("OBJECT-API - position_object - Couldn't get map boundaries.");
      return FAILURE;
    }

  if (x < start_x
      || y < start_y
      || x + object->image->width >= end_x
      || y + object->image->height >= end_y)
    {
      g_critical ("OBJECT-API - position_object - Tried to move out of bounds.");
      return FAILURE;
    }


  /* Mark old location as dirty. */

  mark_dirty_rect (mapview,
                   object->image->map_x, 
                   object->image->map_y, 
                   object->image->width, 
                   object->image->height);


  /* Try to move object. */

  if (set_object_coordinates (object, 
                              x,
                              y, reference) 
      == FAILURE)
    return FAILURE;


  /* Set object as newly dirty. */

  set_object_dirty (object, mapview);

  return SUCCESS;
}


/* Change the tag associated with an object. */

bool_t
tag_object (const char object_name[], layer_tag_t tag)
{
  object_t *object;


  /* Sanity checking. */

  if (object_name == NULL)
    {
      g_critical ("OBJECT-API - tag_object - Null object name.");
      return FAILURE;
    }

  /* End sanity checking. */


  object = get_object (object_name);

  if (object == NULL)
    {
      g_critical ("OBJECT-API - tag_object - Couldn't get object %s.", 
             object_name);
      return FAILURE;
    }

  return set_object_tag (object, tag);
}


/* Change the image associated with an object. */

bool_t
change_object_image (const char object_name[],
                     const char image_filename[], 
                     int16_t x_offset,
                     int16_t y_offset,
                     uint16_t width,
                     uint16_t height)
{
  object_t *object;
  mapview_t *mapview;

  /* These four ints are used for boundary checking later. */
  
  int32_t start_x;
  int32_t start_y;
  int32_t end_x;
  int32_t end_y;


  if (image_filename == NULL)
    {
      g_critical ("OBJECT-API -change_object_image - Null filename.");
      return FAILURE;
    }

  if (object_name == NULL)
    {
      g_critical ("OBJECT-API - change_object_image - Null object name.");
      return FAILURE;
    }

  object = get_object (object_name);

  if (object == NULL)
    {
      g_critical ("OBJECT-API - change_object_image - Couldn't get object %s.", 
             object_name);
      return FAILURE;
    }

  if (object->image == NULL)
    {
      g_critical ("OBJECT-API - change_object_image - Object %s has no image dataset.", 
             object->name);
      return FAILURE;
    }

  mapview = get_field_mapview ();

  if (mapview == NULL)
    {
      g_critical ("OBJECT-API - change_object_image - No field map view.");
      return FAILURE;
    }

  /* Check whether the new image size will push the object out of
     bounds. */

  if (get_field_map_boundaries (&start_x, 
                                &start_y,
                                &end_x,
                                &end_y) == FAILURE)
    {
      g_critical ("OBJECT-API - change_object_image - Couldn't get map boundaries.");
      return FAILURE;
    }

  if (object->image->map_x < start_x
      || object->image->map_y < start_y
      || object->image->map_x + width >= end_x
      || object->image->map_y + height >= end_y)
    {
      g_critical ("OBJECT-API - change_object_image - Tried to move out of bounds.");
      return FAILURE;
    }

  if (set_object_image (object, 
                        image_filename,
                        x_offset,
                        y_offset,
                        width,
                        height) == FAILURE)
    {
      g_critical ("OBJECT-API - change_object_image - Could not set image.");
      return FAILURE;
    }

  return set_object_dirty (object, mapview);
}
