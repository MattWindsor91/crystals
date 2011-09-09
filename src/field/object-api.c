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
 * @file    src/field/object-api.c
 * @author  Matt Windsor
 * @brief   Externally exposed API for object manipulation.
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


/* -- STATIC DECLARATIONS -- */

/**
 * Moves an object by that offset from its current co-ordinates.
 *
 * Either or both of the two offset parameters may be negative, which
 * has the effect of moving the object in the opposite direction.
 *
 * @param object_name   Name of the object to move.
 * @param dx            Change in x co-ordinate, in pixels towards the
 *                      rightmost edge of the map.
 * @param dy            Change in y co-ordinate, in pixels towards the
 *                      bottom of the map.
 *
 * @return  SUCCESS for success; FAILURE otherwise (eg if the object
 *          doesn't exist, or the co-ordinates are out of bounds).
 */
static bool_t
move_object_post_check (const char object_name[], 
                        int32_t dx,
                        int32_t dy);

/**
 * Marks an object's drawing rectangle on the field as dirty.
 *
 * @param object  The object whose rectangle is to be marked dirty.
 */
static void
mark_object_field_location_dirty (object_t *object);


/**
 * Given a valid image filename, changes the image associated with an
 * object.
 *
 * @param object_name     Name of the object to change the image of.
 * @param image_filename  Name of the file to use for the image.
 * @param x_offset        X offset of the rectangular area of the
 *                        image file to use as the object image, in
 *                        pixels from the left edge of the image
 *                        in the file provided.
 * @param y_offset        Y offset of the rectangular area of the
 *                        image file to use as the object image, in
 *                        pixels from the top edge of the image in
 *                        the file provided.
 * @param width           Width of the rectangular area of the image
 *                        file to use as the object image, in pixels.
 * @param height          Height of the rectangular area of the image
 *                        file to use as the object image, in
 *                        pixels.
 *
 * @return  SUCCESS for success; FAILURE otherwise (eg if the object
 *          doesn't exist, or the co-ordinates are out of bounds).
 */
static bool_t
change_object_image_post_check (const char object_name[],
                                const char image_filename[], 
                                int16_t x_offset,
                                int16_t y_offset,
                                uint16_t width,
                                uint16_t height);


/**
 * Checks whether the given rectangle is outside the map bounds.
 *
 * @param x       The X co-ordinate of the left side of the rectangle,
 *                in pixels from the left edge of the map.
 * @param y       The Y co-ordinate of the top side of the rectangle,
 *                in pixels from the top edge of the map.
 * @param width   The width of the rectangle, in pixels.
 * @param height  The height of the rectangle, in pixels.
 *
 * @return TRUE if the rectangle is out of bounds, FALSE if not.
 */
static bool_t
rectangle_out_of_bounds (int x, int y, int width, int height);


/**
 * Moves the field camera by an offset.
 *
 * Either or both of the two offset parameters may be negative, which
 * has the effect of moving the object in the opposite direction.
 *
 * @param dx            Change in x co-ordinate, in pixels towards the
 *                      rightmost edge of the map.
 * @param dy            Change in y co-ordinate, in pixels towards the
 *                      bottom of the map.
 *
 * @return  SUCCESS if the camera was moved successfully;
 *          FAILURE otherwise.
 */
static bool_t
move_field_camera (int32_t dx, int32_t dy);


/* -- DEFINITIONS -- */

/* Set an object as the camera focus point. */
bool_t
focus_camera_on_object (const char object_name[])
{
  object_t *object = get_object (object_name);
  if (object == NULL)
    {
      error ("OBJECT-API - focus_camera_on_object - Couldn't get object %s.", 
             object_name);
      return FAILURE;
    }

  sg_camera_focus = object;

  return SUCCESS;
}


/* Given a non-zero offset, moves an object by that offset from its 
 * current co-ordinates.
 */
bool_t
move_object (const char object_name[], int32_t dx, int32_t dy)
{
  /* No point moving by (0, 0). */
  if (dx == 0 && dy == 0)
    {
      return SUCCESS;
    }

  return move_object_post_check (object_name, dx, dy);
}


/* Move an object by an offset from its current co-ordinates. */
static bool_t
move_object_post_check (const char object_name[], int32_t dx, int32_t dy)
{
  object_t *object = get_object (object_name);
  if (object == NULL)
    {
      error ("OBJECT-API - move_object - Couldn't get object %s.", 
             object_name);
      return FAILURE;
    }
  else if (object->image == NULL)
    {
      error ("OBJECT-API - move_object - Object %s has no image dataset.", 
             object->name);
      return FAILURE;
    }
  else if (rectangle_out_of_bounds (object->image->map_x + dx,
                                    object->image->map_y + dy,
                                    (object->image->map_x + dx
                                     + object->image->width), 
                                    (object->image->map_y + dy
                                     + object->image->height)))
    {
      error ("OBJECT-API - move_object - Object moving out of bounds.");
      return FAILURE;
    }
    
  /* Mark old location as dirty. */
  mark_object_field_location_dirty (object);

  /* Try to move object. */
  if (set_object_coordinates (object, 
                              object->image->map_x + dx,
                              object->image->map_y + dy, TOP_LEFT) 
      == FAILURE)
    return FAILURE;

  /* Mark new location as dirty. */
  mark_object_field_location_dirty (object);

  if (object == sg_camera_focus)
    {
      return move_field_camera (dx, dy);
    }
  else
    {
      return SUCCESS;
    }
}


