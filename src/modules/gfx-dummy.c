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
 * @file    src/modules/gfx-dummy.c
 * @author  Alexander Preisinger
 * @brief   Dummy graphics backend.
 */


#include "gfx-module.h"  /* Module header file. */
#include "../util.h"     /* Constants. */
#include "../types.h"    /* Type definitions. */


/* -- DEFINITIONS -- */

/* Initialise the module. */

EXPORT bool_t
init (void)
{
  return SUCCESS;
}


/* Terminate the module, freeing any remaining data dynamically
   allocated by the module. */

EXPORT void
term (void)
{
}


/* Initialise a screen of a given width, height and depth. */

EXPORT bool_t
init_screen_internal (uint16_t width,
                      uint16_t height,
                      uint8_t depth)
{
  (void) width;
  (void) height;  
  (void) depth;  


  return SUCCESS;
}


/* Draw a rectangle of colour on-screen. */

EXPORT bool_t
draw_rect_internal (int16_t x,
                    int16_t y,
                    uint16_t width,
                    uint16_t height,
                    uint8_t red,
                    uint8_t green,
                    uint8_t blue)
{
  (void) x;
  (void) y;
  (void) width;
  (void) height;
  (void) red;
  (void) green;
  (void) blue;


  return SUCCESS;
}


/* Load an image and return its data in the module's native format. */

EXPORT void *
load_image_data (const char filename[])
{ 
  (void) filename;


  return (void *) 1; /* pretend you didn't see it */
}


/* Free image data retrieved by load_image_data. */

EXPORT bool_t
free_image_data (void *data)
{
  (void) data;


  return SUCCESS;
}


/* Draw a rectangular portion of an image on-screen. */

EXPORT bool_t
draw_image_internal (void *image,
                     int16_t image_x,
                     int16_t image_y,
                     int16_t screen_x,
                     int16_t screen_y,
                     uint16_t width,
                     uint16_t height)
{
  (void) image;
  (void) image_x;
  (void) image_y;
  (void) screen_x;
  (void) screen_y;
  (void) width;
  (void) height;


  return SUCCESS;
}


/* Update the screen. */

EXPORT bool_t
update_screen_internal (void)
{
  return SUCCESS;
}


/* Translate the screen by a co-ordinate pair, leaving damage. */

EXPORT bool_t
scroll_screen_internal (int16_t x_offset, int16_t y_offset)
{
  (void) x_offset;
  (void) y_offset;


  return SUCCESS;
}
