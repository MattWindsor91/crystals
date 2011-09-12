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
 */
void
init_graphics (void);


/**
 * Given a relative path to an image file, appends the graphics root
 * path to it and returns a pointer to the created string.
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
 * Writes a string on the screen, using the standard font.
 *
 * A wrapper to the image drawing functions that allows text to be
 * left, centre, or right-aligned on a line of length box_width
 * starting at (x, y).
 *
 * @param x          X position of the left edge of the text box,
 *                   in pixels from the left edge of the screen.
 * @param y          Y position of the top edge of the text box,
 *                   in pixels from the top edge of the screen.
 * @param box_width  Width of the box in which the text is rendered.
 *                   This is only used for centre or right-aligned
 *                   text, and is ignored for left-aligned text.
 * @param alignment  Desired alignment (ALIGN_LEFT, ALIGN_CENTRE or
 *                   ALIGN_RIGHT).
 * @param string     The string to write.
 */
void
write_string (int16_t x,
              int16_t y,
              uint16_t box_width,
              alignment_t alignment,
              const char string[]);


/**
 * Draws a rectangle on the screen of the given colour.
 *
 * Depending on the graphics module, the colour displayed on screen
 * may not exactly match the desired colour.
 * 
 * @param x      X position of the left edge of the rectangle,
 *               in pixels from the left edge of the screen.
 * @param y      Y position of the top edge of the rectangle,
 *               in pixels from the top edge of the screen.
 * @param width  Width of the rectangle, in pixels.
 * @param height Height of the rectangle, in pixels.
 * @param red    The red component of the fill colour (0-255).
 * @param green  The green component of the fill colour (0-255).
 * @param blue   The blue component of the fill colour (0-255).
 *
 * @return       SUCCESS for success; FAILURE otherwise.
 */
bool_t
draw_rectangle (int16_t x,
                int16_t y,
                uint16_t width,
                uint16_t height,
                uint8_t red,
                uint8_t green,
                uint8_t blue);


/**
 * Fills the screen with the given colour.
 *
 * Depending on the graphics module, the colour displayed on screen
 * may not exactly match the desired colour.
 *
 * @param red    The red component of the fill colour (0-255).
 * @param green  The green component of the fill colour (0-255).
 * @param blue   The blue component of the fill colour (0-255).
 *
 * @return       SUCCESS for success; FAILURE otherwise.
 */
bool_t
fill_screen (uint8_t red, uint8_t green, uint8_t blue);


/**
 * Translates the screen by a co-ordinate pair, leaving damage.
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
 * Loads an image.
 *
 * This does not need to be called directly in normal circumstances,
 * as draw_image automatically loads its requested image if it is not
 * present in the image cache.  Still, this function may be useful on
 * its own for pre-caching images, especially on systems with
 * noticeably slow disk access.
 *
 * @note            It is safe to directly use the pointer returned,
 *                  for example as an argument to draw_image_direct,
 *                  so long as the image is known to still be loaded.
 *                  This can be used to speed up successive drawing 
 *                  calls after an image load check.
 *
 * @param filename  The filename of the image to load, relative from
 *                  the graphics path.
 *
 * @return  A pointer to the raw data of the image if
 *          successfully loaded and stored in the image cache;
 *          NULL otherwise.
 */
image_t *
load_image (const char filename[]);


/**
 * Frees image data.
 *
 * This is, at time of writing, merely a wrapper for the driver
 * free_image_data function.
 *
 * @param image  Pointer to the image data to free.
 */
void
free_image (image_t *image);


/**
 * Draws a rectangular portion of an image on-screen.
 *
 * This will pre-load the image into the cache, if it does not
 * already exist there.
 *
 * @param filename  The filename of the image.
 * @param image_x   The X-coordinate of the left edge of the
 *                  on-image rectangle to display, in pixels from the
 *                  left edge of the entire image.
 * @param image_y   The Y-coordinate of the top edge of the
 *                  on-image rectangle to display. in pixels from the
 *                  top edge of the entire image.
 * @param screen_x  The X-coordinate of the left edge of the
 *                  on-screen rectangle to place the image in, in
 *                  pixels from the left edge of the screen.
 * @param screen_y  The Y-coordinate of the top edge of the
 *                  on-screen rectangle to place the image in, in
 *                  pixels from the top edge of the string.
 * @param width     The width of the rectangle, in pixels.
 * @param height    The height of the rectangle, in pixels.
 *
 * @return          SUCCESS for success, FAILURE otherwise.  In most
 *                  cases, a failure will simply cause the image to
 *                  not appear.
 */
bool_t
draw_image (const char filename[],
            int16_t image_x,
            int16_t image_y,
            int16_t screen_x,
            int16_t screen_y,
            uint16_t width,
            uint16_t height);


/**
 * Draws a rectangular portion of an image on-screen, using a direct
 * pointer to the driver-specific image data.
 *
 * @param data      A pointer to the raw image data.
 * @param image_x   The X-coordinate of the left edge of the
 *                  on-image rectangle to display, in pixels from the
 *                  left edge of the entire image.
 * @param image_y   The Y-coordinate of the top edge of the
 *                  on-image rectangle to display. in pixels from the
 *                  top edge of the entire image.
 * @param screen_x  The X-coordinate of the left edge of the
 *                  on-screen rectangle to place the image in, in
 *                  pixels from the left edge of the screen.
 * @param screen_y  The Y-coordinate of the top edge of the
 *                  on-screen rectangle to place the image in, in
 *                  pixels from the top edge of the string.
 * @param width     The width of the rectangle, in pixels.
 * @param height    The height of the rectangle, in pixels. *
 *
 * @return          SUCCESS for success, FAILURE otherwise. In most
 *                  cases, a failure will simply cause the image to
 *                  not appear.
 */
bool_t
draw_image_direct (image_t *data,
                   int16_t image_x,
                   int16_t image_y,
                   int16_t screen_x,
                   int16_t screen_y,
                   uint16_t width,
                   uint16_t height);


/**
 * Deletes an image previously loaded into the image cache.
 *
 * @param filename  Filename of the image.
 *
 * @return          SUCCESS if the deletion succeeded; 
 *                  FAILURE otherwise.
 */
bool_t
delete_image (const char filename[]);


/**
 * Deletes all images in the image cache.
 */
void
clear_images (void);


/**
 * Retrieves an image from the image cache.
 *
 * @param filename  The filename of the image to retrieve.
 *
 * @return          A pointer to the image if found, 
 *                  or NULL otherwise.
 */
image_t *
find_image (const char filename[]);


/**
 * Adds a rectangle to the next update run.
 * 
 * @param x       The X co-ordinate of the left side of the rectangle,
 *                in pixels from the left side of the screen.
 * @param y       The Y co-ordinate of the top side of the rectangle,
 *                in pixels from the top side of the screen.
 * @param width   The width of the rectangle, in pixels.
 * @param height  The height of the rectangle, in pixels.
 */
void
add_update_rectangle (uint16_t x,
                      uint16_t y,
                      int16_t width,
                      int16_t height);


/**
 * Updates the screen.
 *
 * @return  SUCCESS for success, FAILURE otherwise.
 */
bool_t
update_screen (void);


/**
 * Cleans up the graphics subsystem.
 */
void
cleanup_graphics (void);


#endif /* not _GRAPHICS_H */
