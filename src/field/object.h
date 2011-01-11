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

/** @file    src/field/object.h
 *  @author  Matt Windsor
 *  @brief   Prototypes and declarations for low-level object
 *           functions.
 */

#ifndef _OBJECT_H
#define _OBJECT_H

#include "../types.h" /* Types. */
#include "../hash.h"  /* Hash stuff. */
#include "map.h"      /* layer_value_t */
#include "mapview.h"  /* struct object_image, struct map_view */


/* -- TYPEDEFS -- */

typedef uint8_t reference_t; /**< Reference constant type. */

/* -- CONSTANTS -- */

enum
  {
    TOP_LEFT = 0,   /**< Top-left of image reference point. */
    BOTTOM_LEFT = 1 /**< Bottom-left of image reference point. */
  };


/* -- STRUCTURES -- */

/** An object node in the object hash table.
 *
 *  This will inevitably have to be changed!
 */

typedef struct object
{
  char *name;                 /**< Descriptive name of the object
                                 (must be unique).
                                 @todo FIXME: add code to check
                                 duplicates. */

  char *script_filename;      /**< Filename of the script associated with
                                 the object. */
  
  layer_tag_t tag;            /**< The tag on which to render. */

  bool_t is_dirty;            /**< Boolean determining whether or not
                                 the object is "dirty" (should be
                                 rendered this frame). */

  struct object_image *image; /**< Pointer to the object's associated
                                 image data. */
} object_t;


/* -- GLOBAL VARIABLES -- */

extern struct hash_object *g_objects[HASH_VALS]; /**< The object hash
                                                    table. */

/* -- DECLARATIONS-- */

/** Initialise the object base.
 *
 *  @return SUCCESS for success, FAILURE otherwise.
 */

bool_t
init_objects (void);


/** Create a new object and add it to the object table.
 *
 *  @param object_name      The name of the object, used to look it up
 *                          in the object table.
 *
 *  @param script_filename  The filename of the script to associate
 *                          with the object. This script's
 *                          initialisation code will be called once
 *                          the object is installed.
 *
 *  @return  a pointer to the new object, or NULL if there was an
 *           error while creating it.
 */

object_t *
add_object (const char object_name[],
            const char script_filename[]);


/** Change the layer tag associated with an object.
 *
 *  @param object  Pointer to the object whose tag is to be changed.
 *
 *  @param tag     The tag to associate with the object.
 *
 *  @return  SUCCESS if no errors occurred; FAILURE otherwise.
 */

bool_t
set_object_tag (object_t *object, layer_tag_t tag);


/** Get the graphic associated with an object.
 *
 *  @param object  Pointer to the object to query.
 *
 *  @return  The object image, or NULL if an error occurred.
 */

struct object_image *
get_object_image (object_t *object);


/** Change the graphic associated with an object.
 *
 *  This will instantly update the object image.
 *
 *  @note  In order to make an object have no physical presence on the
 *         map, change its tag to 0.
 *
 *  @param object    Pointer to the object to change.
 *
 *  @param filename  Filename of the image.
 *
 *  @param image_x   X co-ordinate of the left edge of the on-image
 *                   rectangle from which to source the rendered
 *                   image, in pixels.
 *
 *  @param image_y   Y co-ordinate of the top edge of the on-image
 *                   rectangle from which to source the rendered
 *                   image, in pixels.
 *
 *  @param width     Width of the image rectangle to render, in
 *                   pixels.
 *
 *  @param height    Height of the image rectangle to render, in
 *                   pixels.
 *
 *  @return SUCCESS if there were no errors encountered; FAILURE
 *  otherwise.
 */

bool_t
set_object_image (object_t *object,
                  const char filename[],
                  int16_t image_x,
                  int16_t image_y,
                  uint16_t width,
                  uint16_t height);


/** Retrieve the object's co-ordinates on-map.
 *
 *  The reference parameter determines the reference point from which
 *  the object's co-ordinates should be determined.
 *
 *  TOP_LEFT sets the co-ordinates to refer to the top-left of the
 *  image.  BOTTOM_LEFT sets the co-ordinates to refer to the
 *  bottom-left of the image, which in most cases will represent the
 *  actual physical "base" of the object.
 *
 *  @param object  Pointer to the object to query.
 *
 *  @param x_pointer  Pointer to a variable in which to store the X
 *                    co-ordinate.
 *
 *  @param y_pointer  Pointer to a variable in which to store the Y
 *                    co-ordinate.
 *
 *  @param reference  The reference point to use (TOP_LEFT or
 *                    BOTTOM_LEFT). In most cases, BOTTOM_LEFT is
 *                    preferred, as the bottom of the image is the
 *                    reference point for Z-order calculation.
 *
 *  @return  SUCCESS for success; FAILURE otherwise (eg if the map
 *  pointer is NULL).
 */ 

