/* module.c - functions to handle loading of the modules that make up the engine. */

#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <string.h>

#include "main.h"
#include "module.h"

struct ModuleSet g_modules;

/* This initialises the struct of modules to NULL and sets the load path */

int
init_modules (const char *path)
{
  g_modules.path = malloc (sizeof (char) * (strlen (path) + 1));

  if (g_modules.path)
    {
      strncpy (g_modules.path, path, strlen (path) + 1);
      
      module_bare_init (&g_modules.gfx.metadata);
      g_modules.gfx.init_screen = NULL;
      g_modules.gfx.draw_rect = NULL;
      g_modules.gfx.load_image_data = NULL;
      g_modules.gfx.free_image_data = NULL;
      g_modules.gfx.draw_image = NULL;
      g_modules.gfx.update_screen = NULL;
      g_modules.gfx.scroll_screen = NULL;

      module_bare_init (&g_modules.event.metadata);
      g_modules.event.process_events = NULL;
      g_modules.event.register_release_handle = NULL;

      return SUCCESS;
    }
  else
    {
      fprintf (stderr, "ERROR: Couldn't alloc modules path!\n");
    }

  return FAILURE;
}

/* This does the bare minimum initialisation for a module */
void
module_bare_init (module_data *module)
{
  module->lib_handle = NULL;
  module->init       = NULL;
  module->term       = NULL;
}

/* This gets the path of a module, storing it in out */
void
get_module_path (const char* module, char** out)
{
  char *path;

  path = (char*) malloc (sizeof (char) * (strlen (g_modules.path)
                                          + strlen (module) 
                                          + strlen (MODULESUFFIX) + 1));

  if (path)
    {
      strcpy (path, g_modules.path);
      strcat (path, module);
      strcat (path, MODULESUFFIX);
    }
  else
    {
      fprintf (stderr, "ERROR: couldn't allocate module path.\n");
    }

  *out = path;
}

/* This opens a module file and runs any init code */
void
get_module (const char* modulepath, module_data *module)
{
  char *error;

  module->lib_handle = dlopen (modulepath, RTLD_LAZY);

  if ((error = dlerror ()) != NULL)
    {
      fprintf (stderr, "DLERROR: %s\n", dlerror ());

      exit (1);
    }

  /* Get init and termination functions if present */
  get_module_function (*module, "init", (void**) &module->init);
  get_module_function (*module, "term", (void**) &module->term);
  
  /* Execute init function if present */
  if (module->init != NULL)
    {
      (*module->init) ();
    }
}

/* This loads a pointer to a function from a module */
void
get_module_function (module_data module, const char *function, void **func)
{
  char *error;

  *(void**)(func) = dlsym (module.lib_handle, function);

  if ((error = dlerror ()) != NULL)
    {
      fprintf (stderr, "DLERROR: %s\n", error);

      exit (1);
    }
}

int
load_module_gfx (void)
{
  char *modulepath;

  get_module_path ("gfx-sdl", &modulepath);

  if (g_modules.gfx.metadata.lib_handle == NULL && 
      modulepath)
    {
      get_module (modulepath, &g_modules.gfx.metadata);

      get_module_function (g_modules.gfx.metadata, "init_screen",
                           (void**) &g_modules.gfx.init_screen);

      get_module_function (g_modules.gfx.metadata, "draw_rect", 
                           (void**) &g_modules.gfx.draw_rect);

      get_module_function (g_modules.gfx.metadata, "load_image_data", 
                           (void**) &g_modules.gfx.load_image_data);

      get_module_function (g_modules.gfx.metadata, "free_image_data", 
                           (void**) &g_modules.gfx.free_image_data);

      get_module_function (g_modules.gfx.metadata, "draw_image", 
                           (void**) &g_modules.gfx.draw_image);

      get_module_function (g_modules.gfx.metadata, "update_screen", 
                           (void**) &g_modules.gfx.update_screen);

      get_module_function (g_modules.gfx.metadata, "scroll_screen", 
                           (void**) &g_modules.gfx.scroll_screen);

      free (modulepath);

      return SUCCESS;
    }
  return FAILURE;
}

int
load_module_event (void)
{
  char *modulepath;

  get_module_path ("event-sdl", &modulepath);

  if (g_modules.event.metadata.lib_handle == NULL && 
      modulepath)
    {
      get_module (modulepath, &g_modules.event.metadata);

      get_module_function (g_modules.event.metadata, "process_events",
                           (void**) &g_modules.event.process_events);

      get_module_function (g_modules.event.metadata, "register_release_handle",
                           (void**) &g_modules.event.register_release_handle);

      free (modulepath);

      return SUCCESS;
    }
  return FAILURE;
}

/* This closes an individual module and runs any termination code */
void
close_module (module_data *module)
{
  if (module->lib_handle)
    {
      if (module->term)
        {
          /* Call any termination code */
          (*module->term) ();
        }
      /* Close the libdl handle */
      dlclose (module->lib_handle);
    }
}

/* This closes any loaded modules, run before program termination */
void
cleanup_modules (void)
{
  close_module (&g_modules.event.metadata);
  close_module (&g_modules.gfx.metadata);
}
