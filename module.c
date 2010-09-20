/* module.c - functions to handle loading of the modules that make up the engine. */

#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <string.h>

#include "module.h"

/* If making tests, we have redefined stuff in the tester. */
#ifdef TESTSUITE
#include "tests/module.h"
#endif /* TESTSUITE */

/* This initialises the struct of modules to NULL and sets the load path */
#ifndef TESTSUITE
void
init_modules (const char *path)
{
  modules.path = (char*) malloc (sizeof (char) * strlen (path));
  strcpy (modules.path, path);

  /* When we have modules, do this:
  module_bare_init (&modules.MODULE.metadata);
  modules.MODULE.FUNCTION = null
  */
}
#endif /* TESTSUITE */

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

  path = (char*) malloc (sizeof (char) * strlen (modules.path));

  strcpy (path, modules.path);
  strcat (path, module);
  strcat (path, MODULESUFFIX);

  *out = path;
}

/* This opens a module file and runs any init code */
void
get_module (const char* modulepath, module_data *module)
{
  char *error;

#ifdef TESTSUITE
  printf("Loading module: %s\n", modulepath);
#endif /* TESTSUITE */

  module->lib_handle = dlopen (modulepath, RTLD_LAZY);

  if ((error = dlerror ()) != NULL)
    {
      fprintf (stderr, "%s\n", dlerror ());

#ifndef TESTSUITE
      exit (1);
#endif /* TESTSUITE */
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
      fprintf (stderr, "%s\n", error);

#ifndef TESTSUITE
      exit (1);
#endif /* TESTSUITE */
    }
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
#ifndef TESTSUITE
void
close_modules (void)
{
  /* No modules - but when we have some do this:
  close_module (&modules.MODULE.metadata);
  */
}
#endif /* TESTSUITE */
