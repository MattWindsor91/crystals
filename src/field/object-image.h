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
 * @file    src/field/object-image.h
 * @author  Matt Windsor
 * @brief   Prototypes and declarations for object images.
 */


#ifndef _OBJECT_IMAGE_H
#define _OBJECT_IMAGE_H


#include "../types.h"


/* -- TYPEDEFS -- */

/** An object image. */

typedef struct object_image
{
  char *filename;        /**< Name of the image, used when looking up
                            the image in the image cache. */

  int16_t image_x;       /**< X co-ordinate of the left edge of the
                            on-image rectangle from which to source
                            the rendered image, in pixels. */

  int16_t image_y;       /**< Y co-ordinate of the top edge of the
                            on-image rectangle from which to source
                            the rendered image, in pixels. */

  int32_t map_x;         /**< X co-ordinate of the left edge of the
                            on-map rectangle in which to render the
                            image, in pixels. */

  int32_t map_y;         /**< Y co-ordinate of the top edge of the
                            on-map rectangle in which to render the
                            image, in pixels. */

  unsigned short width;  /**< Width of the object image, in pixels. */

  unsigned short height; /**< Height of the object image, in
                            pixels. */

  struct object *parent; /**< Pointer to the object parent, if any. */

  struct object_image *next; /**< Next node in the queue. */

} object_image_t;


/* -- DECLARATIONS -- */


/**
 * Allocate and initialise an object image.
 *
 * @param parent  Pointer to the parent object of the object image.
 *
 * @return the new object image, or NULL if an error occurred.
 */

object_image_t *
init_object_image (struct object *parent);


/**
 * Free an object image render queue node.
 *
 * @param image  Pointer to the render node to free.
 */

void
free_object_image (object_image_t *image);


#endif /* not _OBJECT_IMAGE_H */
