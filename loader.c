/* loader - a simple module loader, proof-of-concept for Crystals (working title) */

#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <string.h>
#include "loader.h"

/* This initialises the struct of modules to NULL */
void
init_modules (void)
{
  modules.test.lib_handle  = NULL;
  modules.test.hello       = NULL;
  modules.test.sum_numbers = NULL;
}

/* This gets the path of a module, storing it in out */
void
get_module_path (const char* module, char** out)
{
  char *path = strdup(MODULEPATH);

  strcat(path, module);
  strcat(path, MODULESUFFIX);

  *out = strdup(path);
}

/* This opens a module file */
void
get_module_handle (const char* modulepath, void *lib_handle)
{
  modules.test.lib_handle = dlopen(modulepath, RTLD_LAZY);

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

/* This loads the test module and populates the global module struct */
void
load_module_test (void)
{
  /* Get the path of the module */
  char *modulepath;
  get_module_path ("test", &modulepath);

  if (modules.test.lib_handle == NULL)
    {
      get_module_handle (modulepath, &modules.test.lib_handle);
      get_module_function(modules.test.lib_handle, "hello",       (void**) &modules.test.hello);
      get_module_function(modules.test.lib_handle, "sum_numbers", (void**) &modules.test.sum_numbers);
    }
}

/* This closes any loaded modules, run before program termination */
void
close_modules(void)
{
  if (modules.test.lib_handle)
    {
      dlclose(modules.test.lib_handle);
    }
}

/* Load a module, run some test functions, and exit */
int
main (int argc, char* argv[])
{
  /* Used later on */
  int ans;

  /* We don't want these variables */
  (void) argc;
  (void) argv;

  /* Initialise the module struct */
  init_modules ();

  /* Load the test module */
  load_module_test ();

  /* Test */
  printf("Should be hello world: ");
  (*modules.test.hello) ();
  printf("\n");

  (*modules.test.sum_numbers) (2, 2, &ans);
  printf("2 + 2 = %i\n", ans);

  /* Tidy up and exit */
  close_modules ();
  return 0;
}
