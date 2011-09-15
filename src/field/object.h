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
 * @file    src/field/object.h
 * @author  Matt Windsor
 * @brief   Prototypes and declarations for low-level object
 *          functions.
 */

#ifndef _OBJECT_H
#define _OBJECT_H


/* -- CONSTANTS -- */

typedef enum reference_point
{
  TOP_LEFT = 0,	    /**< Top-left of image reference point. */
  BOTTOM_LEFT = 1   /**< Bottom-left of image reference point. */
} reference_point_t;


/* -- STRUCTURES -- */

/**
 * An object node in the object hash table.
 *
 * This will inevitably have to be changed!
 */
typedef struct object
{
  char *name;		      /**< Descriptive name of the object
                                 (must be unique).
                                 @todo FIXME: add code to check
                                 duplicates. */

  char *script_filename;      /**< Filename of the script associated with
                                 the object. */

  layer_tag_t tag;	      /**< The tag on which to render. */

  bool_t is_dirty;	      /**< Boolean determining whether or not
                                 the object is "dirty" (should be
                                 rendered this frame). */

  object_image_t *image;      /**< Pointer to the object's associated
                                 image data. */
} object_t;


/* -- DECLARATIONS-- */

/**
 * Change the layer tag associated with an object.
 *
 * @param object  Pointer to the object whose tag is to be changed.
 * @param tag     The tag to associate with the object.
 */
void set_object_tag (object_t *object, layer_tag_t tag);


/**
 * Get the graphic associated with an object.
 *
 * @param object  Pointer to the object to query.
 *
 * @return  The object image, or NULL if an error occurred.
 */
struct object_image *get_object_image (object_t *object);


/**
 * Change the graphic associated with an object.
 *
 * This will instantly update the object image.
 *
 * @note  In order to make an object have no physical presence on the
 *        map, change its tag to 0.
 *
 * @param object    Pointer to the object to change.
 * @param filename  Filename of the image.
 * @param image_x   X co-ordinate of the left edge of the on-image
 *                  rectangle from which to source the rendered
 *                  image, in pixels.
 *
 * @param image_y   Y co-ordinate of the top edge of the on-image
 *                  rectangle from which to source the rendered
 *                  image, in pixels.
 *
 * @param width     Width of the image rectangle to render, in
 *                  pixels.
 *
 * @param height    Height of the image rectangle to render, in
 *                  pixels.
 */
void
set_object_image (object_t *object,
		  const char filename[],
		  int16_t image_x,
		  int16_t image_y, uint16_t width, uint16_t height);


/**
 * Retrieve the object's co-ordinates on-map.
 *
 * The reference parameter determines the reference point from which
 * the object's co-ordinates should be determined.
 *
 * TOP_LEFT sets the co-ordinates to refer to the top-left of the
 * image.  BOTTOM_LEFT sets the co-ordinates to refer to the
 * bottom-left of the image, which in most cases will represent the
 * actual physical "base" of the object.
 *
 * @param object     Pointer to the object to query.
 * @param x_pointer  Pointer to a variable in which to store the X
 *                   co-ordinate.
 * @param y_pointer  Pointer to a variable in which to store the Y
 *                   co-ordinate.
 * @param reference  The reference point to use (TOP_LEFT or
 *                   BOTTOM_LEFT). In most cases, BOTTOM_LEFT is
 *                   preferred, as the bottom of the image is the
 *                   reference point for Z-order calculation.
 */
void get_object_coordinates (object_t *object,
			     int32_t *x_pointer,
			     int32_t *y_pointer,
			     reference_point_t reference);


/**
 * Sets the object's co-ordinates on-map.
 *
 * The reference parameter determines the reference point from which
 * the object's co-ordinates should be determined.
 *
 * TOP_LEFT sets the co-ordinates to refer to the top-left of the
 * image.  BOTTOM_LEFT sets the co-ordinates to refer to the
 * bottom-left of the image, which in most cases will represent the
 * actual physical "base" of the object.
 *
 * @param object     Pointer to the object to modify.
 * @param x          The x co-ordinate, representing the left edge of
 *                   the object, in pixels from the left edge of the
 *                   map.
 * @param y          The y co-ordinate, representing the top or
 *                   bottom edge of the object, in pixels from the
 *                   top edge of the map.
 * @param reference  The reference point to use (TOP_LEFT or
 *                   BOTTOM_LEFT). In most cases, BOTTOM_LEFT is
 *                   preferred, as the bottom of the image is the
 *                   reference point for Z-order calculation.
 */
void set_object_coordinates (object_t *object,
			     int32_t x, int32_t y,
			     reference_point_t reference);


/**
 * Marks an object as being dirty on the given map view.
 *
 * @param object   Pointer to the object to render.
 * @param mapview  Pointer to the map view on which to render the
 *                 object.
 */
void set_object_dirty (object_t *object, struct mapview *mapview);


/**
 * Deletes an object and all associated data.
 *
 * @param object  Pointer to the object to delete.
 */
void free_object (object_t *object);


/**
 * Check to see whether the given object falls within the given dirty
 * rectangle and, if so, mark the object as dirty (if it has not been
 * marked before).
 *
 * @param key_ptr     Pointer to the (unused) object name.
 * @param object_ptr  Pointer to the object to test.
 * @param rect_ptr    Pointer to the dirty rectangle to test.
 */
void dirty_object_test (gpointer key_ptr,
			gpointer object_ptr, gpointer rect_ptr);


#endif /* not _OBJECT_H */
