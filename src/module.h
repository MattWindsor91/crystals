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

/** @file    src/module.h
 *  @author  Michael Walker
 *  @brief   Prototypes and declarations for module loading and
 *           unloading functions.
 */

#ifndef _MODULE_H
#define _MODULE_H

#include "events.h"

/* -- MACROS -- */

/* The following macros are used for the purposes of POSIX and Windows
 * compatibility with regards to dynamic library loading.
 */

/** Windows 32-bit - use Windows API to load .dll files. */

#ifdef PLATFORM_WINDOWS

#include <windows.h>
typedef HMODULE dll_handle;                  /**< Handle type used for DLL modules. */
typedef FARPROC mod_function_ptr;            /**< Function pointer type used for DLLs. */
#define DLLOPEN(x) LoadLibrary(x)            /**< Function used for opening DLLs. */
#define DLLLOOKUP(x,y) GetProcAddress(x,y)   /**< Function used for looking up DLL symbols. */
#define DLLCLOSE(x) FreeLibrary(x)           /**< Function used for closing DLLs. */
#define MODULESUFFIX ".dll"                  /**< Suffix used for DLLs. */

#endif /* PLATFORM_WINDOWS */


/** UNIX and similar - use dlfcn.h, dlopen etc. to load .so files. */

#ifdef USE_LIBDL

#include <dlfcn.h>
typedef void* dll_handle;                    /**< Handle type used for SO modules. */
typedef void* mod_function_ptr;              /**< Function pointer type used for SOs. */
#define DLLOPEN(x) dlopen(x, RTLD_LAZY)      /**< Function used for opening SOs. */
#define DLLLOOKUP(x,y) dlsym(x,y)            /**< Function used for looking up SO symbols. */
#define DLLCLOSE(x) dlclose(x)               /**< Function used for closing SOs. */
#define MODULESUFFIX ".so"                   /**< Function used for SOs. */

#endif /* USE_LIBDL */


/* -- STRUCTURES -- */

/** The module metadata structure.
 *
 *  This contains the basic data that all modules require to function.
 */

#ifndef TESTSUITE
typedef struct
{
  void *lib_handle; /**< The dynamic loading handle for the module. */

  void
  (*init) (void); /**< Pointer to the module's initialisation
                     function. */
  void
  (*term) (void); /**< Pointer to the module's termination
                     function. */
} module_data;
#endif /* TESTSUITE */


/** The graphics module vtable.
 *
 *  This contains function pointers for the graphics class of modules,
 *  along with module metadata.
 */

