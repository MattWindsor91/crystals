/* tests/module - a sanity-test for the module loader */

#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <string.h>

#include "../main.h"
#include "../module.h"
#include "module.h"

/* This initialises the struct of modules to NULL and sets the load path */
int
init_modules (const char *path)
{
  g_modules.path = malloc (sizeof (char) * (strlen (path) + 1));

  if (g_modules.path)
    {
      strncpy (g_modules.path, path, strlen (path) + 1);
      
      module_bare_init (&g_modules.test.metadata);
      module_bare_init (&g_modules.foo.metadata);
      
      g_modules.test.hello       = NULL;
      g_modules.test.sum_numbers = NULL;
      g_modules.foo.bar          = NULL;
      return SUCCESS;
    }
  else
    {
      fprintf (stderr, "ERROR: Couldn't alloc modules path!\n");
    }

  return FAILURE;
}

/* This closes any loaded modules, run before program termination */
void
cleanup_modules (void)
{
  close_module (&g_modules.test.metadata);
  close_module (&g_modules.foo.metadata);
}

/* Function to load 'test' module. */
void
load_module_test (void)
{
  if (g_modules.test.metadata.lib_handle == NULL)
    {
      get_module_by_name  ("test", &g_modules.test.metadata);
      get_module_function (g_modules.test.metadata, "hello",       (void**) &g_modules.test.hello);
      get_module_function (g_modules.test.metadata, "sum_numbers", (void**) &g_modules.test.sum_numbers);
    }
}

/* Function to load 'foo' module. */
void
load_module_foo (void)
{
  if (g_modules.foo.metadata.lib_handle == NULL)
    {
      get_module_by_name ("foo", &g_modules.foo.metadata);
      /* Normally, the program would have terminated before reaching this line */
      get_module_function(g_modules.foo.metadata, "bar", (void**) &g_modules.foo.bar);
    }
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
  (*g_modules.test.hello) ();
  (*g_modules.test.sum_numbers) (2, 3, &ans);
  printf("\n    2 + 3 = %i\n", ans);
  
  /* Tidy up and close */
  cleanup_modules ();
  return 0;
}
