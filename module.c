/** @file    module.c
 *  @author  Michael Walker
 *  @brief   Functions to handle loading and unloading of the modules that make up the engine.
 */

#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <string.h>

#ifdef TESTSUITE
#include "tests/module.h"
#endif /* TESTSUITE */

#include "util.h"
#include "module.h"

module_set g_modules; /* The set of all modules in use */

/* This initialises the struct of modules to NULL and sets the load path */
int
init_modules (const char *path)
{
  g_modules.path = malloc (sizeof (char) * (strlen (path) + 1));

  if (g_modules.path)
    {
      strncpy (g_modules.path, path, strlen (path) + 1);

      module_bare_init (&g_modules.bindings.metadata);
      module_bare_init (&g_modules.gfx.metadata);
      module_bare_init (&g_modules.event.metadata);

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
int
get_module_path (const char* module, const char* modulespath, char** out)
{
  char *path;

  path = (char*) malloc (sizeof (char) * (strlen (modulespath)
                                          + strlen (module)
                                          + strlen (MODULESUFFIX) + 1));

  if (path)
    {
      strcpy (path, modulespath);
      strcat (path, module);
      strcat (path, MODULESUFFIX);
    }
  else
    {
      fprintf (stderr, "ERROR: couldn't allocate module path.\n");
      return FAILURE;
    }

  *out = path;
  return SUCCESS;
}

/* This finds the filename of a module and calls get_module */
int
get_module_by_name (const char* name, const char *modulespath, module_data *module)
{
  /* Soon-to-be return value of get_module */
  int out;
  /* Get the name of the module */
  char *modulepath;
  if (get_module_path (name, modulespath, &modulepath) == FAILURE) return FAILURE;

  /* And get the module */
  if (modulepath)
    {
      out = get_module (modulepath, module);
      free (modulepath);
      return out;
    }
  else
    {
      fprintf (stderr, "ERROR: couldn't find module path.\n");
      return FAILURE;
    }
}

/* This opens a module file and runs any init code */
int
get_module (const char* modulepath, module_data *module)
{
  char *error;

  if (module->lib_handle != NULL) return FAILURE;

  module->lib_handle = dlopen (modulepath, RTLD_LAZY);

  if ((error = dlerror ()) != NULL)
    {
      fprintf (stderr, "DLERROR: %s\n", error);
      return FAILURE;
    }

  /* Get init and termination functions if present */
  get_module_function (*module, "init", (void**) &module->init);
  get_module_function (*module, "term", (void**) &module->term);

  /* Execute init function if present */
  if (module->init != NULL)
    {
      (*module->init) ();
    }

  return SUCCESS;
}

/* This loads a pointer to a function from a module */
int
get_module_function (module_data module, const char *function, void **func)
{
  char *error;

  *(void**)(func) = dlsym (module.lib_handle, function);

  if ((error = dlerror ()) != NULL)
    {
      fprintf (stderr, "DLERROR: %s\n", error);
      return FAILURE;
    }

  return SUCCESS;
}

/* Load a graphics module. */

int
load_module_gfx (const char* name, module_set* modules)
{
  if (get_module_by_name (name, modules->path, &modules->gfx.metadata) == FAILURE)
    return FAILURE;
  
  if (get_module_function (modules->gfx.metadata, "init_screen",
                           (void**)
                           &modules->gfx.init_screen) == FAILURE)
    return FAILURE;
  
  if (get_module_function (modules->gfx.metadata, "draw_rect",
                           (void**)
                           &modules->gfx.draw_rect) == FAILURE)
    return FAILURE;
  
  if (get_module_function (modules->gfx.metadata, "load_image_data",
                           (void**)
                           &modules->gfx.load_image_data) == FAILURE)
    return FAILURE;
  
  if (get_module_function (modules->gfx.metadata, "free_image_data",
                           (void**)
                           &modules->gfx.free_image_data) == FAILURE)
    return FAILURE;
  
  if (get_module_function (modules->gfx.metadata, "draw_image",
                           (void**)
                           &modules->gfx.draw_image) == FAILURE)
    return FAILURE;
  
  if (get_module_function (modules->gfx.metadata, "update_screen",
                           (void**)
                           &modules->gfx.update_screen)  == FAILURE)
    return FAILURE;
  
  if (get_module_function (modules->gfx.metadata, "scroll_screen",
                           (void**)
                           &modules->gfx.scroll_screen) == FAILURE)
    return FAILURE;
  
  return SUCCESS;
}

/* Load an event module. */

int
load_module_event (const char *name, module_set *modules)
{
  if (get_module_by_name (name, modules->path, &modules->event.metadata) == FAILURE)
    return FAILURE;
  
  if (get_module_function (modules->event.metadata, "process_events",
                           (void**)
                           &modules->event.process_events) == FAILURE)
    return FAILURE;
  
  if (get_module_function (modules->event.metadata,
                           "register_release_handle",
                           (void**)
                           &modules->event.register_release_handle)
      == FAILURE)
    return FAILURE;
  
  return SUCCESS;
}

int
load_module_bindings (const char *name, module_set *modules)
{
  if (get_module_by_name (name, modules->path, &modules->bindings.metadata) 
        == FAILURE)
        return FAILURE;

  if (get_module_function (modules->bindings.metadata, "run_file",
                               (void**)
                               &modules->bindings.run_file) == FAILURE)
        return FAILURE;

  return SUCCESS;
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
  close_module (&g_modules.bindings.metadata);
  close_module (&g_modules.event.metadata);
  close_module (&g_modules.gfx.metadata);

  free (g_modules.path);
}
/* vim: set ts=2 sw=2 softtabstop=2: */
