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

/** @file    graphics.h
 *  @author  Matt Windsor
 *  @brief   Prototypes and declarations for generic graphics system.
 */

#ifndef _GRAPHICS_H
#define _GRAPHICS_H

#include "hash.h"    /* Hash stuff. */


enum
  {
    SCREEN_W = 640, /**< Width of the screen (in pixels). 
                       @todo FIXME: Make this changeable at runtime. */

    SCREEN_H = 480, /**< Height of the screen (in pixels).
                       @todo FIXME: Make this changeable at runtime. */

    SCREEN_D = 32   /**< Colour depth of the screen (in bits per
                       pixel.) */
  };

int
init_graphics (void);

void
fill_screen (unsigned char red,
             unsigned char green,
             unsigned char blue);

struct hash_object *
load_image (const char filename[]);

void
free_image (void *image);

int
draw_image (const char filename[],
            short image_x,
            short image_y,
            short screen_x,
            short screen_y,
            unsigned short width,
            unsigned short height);



int
delete_image (const char filename[]);

void
clear_images (void);

struct hash_object *
get_image (const char filename[], struct hash_object *add_pointer);

void
update_screen (void);

void
scroll_screen (unsigned int direction);

void
cleanup_graphics (void);

#endif /* _GRAPHICS_H */
