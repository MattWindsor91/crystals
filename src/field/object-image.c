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

/** @file    src/field/object-image.c
 *  @author  Matt Windsor
 *  @brief   Object image operations.
 */


#include <stdlib.h>

#include "object-image.h"
#include "object.h"
#include "../types.h"
#include "../util.h"


/* Set all the parameters of an object image node to default values. */

object_image_t *
init_object_image (object_t *parent)
{
  object_image_t *image;


  /* Sanity checking. */

  if (parent == NULL)
    {
      error ("OBJECT-IMAGE - init_object_image - Object image has no parent.");
      return NULL;
    }

  /* End sanity checking. */

  image = malloc (sizeof (object_image_t));

  if (image == NULL)
     {
       error ("OBJECT-IMAGE - init_object_image - Tried to init null object image.");
       return NULL;
     }


  /* Initialise everything else. */

  image->parent = parent;
  image->filename = NULL;
  image->next = NULL;

  image->image_x = 0;
  image->image_y = 0;
  image->map_x = 0;
  image->map_y = 0;
  image->width = 0;
  image->height = 0;


  return image;
}


/* Free an object image render queue node. */

void
free_object_image (object_image_t *image)
{
  if (image)
    {
      if (image->filename)
        free (image->filename);

      free (image);
    }
}
