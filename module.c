/* module.c - functions to handle loading of the modules that make up the engine. */

#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <string.h>
#include "module.h"

/* This initialises the struct of modules to NULL and sets the load path */
void
init_modules (const char *path)
{
  modules.path = strdup(path);
}

/* This gets the path of a module, storing it in out */
void
get_module_path (const char* module, char** out)
{
  char *path = strdup(modules.path);

  strcat(path, module);
  strcat(path, MODULESUFFIX);

  *out = strdup(path);

  free(path);
}

/* This opens a module file */
void
get_module_handle (const char* modulepath, void **lib_handle)
{
  *lib_handle = dlopen(modulepath, RTLD_LAZY);

  if (!lib_handle)
    {
      fprintf (stderr, "%s\n", dlerror ());
      exit (1);
    }
}

/* This loads a pointer to a function from a module */
void
get_module_function (void *lib_handle, const char *function, void **func)
{
  char *error;

  *(void**)(func) = dlsym(lib_handle, function);

  if ((error = dlerror ()) != NULL)
    {
      fprintf (stderr, "%s\n", error);
      exit (1);
    }
}

/* This closes any loaded modules, run before program termination */
void
close_modules(void)
{
  /* No modules - but when we have some do this:
  if (modules.MODULE.lib_handle)
    {
      dlclose(modules.MODULE.lib_handle);
    }
  */
}