typedef struct
{
  module_data metadata; /**< Metadata for the graphics module. */

  /** Initialise a screen of a given width, height and depth.
   *
   *  The screen's data will automatically be deleted, if not sooner,
   *  when the module is unloaded (ie, via the module's term
   *  function).
   *
   *  @todo Fullscreen option?
   *
   *  @param width   Width of the screen, in pixels.
   *  @param height  Height of the screen, in pixels.
   *  @param depth   Colour depth of the screen, in bits per pixel.
   *
   *  @return  SUCCESS for success; FAILURE otherwise.
   */


  int
  (*init_screen_internal) (unsigned short width,
                           unsigned short height,
                           unsigned char depth);

  /** Draw a rectangle of colour on-screen.
   *
   *  Depending on the graphics module, the colour displayed on screen
   *  may not exactly match the desired colour.
   *
   *  @param x       X co-ordinate of the left edge of the rectangle.
   *
   *  @param y       Y co-ordinate of the top edge of the rectangle.
   *
   *  @param width   The width of the rectangle, in pixels.
   *
   *  @param height  The height of the rectangle, in pixels.
   *
   *  @param red     The red component of the fill colour (0-255).
   *
   *  @param green   The green component of the fill colour (0-255).
   *
   *  @param blue    The blue component of the fill colour (0-255).
   */


  int
  (*draw_rect_internal) (short x,
                         short y,
                         unsigned short width,
                         unsigned short height,
                         unsigned char red,
                         unsigned char green,
                         unsigned char blue);

  /** Load an image and return its data in the module's native
   *  format.
   *
   *  As the exact format returned varies from module to module, you
   *  will likely only want to use this function through the graphics
   *  subsystem's wrapper function, load_image, which also stores the
   *  data into a cache.
   *
   *  @param filename  The path to the file to load.
   *
   *  @return  a pointer to a memory location containing image data
   *  which can eventually be passed to the module's draw_image
   *  function.
   */

  void *
  (*load_image_data) (const char filename[]);


  /** Free image data retrieved by load_image_data.
   *
   *  Since the nature of the image data in question varies from
   *  module to module, simply freeing the data directly may not be
   *  sufficient to unload the image from memory, hence the existence
   *  of this function.
   *
   *  @param data  A pointer to a memory location containing image
   *               data (in the module's native format) to be freed.
   */

  int
  (*free_image_data) (void *data);


  /** Draw a rectangular portion of an image on-screen.
   *
   *  This should not be called directly, but instead accessed through
   *  the graphics subsystem's draw_image function (see graphics.h).
   *
   *  @param image     The image data, in the graphics module-specific
   *                   format returned by load_image_data.
   *
   *  @param image_x   The X-coordinate of the left edge of the
   *                   on-image rectangle to display.
   *
   *  @param image_y   The Y-coordinate of the top edge of the
   *                   on-image rectangle to display.
   *
   *  @param screen_x  The X-coordinate of the left edge of the
   *                   on-screen rectangle to place the image in.
   *
   *  @param screen_y  The Y-coordinate of the top edge of the
   *                   on-screen rectangle to place the image in.
   *
   *  @param width     The width of the rectangle.
   *
   *  @param height    The height of the rectangle.
   *
   *  @return  SUCCESS for success, FAILURE otherwise. In most
   *           cases, a failure will simply cause the image to not appear.
   */

  int
  (*draw_image_internal) (void *image,
                          short image_x,
                          short image_y,
                          short screen_x,
                          short screen_y,
                          unsigned short width,
                          unsigned short height);


  /** Update the screen. */

  int
  (*update_screen_internal) (void);


  /** Translate the screen by a co-ordinate pair, leaving damage.
   *
   *  @param x_offset  The X co-ordinate offset in which to scroll the 
   *                   screen.
   *
   *  @param y_offset  The Y co-ordinate offset in which to scroll the 
   *                   screen.
   */

  int
  (*scroll_screen_internal) (short x_offset, short y_offset);


} module_gfx;

/** The event module vtable.
 *
 *  This contains function pointers for the event-handler class of
 *  modules, along with module metadata.
 */

typedef struct
{
  module_data metadata; /**< Metadata for the event module. */

  /** Process all waiting events.
   *
   *  This reads events from the module's events backend (for example,
   *  SDL's events system) and then proceeds to pass them up to the
   *  main events subsystem after being processed into the general
   *  event format (see events.h).
   */

  void
  (*process_events_internal) (void);

  /** Register an event release handle with the event module.
   *
   *  The event release handle is a function that the event module
   *  calls once per event handled, and is expected to distribute the
   *  event to registered callbacks.  It is necessary to supply a
   *  pointer to this function, rather than directly calling it, due
   *  to the nature of the module loading environment.
   *
   *  The function to be passed should, in all or almost all
   *  circumstances, be event.c's implementation of event_release.
   *
   *  @param handle  The event release handle to register. This should
   *                 be a function that takes an event_t pointer as
   *                 its sole parameter and distributes that event to
   *                 the rest of the system.
   */

  void
  (*register_release_handle) (void (*handle) (event_t *event));

} module_event;

/** The module set.
 *
 *  This contains the functions for engine bindings to different
 *  programming languages
 */

