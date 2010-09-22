/** @file    module.h
 *  @author  Michael Walker
 *  @brief   Prototypes and declarations for module loading and unloading functions.
 */

#ifndef _MODULE_H
#define _MODULE_H

#include "events.h"

/* -- CONSTANTS -- */

/** The file suffix of modules */
#define MODULESUFFIX ".so"

/* -- STRUCTURES -- */

#ifndef TESTSUITE
/** The module metadata structure.
 *
 *  This contains the basic data that all modules require to function.
 */

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
  (*init_screen) (unsigned short width,
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

  void
  (*draw_rect) (short x,
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

  /** Frees image data retrieved by load_image_data.
   *
   *  Since the nature of the image data in question varies from
   *  module to module, simply freeing the data directly may not be
   *  sufficient to unload the image from memory, hence the existence
   *  of this function.
   *
   *  @param data  A pointer to a memory location containing image
   *               data (in the module's native format) to be freed.
   */

  void
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
   *  @param image_y   The Y-coordinate of the top edge of the
   *                   on-image rectangle to display.
   *
   *  @param width     The width of the rectangle.
   *
   *  @param height    The height of the rectangle.
   *
   *  @return  SUCCESS for success, FAILURE otherwise. In most
   *           cases, a failure will simply cause the image to not appear.
   */

  int
  (*draw_image) (void *image,
                 short image_x,
                 short image_y,
                 short screen_x,
                 short screen_y,
                 unsigned short width,
                 unsigned short height);

  /** Update the screen. */

  void
  (*update_screen) (void);

  /** Scroll the entire screen one pixel in a given direction.
   *
   *  @param direction  The cardinal direction (NORTH, SOUTH, EAST or
   *  WEST) to scroll in.
   */

  void
  (*scroll_screen) (unsigned int direction);

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
  (*process_events) (void);

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


/** The bindings module table.
 *
 *  This contains functions to handle bindings
 *  for different programming languages.
 */

typedef struct
{
  module_data metadata; /**< Metadata for the bindings module. */

  /** Executes the given file
   *
   *  @param path   Path to file.
   *
   *  @return Returns SUCCESS if success else FAILURE.
   */

  int
  (*run_file) (const char *path);

  /** A Test */

  void
  (*test) (void);

} module_bindings;

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
  module_bindings bindings; /**< The bindings module */

} module_set;

#endif /* TESTSUITE */

/* -- GLOBAL VARIABLES -- */

/** The set of modules in use. */
extern module_set g_modules;

/* -- PROTOTYPES -- */

#ifndef TESTSUITE
/** Initialise the g_modules structure.
 *
 *  @todo Allow passing a structure by reference, perhaps? Would allow
 *  (with changes elsewhere) for multiple sets of modules to be loaded
 *  at once.
 *
 *
 *  @param path The path to the modules.
 *
 *  @return Either SUCCESS or FAILURE (defined in main.h).
 */

int
init_modules (const char *path);
#endif /* TESTSUITE */

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
 *  @param module The name of the module
 *  @param out    A string to store the path in
 */
void
get_module_path (const char *module, char **out);

/** Find and load a module by name
 *
 *  This finds and loads a module, initialising the module_data struct so it can be
 *  used by get_module_function.
 *
 *  @param name   The name of the module
 *  @param module Pointer to the module_data structure to use
 *
 *  @return Either SUCCESS or FAILURE (defined in main.h)
 */

int
get_module_by_name (const char* name, module_data *module);

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

/** Find a pointer to a function within a module
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
get_module_function (module_data metadata, const char *function, void **func);

#ifndef TESTSUITE
/** Load the graphics module.
 *
 *  This loads a graphics module and sets up g_modules.gfx.
 *
 *  @param name The name of the module to load (without the extension).
 *
 *  @return Either SUCCESS or FAILURE (defined in main.h).
 */

int
load_module_gfx (const char* name);

/** Load the event module.
 *
 *  This loads an event module and sets up g_modules.event.
 *
 *  @param name The name of the module to load (without the extension).
 *
 *  @return Either SUCCESS or FAILURE (defined in main.h).
 */

int
load_module_event (const char* name);

/** Load the language binding module.
 *
 *  This loads an event module and sets up g_modules.event.
 *
 *  @param name The name of the module to load (without the extension).
 *
 *  @return Either SUCCESS or FAILURE (defined in main.h).
 */

int
load_module_bindings (const char* name);

#endif /* TESTSUITE */

/** Close an individual module.
 *
 *  This runs any term function which is present in the module and closes
 *  the open libdl handle to it.
 *
 *  @param module A pointer to the module_data struct to close.
 */
void
close_module (module_data *module);

#ifndef TESTSUITE
/** Clean up all modules.
 *
 *  This runs close_module on every module.
 */
void
cleanup_modules (void);
#endif /* TESTSUITE */

#endif /* _MODULE_H */
