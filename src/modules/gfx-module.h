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
 * @file    modules/gfx-module.h
 * @author  Matt Windsor
 * @brief   Prototypes and declarations for graphics module.
 */

#ifndef _GFX_MODULE_H
#define _GFX_MODULE_H


/* All outward-facing functions MUST be preceded with
 * EXPORT so that they are exposed correctly in the Windows DLL.
 */
#ifdef PLATFORM_WINDOWS
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif /* PLATFORM_WINDOWS */


/* -- DECLARATIONS -- */

/**
 * Initialises the module.
 */
EXPORT bool
init (void);


/**
 * Terminates the module, freeing any remaining data dynamically
 * allocated by the module.
 */
EXPORT void
term (void);


/**
 * Initialises a screen of a given width, height and depth.
 *
 * The screen's data will automatically be deleted, if not sooner,
 * when the module is unloaded (ie, via the module's term
 * function).
 *
 * @todo  Fullscreen option?
 *
 * @param width   Width of the screen, in pixels.
 * @param height  Height of the screen, in pixels.
 * @param depth   Colour depth of the screen, in bits per pixel.
 *
 * @return  true for success; false otherwise.
 */
EXPORT bool
init_screen_internal (uint16_t width, uint16_t height,
                      uint8_t depth);


/**
 * Draws a rectangle of colour on-screen.
 *
 * Depending on the graphics module, the colour displayed on screen
 * may not exactly match the desired colour.
 *
 * @param x       X co-ordinate of the left edge of the rectangle.
 *
 * @param y       Y co-ordinate of the top edge of the rectangle.
 * @param width   The width of the rectangle, in pixels.
 * @param height  The height of the rectangle, in pixels.
 * @param red     The red component of the fill colour (0-255).
 * @param green   The green component of the fill colour (0-255).
 * @param blue    The blue component of the fill colour (0-255).
 */
EXPORT void
draw_rect_internal (int16_t x,
                    int16_t y,
                    uint16_t width,
                    uint16_t height,
                    uint8_t red,
                    uint8_t green,
                    uint8_t blue);


/**
 * Loads an image and return its data in the module's native
 * format.
 *
 * As the exact format returned varies from module to module, you
 * will likely only want to use this function through the graphics
 * subsystem's wrapper function, load_image, which also stores the
 * data into a cache.
 *
 * @param filename  The path to the file to load.
 *
 * @return  a pointer to a memory location containing image data
 *          which can eventually be passed to the module's draw_image
 *          function.
 */
EXPORT void *
load_image_data (const char filename[]);


/**
 * Frees image data retrieved by load_image_data.
 *
 * Since the nature of the image data in question varies from
 * module to module, simply freeing the data directly may not be
 * sufficient to unload the image from memory, hence the existence
 * of this function.
 *
 * @param data  A pointer to a memory location containing image
 *              data (in the module's native format) to be freed.
 */
EXPORT void
free_image_data (void *data);


/**
 * Draws a rectangular portion of an image on-screen.
 *
 * This should not be called directly, but instead accessed through
 * the graphics subsystem's draw_image function (see graphics.h).
 *
 * @param image     The image data, in the graphics module-specific
 *                  format returned by load_image_data.
 * @param image_x   The X-coordinate of the left edge of the
 *                  on-image rectangle to display.
 * @param image_y   The Y-coordinate of the top edge of the
 *                  on-image rectangle to display.
 * @param screen_x  The X-coordinate of the left edge of the
 *                  on-screen rectangle to place the image in.
 * @param screen_y  The Y-coordinate of the top edge of the
 *                  on-screen rectangle to place the image in.
 * @param width     The width of the rectangle.
 * @param height    The height of the rectangle.
 */
EXPORT void
draw_image_internal (void *image,
                     int16_t image_x,
                     int16_t image_y,
                     int16_t screen_x,
                     int16_t screen_y,
                     uint16_t width,
                     uint16_t height);


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
EXPORT void
add_update_rectangle_internal (int16_t x,
                               int16_t y,
                               uint16_t width,
                               uint16_t height);


/**
 * Updates the screen.
 */
EXPORT void
update_screen_internal (void);


/**
 * Translates the screen by a co-ordinate pair, leaving damage.
 *
 * @param x_offset  The X co-ordinate offset in which to scroll the
 *                  screen.
 * @param y_offset  The Y co-ordinate offset in which to scroll the
 *                  screen.
 */
EXPORT void
scroll_screen_internal (int16_t x_offset, int16_t y_offset);


#endif /* _GFX_MODULE_H */
