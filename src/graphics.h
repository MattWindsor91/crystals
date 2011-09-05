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
 * @file    graphics.h
 * @author  Matt Windsor
 * @brief   Prototypes and declarations for generic graphics system.
 */


#ifndef _GRAPHICS_H
#define _GRAPHICS_H


/* -- INCLUDES -- */

#include "hash.h"  /* Hash stuff. */
#include "types.h" /* Types */


/* -- TYPEDEFS -- */

typedef void image_t;         /**< Generic image data type. */
typedef uint8_t alignment_t;  /**< Text alignment modifier type. */


/* -- CONSTANTS -- */

enum
  {
    ALIGN_LEFT   = 0,   /**< Left alignment for text. */
    ALIGN_CENTRE = 1,   /**< Centre alignment for text. */
    ALIGN_RIGHT  = 2,   /**< Right alignment for text. */

    SCREEN_W     = 640, /**<
                         * Width of the screen (in pixels).
                         *
                         * @todo FIXME: Make this changeable at runtime.
                         */

    SCREEN_H     = 480, /**<
                         * Height of the screen (in pixels).
                         *
                         * @todo FIXME: Make this changeable at runtime.
                         */

    SCREEN_D     = 32   /**< Screen colour depth (in bits per pixel). */
  };


extern const char DEFGFXPATH[];     /**<
                                     * Default root path for graphics,
                                     * to be invoked if the root path
                                     * cannot be found in the
                                     * configuration file.
                                     */

extern const char FONT_FILENAME[];  /**< Filename of the default font. */

extern const uint16_t FONT_W;        /**< Font character width, in pixels. */

extern const uint16_t FONT_H;        /**< Font character height, in pixels. */


/* -- DECLARATIONS -- */

/**
 * Initialise the graphics subsystem.
 *
 * @return  SUCCESS if the graphics subsystem was initialised
 *          successfully; FAILURE otherwise.
 */

bool_t
init_graphics (void);


/**
 * Given a relative path to an image file, append the graphics root
 * path to it and store it in the given pointer.
 *
 * @param path  The relative path to the image file, for example
 *              "bob.png".
 *
 * @return      The absolute path to the image file, for example
 *              "/usr/share/crystals/gfx/bob.png".
 */

char *
get_absolute_path (const char path[]);


/**
 * Write a string on the screen, using the standard font.
 *
 * A wrapper to the image drawing functions that allows text to be
 * left, centre, or right-aligned on a line of length box_width
 * starting at (x, y).
 *
 * @param x          X position of text.
 * @param y          Y position of text.
 * @param box_width  Width of line to align text on. This need only be given
 *                   for centre or right-aligned text.
 * @param alignment  Desired alignment (ALIGN_LEFT, ALIGN_CENTRE or
 *                   ALIGN_RIGHT).
 * @param string     The string to write.
 */

void
write_string (int16_t x, int16_t y, uint16_t box_width, alignment_t alignment,
              const char string[]);


/**
 * Fill the screen with the given colour.
 *
 * Depending on the graphics module, the colour displayed on screen
 * may not exactly match the desired colour.
 *
 * @param red    The red component of the fill colour (0-255).
 *
 * @param green  The green component of the fill colour (0-255).
 *
 * @param blue   The blue component of the fill colour (0-255).
 *
 * @return       SUCCESS for success; FAILURE otherwise.
 */

bool_t
fill_screen (uint8_t red, uint8_t green, uint8_t blue);


/**
 * Translate the screen by a co-ordinate pair, leaving damage.
 *
 * @param x_offset  The X co-ordinate offset in which to scroll the
 *                  screen, in pixels towards the right.
 *
 * @param y_offset  The Y co-ordinate offset in which to scroll the
 *                  screen, in pixels downwards.
 *
 * @return          SUCCESS for success; FAILURE otherwise.
 */

bool_t
scroll_screen (int16_t x_offset, int16_t y_offset);


