/** @file    module.h
 *  @author  Michael Walker
 *  @brief   Prototypes and declarations for module loading and unloading functions.
 */

#ifndef _MODULE_H
#define _MODULE_H

/* -- CONSTANTS -- */

/* The file suffix of modules */
#define MODULESUFFIX ".so"

/* -- STRUCTURES -- */

#ifndef TESTSUITE
/** The module_data structure.
 *
 *  This contains the basic data that all modules require to function
 */
typedef struct
{
  void *lib_handle;
  void (*init)(void);
  void (*term)(void);
} module_data;

/** The module_set struct
 *
 *  This contains the path to the module files, and additionally
 *  structs for each individual module type - currently graphics.
 */
typedef struct
{
  char *path;

  struct
  {
    module_data metadata;

    int
    (*init_screen) (unsigned short width,
                    unsigned short height, unsigned char depth);

    void
    (*draw_rect) (short x, 
                  short y, 
                  unsigned short width,
                  unsigned short height, 
                  unsigned char red,
                  unsigned char green,
                  unsigned char blue);

    void *
    (*load_image_data) (const char filename[]);

    void 
    (*free_image_data) (void *data);

    int
    (*draw_image) (void *image, 
                   short image_x,
                   short image_y,
                   short screen_x,
                   short screen_y,
                   unsigned short width,
                   unsigned short height);

    void
    (*update_screen) (void);

    void
    (*scroll_screen) (unsigned int direction);

  } gfx;
 
} module_set;
#endif /* TESTSUITE */

/* -- GLOBAL VARIABLES -- */

/* The set of modules in use */
extern module_set g_modules;

/* -- PROTOTYPES -- */

#ifndef TESTSUITE
/** Initialise the g_modules structure
 *  
 *  @todo Allow passing a structure by reference, perhaps? Would allow (with changes elsewhere)
 *  for multiple sets of modules to be loaded at once.
 * 
 *  @param path The path to the modules.
 *   
 *  @return Either SUCCESS or FAILURE (defined in main.h)
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
/** Load the graphics module
 *  
 *  This loads the graphics module, currently gfx-sdl, and sets up g_modules.gfx
 *  
 *  @return Either SUCCESS or FAILURE (defined in main.h)
 */
int
load_module_gfx (void);
#endif /* TESTSUITE */

/** Close an individual module
 *  
 *  This runs any term function which is present in the module and closes
 *  the open libdl handle to it.
 *  
 *  @param module A pointer to the module_data struct to close.
 */
void
close_module (module_data *module);

#ifndef TESTSUITE
/** Clean up all modules
 *  
 *  This runs close_module on every module.
 */
void
cleanup_modules (void);
#endif /* TESTSUITE */

#endif /* _MODULE_H */
