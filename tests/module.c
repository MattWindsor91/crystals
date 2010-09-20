/* tests/module - a sanity-test for the module loader */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>
#include "../module.h"
#include "module.h"

/* The test module functions - in the main module loader these would reflect the
   modules used in the real engine */
void
init_modules (const char *path)
{
  modules.path = strdup (path);

  module_bare_init (&modules.test.metadata);
  module_bare_init (&modules.foo.metadata);

  modules.test.hello       = NULL;
  modules.test.sum_numbers = NULL;
  modules.foo.bar          = NULL;
}

void
close_modules (void)
{
  close_module(&modules.test.metadata);
  close_module(&modules.foo.metadata);
}

/* Function to load 'test' module. */
void
load_module_test (void)
{
  /* Get the path of the module */
  char *modulepath;
  get_module_path ("test", &modulepath);

  if (modules.test.metadata.lib_handle == NULL)
    {
      get_module          (modulepath, &modules.test.metadata);
      get_module_function (modules.test.metadata, "hello",       (void**) &modules.test.hello);
      get_module_function (modules.test.metadata, "sum_numbers", (void**) &modules.test.sum_numbers);
    }

  free(modulepath);
}

/* Function to load 'foo' module. */
void
load_module_foo (void)
{
  char *modulepath;
  get_module_path ("foo", &modulepath);

  if (modules.foo.metadata.lib_handle == NULL)
    {
      get_module (modulepath, &modules.foo.metadata);
      /* Normally, the program would have terminated before reaching this line */
      get_module_function(modules.foo.metadata, "bar", (void**) &modules.foo.bar);
    }

  free(modulepath);
}

/* Test the module loader:
   - Load a module and try to use it
   - Load a nonexistant module and try to use it
*/
int
main (int argc, char *argv[])
{
  /* Used in testing: */
  int ans;

  /* Don't want these */
  (void) argc;
  (void) argv;

  /* Initialise the module loader */
  init_modules (MODPATH);

  /* Load the modules */
  printf ("Loading 'test' module\n");
  load_module_test ();

  printf ("Loading 'foo' module\n");
  load_module_foo ();

  /* Run test functions */
  printf ("Testing 'test' module:\n");
  printf ("    Should be hello world: ");
  (*modules.test.hello) ();
  (*modules.test.sum_numbers) (2, 3, &ans);
  printf("\n    2 + 3 = %i\n", ans);
  
  /* Tidy up and close */
  close_modules ();
  return 0;
}