typedef struct
{
  char *path;               /**< Path to the module directory. */

  module_gfx gfx;           /**< The graphics module. */
  module_event event;       /**< The event module. */

} module_set;


/* -- GLOBAL VARIABLES -- */

/** The set of modules in use. */

extern module_set g_modules;


/* -- PROTOTYPES -- */

/** Initialise the g_modules structure.
 *
 *  @param path The path to the modules.
 *
 *  @return Either SUCCESS or FAILURE (defined in main.h).
 */

int
init_modules (const char *path);


/** Perform the minimum initialisation needed for a module
 *
 *  This is called for every module as part of init_modules.
 *
 *  @param module Pointer to the module_data structure to initialise.
 */

void
module_bare_init (module_data *module);


/** Get the path to a module, given its name.
 *
 *  @param module      The name of the module
 *  @param modulespath The path where modules are stored
 *  @param out         A string to store the path in
 *
 *  @return Either SUCCESS or FAILURE (defined in main.h).
 */

int
get_module_path (const char *module, const char *modulespath, char **out);


/** Find and load a module by name.
 *
 *  This finds and loads a module, initialising the module_data struct so it can be
 *  used by get_module_function.
 *
 *  @param name        The name of the module
 *  @param modulespath The path where modules are stored
 *  @param module      Pointer to the module_data structure to use
 *
 *  @return Either SUCCESS or FAILURE (defined in main.h)
 */

int
get_module_by_name (const char* name, const char *modulespath, module_data *module);

/** Find and load a module by filename
 *
 *  This finds and loads a module, initialising the module_data struct so it can be
 *  used by get_module_function.
 *
 *  @param modulepath The filename of the module
 *  @param module     Pointer to the module_data structure to use
 *
 *  @return Either SUCCESS or FAILURE (defined in main.h)
 */

int
get_module (const char* modulepath, module_data *module);


/**
 * Print the last DLL-acquisition error message, if any.
 *
 * @param function_name  The name of the function calling print_dll_error,
 * to display in the message.
 *
 */

void
print_dll_error (const char function_name[]);


/** Find a pointer to a function within a module.
 *
 *  This finds a pointer to a named symbol (only tested for functions, currently) in
 *  a loaded module.
 *
 *  @param metadata The module_data structure for the desired module.
 *  @param function The name of the symbol to be loaded.
 *  @param func     A pointer to where the symbol pointer will be stored.
 *
 *  @return Either SUCCESS or FAILURE (defined in main.h)
 */

int
get_module_function (module_data metadata, const char *function, mod_function_ptr *func);


/** Load the graphics module.
 *
 *  This loads a graphics module and sets up g_modules.gfx.
 *
 *  @param name    The name of the module to load (without the extension).
 *  @param modules The module_set structure to use
 *
 *  @return Either SUCCESS or FAILURE (defined in main.h).
 */

int
load_module_gfx (const char* name, module_set* modules);


/** Load the event module.
 *
 *  This loads an event module and sets up g_modules.event.
 *
 *  @param name    The name of the module to load (without the extension).
 *  @param modules The module_set structure to use
 *
 *  @return Either SUCCESS or FAILURE (defined in main.h).
 */

int
load_module_event (const char* name, module_set* modules);


/** Load the language binding module.
 *
 *  This loads an event module and sets up g_modules.event.
 *
 *  @param name    The name of the module to load (without the extension).
 *  @param modules The module_set structure to use
 *
 *  @return Either SUCCESS or FAILURE (defined in main.h).
 */

int
load_module_bindings (const char* name, module_set* modules);


/** Close an individual module.
 *
 *  This runs any term function which is present in the module and closes
 *  the open libdl handle to it.
 *
 *  @param module A pointer to the module_data struct to close.
 */

void
close_module (module_data *module);


/** Clean up all modules in the g_modules structure.
 *
 *  This runs close_module on every module.
 */

void
cleanup_modules (void);


#endif /* _MODULE_H */
