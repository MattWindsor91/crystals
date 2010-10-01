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

/** @file    object.h
 *  @author  Matt Windsor
 *  @brief   Prototypes and declarations for low-level object
 *           functions.
 */

#ifndef _OBJECT_H
#define _OBJECT_H

#include <stdarg.h>

#include "hash.h"    /* Hash stuff. */
#include "map.h"     /* layer_t */
#include "mapview.h" /* struct object_image, struct map_view */

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

struct object_t
{
  char *name;                 /**< Descriptive name of the object
                                 (must be unique).
                                 @todo FIXME: add code to check
                                 duplicates. */

  char *script_filename;      /**< Filename of the script associated with
                                 the object. */
  
  layer_t tag;                /**< The tag on which to render. */

  char is_dirty;              /**< Boolean determining whether or not
                                 the object is "dirty" (should be
                                 rendered this frame). */

  struct object_image *image; /**< Pointer to the object's associated
                                 image data. */
};

/* -- GLOBAL VARIABLES -- */

extern struct hash_object *g_objects[HASH_VALS]; /**< The object hash
                                                    table. */

/* -- PROTOTYPES -- */

/** Initialise the object base.
 *
 *  @return SUCCESS for success, FAILURE otherwise.
 */

int
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

struct object_t *
add_object (const char object_name[],
            const char script_filename[]);

/** Change the layer tag associated with an object.
 *
 *  @param object  Pointer to the object whose tag is to be changed.
 *
 *  @param tag  The tag to associate with the object.
 */

int
set_object_tag (struct object_t *object,
                layer_t tag);

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
 *  @param map_x     X co-ordinate of the left edge of the on-map
 *                   rectangle in which to render the image, in
 *                   pixels.
 *
 *  @param map_y     Y co-ordinate of the top edge of the on-map    
 *                   rectangle in which to render the image, in
 *                   pixels.
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

int
set_object_image (struct object_t *object, 
                  const char filename[],
                  short image_x,
                  short image_y,
                  int map_x,
                  int map_y,
                  unsigned short width,
                  unsigned short height);

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

int
set_object_coordinates (struct object_t *object, 
                        int x, 
                        int y,
                        unsigned short reference);

/** Mark an object as being dirty on the given map view.
 *
 *  @param object   Pointer to the object to render.
 *  @param mapview  Pointer to the map view on which to render the
 *                  object.
 *  
 *  @return  SUCCESS for success; FAILURE otherwise (eg if either
 *  pointer is NULL).
 */ 

int
set_object_dirty (struct object_t *object, 
                  struct map_view *mapview);

/** Delete an object and all associated data.
 *
 *  @param object  Pointer to the object to delete.
 */

void
free_object (struct object_t *object);

/** Remove an object from the object table.
 *
 *  @param object_name  The name of the object to remove from the
 *  table.
 *
 *  @return  SUCCESS for success; FAILURE otherwise (eg if no object
 *  was deleted).
 */ 

int
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

struct object_t *
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

int
dirty_object_test (struct hash_object *hash_object, void *rect_pointer);

/** Clean up the objects subsystem. */

void
cleanup_objects (void);

#endif /* not _GRAPHICS_H */