bool_t
get_object_coordinates (object_t *object,
                        int32_t *x_pointer,
                        int32_t *y_pointer,
                        reference_t reference);


/** Set the object's co-ordinates on-map.
 *
 *  The reference parameter determines the reference point from which
 *  the object's co-ordinates should be determined.
 *
 *  TOP_LEFT sets the co-ordinates to refer to the top-left of the
 *  image.  BOTTOM_LEFT sets the co-ordinates to refer to the
 *  bottom-left of the image, which in most cases will represent the
 *  actual physical "base" of the object.
 *
 *  @param object  Pointer to the object to modify.
 *
 *  @param x          The x co-ordinate, representing the left edge of
 *                    the object, in pixels from the left edge of the
 *                    map. 
 *
 *  @param y          The y co-ordinate, representing the top or
 *                    bottom edge of the object, in pixels from the
 *                    top edge of the map.
 *
 *  @param reference  The reference point to use (TOP_LEFT or
 *                    BOTTOM_LEFT). In most cases, BOTTOM_LEFT is
 *                    preferred, as the bottom of the image is the
 *                    reference point for Z-order calculation.
 *
 *  @return  SUCCESS for success; FAILURE otherwise (eg if the map
 *  pointer is NULL).
 */ 

bool_t
set_object_coordinates (object_t *object,
                        int32_t x,
                        int32_t y,
                        reference_t reference);


/** Mark an object as being dirty on the given map view.
 *
 *  @param object   Pointer to the object to render.
 *  @param mapview  Pointer to the map view on which to render the
 *                  object.
 *  
 *  @return  SUCCESS for success; FAILURE otherwise (eg if either
 *  pointer is NULL).
 */ 

bool_t
set_object_dirty (object_t *object,
                  struct map_view *mapview);


/** Delete an object and all associated data.
 *
 *  @param object  Pointer to the object to delete.
 */

void
free_object (object_t *object);


/** Remove an object from the object table.
 *
 *  @param object_name  The name of the object to remove from the
 *  table.
 *
 *  @return  SUCCESS for success; FAILURE otherwise (eg if no object
 *  was deleted).
 */ 

bool_t
delete_object (const char object_name[]);


/** Deletes all objects in the object table. */

void
clear_objects (void);


/** Retrieve an object, or add an object to the object table.
 *
 *  @param object_name  The name of the object concerned. 
 *
 *  @param add_pointer  If this is non-NULL, the function will attempt
 *                      to add the object pointed to by this function
 *                      to the object table.  Otherwise, the function
 *                      will try to find existing objects with the
 *                      given name.
 *
 *  @return  A pointer to the object with the given name if found or
 *  created, or NULL otherwise.
 */

object_t *
get_object (const char object_name[], struct hash_object *add_pointer);


/** Check to see whether the given object falls within the given dirty
 *  rectangle and, if so, mark the object as dirty.
 *
 *  @param hash_object   The hash container of the object to test.
 *
 *  @param rect_pointer  Void pointer to the dirty rectangle to test.
 *
 *  @return SUCCESS if there were no errors encountered; FAILURE
 *  otherwise.
 */

bool_t
dirty_object_test (struct hash_object *hash_object, void *rect_pointer);


/** Apply the given function to all objects.
 *
 *  @param function  Pointer to the function to apply to the
 *                   object.  The function must take the hash object
 *                   as first parameter followed by a void pointer for
 *                   data, and return a SUCCESS/FAILURE bool_t.
 *
 *  @param data      Void pointer to the data to pass to the function.
 *
 *  @return SUCCESS if all applications of the function succeeded,
 *  FAILURE otherwise.
 */

bool_t
apply_to_objects (bool_t (*function) (hash_object_t *object,
                                      void *data),
                  void *data);


/** Clean up the objects subsystem. */

void
cleanup_objects (void);


#endif /* not _OBJECT_H */