/* Moves the camera by the given offset. */
static bool_t
move_field_camera (int32_t dx, int32_t dy)
{
  mapview_t *mapview = get_field_mapview ();
  if (mapview == NULL)
    {
      error ("OBJECT-API - move_object - No field map view.");
      return FAILURE;
    }

  /* Assert SCREEN_W and SCREEN_H are always int16_t */
  if (dx <= SCREEN_W 
      || dy <= SCREEN_H
      || dx >= -SCREEN_W
      || dy >= -SCREEN_H)
    {
      scroll_map (mapview, (int16_t) dx, (int16_t) dy);
      return SUCCESS;
    }

  return FAILURE;
}


/* Move an object to a new absolute position. */
bool_t
position_object (const char object_name[],
                 int32_t x,
                 int32_t y,
                 reference_t reference)
{
  object_t *object = get_object (object_name);
  if (object == NULL)
    {
      error ("OBJECT-API - position_object - Couldn't get object %s.", 
             object_name);
      return FAILURE;
    }
  else if (object->image == NULL)
    {
      error ("OBJECT-API - position_object - Object %s has no image dataset.", 
             object->name);
      return FAILURE;
    }
  else if (rectangle_out_of_bounds (x,
                                    y,
                                    x + object->image->width, 
                                    y + object->image->height))
    {
      error ("OBJECT-API - position_object - Object moving out of bounds.");
      return FAILURE;
    }

  /* Mark old location as dirty. */
  mark_object_field_location_dirty (object);
  
  /* Try to move object. */
  if (set_object_coordinates (object, 
                              x,
                              y, reference) 
      == FAILURE)
    return FAILURE;

  /* Mark new location as dirty. */
  mark_object_field_location_dirty (object);

  return SUCCESS;
}


/* Change the tag associated with an object. */
bool_t
tag_object (const char object_name[], layer_tag_t tag)
{
  object_t *object = get_object (object_name);
  if (object == NULL)
    {
      error ("OBJECT-API - tag_object - Couldn't get object %s.", 
             object_name);
      return FAILURE;
    }
  
  /* Mark location as dirty - object may have moved up or down. */
  mark_object_field_location_dirty (object);

  return set_object_tag (object, tag);
}


/* Changes the image associated with an object. */
bool_t
change_object_image (const char object_name[],
                     const char image_filename[], 
                     int16_t x_offset,
                     int16_t y_offset,
                     uint16_t width,
                     uint16_t height)
{
  if (image_filename == NULL)
    {
      error ("OBJECT-API -change_object_image - Null filename.");
      return FAILURE;
    }

  return change_object_image_post_check (object_name,
                                         image_filename,
                                         x_offset,
                                         y_offset,
                                         width,
                                         height);
}

/* Given a valid image filename, changes the image associated with an
 * object.
 */
static bool_t
change_object_image_post_check (const char object_name[],
                                const char image_filename[], 
                                int16_t x_offset,
                                int16_t y_offset,
                                uint16_t width,
                                uint16_t height)
{
  object_t *object = get_object (object_name);
  if (object == NULL)
    {
      error ("OBJECT-API - change_object_image - Couldn't get object %s.", 
             object_name);
      return FAILURE;
    }
  else if (object->image == NULL)
    {
      error ("OBJECT-API - change_object_image - Object %s has no image dataset.", 
             object->name);
      return FAILURE;
    }
  else if (rectangle_out_of_bounds (object->image->map_x,
                                    object->image->map_y,
                                    object->image->map_x + width, 
                                    object->image->map_y + height))
    {
      error ("OBJECT-API - change_object_image - Object moving out of bounds.");
      return FAILURE;
    }

  if (set_object_image (object, 
                        image_filename,
                        x_offset,
                        y_offset,
                        width,
                        height) == FAILURE)
    {
      error ("OBJECT-API - change_object_image - Could not set image.");
      return FAILURE;
    }
  
  mark_object_field_location_dirty (object);
      
  return SUCCESS;
}


/* Marks an object's drawing rectangle on the field as dirty. */
static void
mark_object_field_location_dirty (object_t *object)
{
  mapview_t *mapview = get_field_mapview ();
  if (mapview == NULL)
    {
      error ("OBJECT-API - position_object - No field map view.");
      return;
    }

  /* No point marking a dirty rectangle if the object is currently
   * invisible.
   */
  if (object->image->width == 0
      || object->image->height == 0)
    {
      return;
    }

  mark_dirty_rect (mapview,
                   object->image->map_x, 
                   object->image->map_y, 
                   object->image->width, 
                   object->image->height);  
}


/* Checks whether the given rectangle is outside the map bounds. */
static bool_t
rectangle_out_of_bounds (int x, int y, int width, int height)
{
  int start_x;
  int start_y;
  int end_x;
  int end_y;

  if (get_field_map_boundaries (&start_x, 
                                &start_y,
                                &end_x,
                                &end_y) == FAILURE)
    {
      error ("OBJECT-API - position_object - Couldn't get map boundaries.");
      return TRUE;
    }

  if (x < start_x
      || y < start_y
      || x + width >= end_x
      || y + height >= end_y)
    {
      return TRUE;
    }
  else
    {
      return FALSE;
    }
}