/**
 * Load an image.
 *
 * This does not need to be called directly in normal circumstances,
 * as draw_image automatically loads its requested image if it is not
 * present in the image cache.  Still, this function may be useful on
 * its own for pre-caching images, especially on systems with
 * noticeably slow disk access.
 *
 * @note            It is safe to directly use the pointer returned,
 *                  for example to pass its data member to
 *                  draw_image_direct, so long as the image is
 *                  known to still be loaded.  This can be used to
 *                  speed up successive drawing calls after an image
 *                  load check.
 *
 * @param filename  The filename of the image to load. relative from
 *                  the graphics path.
 *
 * @return          a hash_object encapsulating the image data if
 *                  successfully loaded and stored in the image cache;
 *                  NULL otherwise.
 */

image_t *
load_image (const char filename[]);


/**
 * Free image data.
 *
 * This is, at time of writing, merely a wrapper for the driver
 * free_image_data function.
 *
 * @param image  Pointer to the image data to free.
 */

void
free_image (image_t *image);


/**
 * Draw a rectangular portion of an image on-screen.
 *
 * This will pre-load the image into the cache, if it does not
 * already exist there.
 *
 * @param filename  The filename of the image.
 *
 * @param image_x   The X-coordinate of the left edge of the
 *                  on-image rectangle to display.
 *
 * @param image_y   The Y-coordinate of the top edge of the
 *                  on-image rectangle to display.
 *
 * @param screen_x  The X-coordinate of the left edge of the
 *                  on-screen rectangle to place the image in.
 *
 * @param screen_y  The Y-coordinate of the top edge of the
 *                  on-screen rectangle to place the image in.
 *
 * @param width     The width of the rectangle.
 *
 * @param height    The height of the rectangle.
 *
 * @return          SUCCESS for success, FAILURE otherwise.  In most
 *                  cases, a failure will simply cause the image to not
 *                  appear.
 */

bool_t
draw_image (const char filename[], int16_t image_x, int16_t image_y,
            int16_t screen_x, int16_t screen_y, uint16_t width,
            uint16_t height);


/**
 * Draw a rectangular portion of an image on-screen, using a direct
 * pointer to the driver-specific image data.
 *
 * @param data      The void pointer to the image data.
 *
 * @param image_x   The X-coordinate of the left edge of the
 *                  on-image rectangle to display.
 *
 * @param image_y   The Y-coordinate of the top edge of the
 *                  on-image rectangle to display.
 *
 * @param screen_x  The X-coordinate of the left edge of the
 *                  on-screen rectangle to place the image in.
 *
 * @param screen_y  The Y-coordinate of the top edge of the
 *                  on-screen rectangle to place the image in.
 *
 * @param width     The width of the rectangle.
 *
 * @param height    The height of the rectangle.
 *
 * @return          SUCCESS for success, FAILURE otherwise. In most
 *                  cases, a failure will simply cause the image to not
 *                  appear.
 */

bool_t
draw_image_direct (void *data, int16_t image_x, int16_t image_y,
                   int16_t screen_x, int16_t screen_y, uint16_t width,
                   uint16_t height);


/**
 * Delete an image previously loaded into the image cache.
 *
 * @param filename  Filename of the image.
 *
 * @return          SUCCESS if the deletion succeeded; FAILURE otherwise.
 */

bool_t
delete_image (const char filename[]);


/** Delete all images in the image cache. */

void
clear_images (void);


/**
 * Retrieve an image from the image cache.
 *
 * @param filename  The filename of the image to retrieve.
 *
 * @return          A pointer to the image if found, 
 *                  or NULL otherwise.
 */

image_t *
find_image (const char filename[]);


/**
 * Update the screen.
 *
 * @return  SUCCESS for success, FAILURE otherwise.
 */

bool_t
update_screen (void);


/** Clean up the graphics subsystem. */

void
cleanup_graphics (void);


#endif /* not _GRAPHICS_H */
