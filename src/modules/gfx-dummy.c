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

#include "../util.h" /* Constants. */

/* Workaround for Windows DLL symbol load failures.
 *
 * All outward-facing functions MUST be preceded with
 * EXPORT so that the DLL loader can see them.
 */

#ifdef PLATFORM_WINDOWS
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif /* PLATFORM_WINDOWS */


/* -- PROTOTYPES -- */

/** 
 * Initialise the module. 
 */

EXPORT bool_t
init (void);


/** 
 * Terminate the module, freeing any remaining data dynamically
 * allocated by the module. 
 */

EXPORT void
term (void);


/** 
 * Initialise a screen of a given width, height and depth.
 *
 * The screen's data will automatically be deleted, if not sooner,
 * when the module is unloaded (ie, via the module's term
 * function).
 *
 * @todo Fullscreen option?
 *
 * @param width   Width of the screen, in pixels.
 * @param height  Height of the screen, in pixels.
 * @param depth   Colour depth of the screen, in bits per pixel.
 *
 * @return  SUCCESS for success; FAILURE otherwise.
 */

EXPORT bool_t
init_screen_internal (unsigned short width,
                      unsigned short height,
                      unsigned char depth);


/**
 * Draw a rectangle of colour on-screen.
 *
 * Depending on the graphics module, the colour displayed on screen
 * may not exactly match the desired colour.
 *
 * @param x       X co-ordinate of the left edge of the rectangle.
 *
 * @param y       Y co-ordinate of the top edge of the rectangle.
 *
 * @param width   The width of the rectangle, in pixels.
 *
 * @param height  The height of the rectangle, in pixels.
 *
 * @param red     The red component of the fill colour (0-255).
 *
 * @param green   The green component of the fill colour (0-255).
 *
 * @param blue    The blue component of the fill colour (0-255).
 */

EXPORT bool_t
draw_rect_internal (short x,
                    short y, 
                    unsigned short width,
                    unsigned short height, 
                    unsigned char red,
                    unsigned char green,
                    unsigned char blue);


/**
 * Load an image and return its data in the module's native
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
 * Free image data retrieved by load_image_data.
 *
 * Since the nature of the image data in question varies from
 * module to module, simply freeing the data directly may not be
 * sufficient to unload the image from memory, hence the existence
 * of this function.
 *
 * @param data  A pointer to a memory location containing image
 *              data (in the module's native format) to be freed.
 */

EXPORT bool_t
free_image_data (void *data);


/**
 * Draw a rectangular portion of an image on-screen.
 *
 * This should not be called directly, but instead accessed through
 * the graphics subsystem's draw_image function (see graphics.h).
 *
 * @param image     The image data, in the graphics module-specific
 *                  format returned by load_image_data.
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
 * @return  SUCCESS for success, FAILURE otherwise. In most
 *          cases, a failure will simply cause the image to not appear.
 */

EXPORT bool_t
draw_image_internal (void *image, 
                     short image_x,
                     short image_y,
                     short screen_x,
                     short screen_y,
                     unsigned short width,
                     unsigned short height);


/** 
 * Update the screen. 
 */

EXPORT bool_t
update_screen_internal (void);


/** 
 * Translate the screen by a co-ordinate pair, leaving damage.
 *
 * @param x_offset  The X co-ordinate offset in which to scroll the 
 *                  screen.
 *
 * @param y_offset  The Y co-ordinate offset in which to scroll the 
 *                  screen.
 */

EXPORT bool_t
scroll_screen_internal (short x_offset, short y_offset);


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
init_screen_internal (unsigned short width,
                      unsigned short height,
                      unsigned char depth)
{
  (void) width;
  (void) height;  
  (void) depth;  
  return SUCCESS;
}


/* Draw a rectangle of colour on-screen. */

EXPORT bool_t
draw_rect_internal (short x,
                    short y, 
                    unsigned short width,
                    unsigned short height, 
                    unsigned char red,
                    unsigned char green,
                    unsigned char blue)
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
                     short image_x,
                     short image_y,
                     short screen_x,
                     short screen_y,
                     unsigned short width,
                     unsigned short height)
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
scroll_screen_internal (short x_offset, short y_offset)
{
  (void) x_offset;
  (void) y_offset;
  return SUCCESS;
}
