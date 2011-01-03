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

/** @file    src/field/object-api.h
 *  @author  Matt Windsor
 *  @brief   Prototypes and declarations for exposed object API.
 *
 *  Rules of the object API:
 *
 *  - Objects are referred to in arguments by name.
 *
 *  - Object data must not be manipulated directly.
 */

#ifndef _OBJECT_API_H
#define _OBJECT_API_H

#include "object.h"
#include "map.h"

/* -- PROTOTYPES -- */

/** Set an object as the camera focus point.
 *
 *  This causes movement of the object to move the field map view's
 *  offsets too, so the object is at the centre of the screen.
 *
 *  @return  SUCCESS for success; FAILURE otherwise (eg if the object
 *  doesn't exist).
 */

int
focus_camera_on_object (const char object_name[]);


/** Move an object to a new absolute position.
 *
 *  The reference parameter determines the reference point from which
 *  the object's co-ordinates should be determined.
 *
 *  TOP_LEFT sets the co-ordinates to refer to the top-left of the
 *  image.  BOTTOM_LEFT sets the co-ordinates to refer to the
 *  bottom-left of the image, which in most cases will represent the
 *  actual physical "base" of the object.
 *
 *  @param object_name  Name to the object to move.
 *
 *  @param x            The x co-ordinate, representing the left edge
 *                      of the object, in pixels from the left edge of
 *                      the  map. 
 *
 *  @param y            The y co-ordinate, representing the top or
 *                      bottom edge of the object, in pixels from the
 *                      top edge of the map.
 *
 *  @param reference    The reference point to use (TOP_LEFT or
 *                      BOTTOM_LEFT). In most cases, BOTTOM_LEFT is
 *                      preferred, as the bottom of the image is the
 *                      reference point for Z-order calculation.
 *
 *  @return  SUCCESS for success; FAILURE otherwise (eg if the map
 *  pointer is NULL).
 */ 

int
position_object (const char object_name[], 
                 int x,
                 int y, 
                 unsigned short reference);


/** Move an object by an offset from its current co-ordinates.
 *
 *  @param object_name   Name of the object to move.
 *
 *  @param dx            Change in x co-ordinate.
 *
 *  @param dy            Change in y co-ordinate.
 *
 *  @return  SUCCESS for success; FAILURE otherwise (eg if the object
 *  doesn't exist, or the co-ordinates are out of bounds).
 */

int
move_object (const char object_name[], 
             int dx,
             int dy);


/** Change the tag associated with an object.
 *
 *  @param object_name  Name of the object to change the tag of.
 *
 *  @param tag          The new tag.
 *
 *  @return  SUCCESS for success; FAILURE otherwise (eg if the object
 *  doesn't exist).
 */

int
tag_object (const char object_name[],
            layer_t tag);


/** Change the image associated with an object.
 *
 *  @param object_name     Name of the object to change the image of.
 *
 *  @param image_filename  Name of the file to use for the image.
 *
 *  @param x_offset        X offset of the rectangular area of the
 *                         image file to use as the object image, in
 *                         pixels.
 *
 *  @param y_offset        Y offset of the rectangular area of the
 *                         image file to use as the object image, in
 *                         pixels.
 *
 *  @param width           Width of the rectangular area of the image
 *                         file to use as the object image, in pixels.
 *
 *  @param height          Height of the rectangular area of the image
 *                         file to use as the object image, in
 *                         pixels.
 *
 *  @return  SUCCESS for success; FAILURE otherwise (eg if the object
 *  doesn't exist, or the co-ordinates are out of bounds).
 */

int
change_object_image (const char object_name[],
                     const char image_filename[], 
                     short x_offset,
                     short y_offset,
                     unsigned short width, 
                     unsigned short height);


#endif /* not _OBJECT_API_H */
