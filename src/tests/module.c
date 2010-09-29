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

/** @file    src/tests/module.c
 *  @author  Michael Walker
 *  @brief   Sanity-test for the module loader.
 */

#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <string.h>

#include "module.h"
#include "../util.h"
#include "../module.h"

test_module_set g_test_modules; /**< The set of all modules in use. */

/* This initialises the struct of modules to NULL and sets the load path. */

int
init_test_modules (const char *path)
{
  g_test_modules.path = malloc (sizeof (char) * (strlen (path) + 1));

  if (g_test_modules.path)
    {
      strncpy (g_test_modules.path, path, strlen (path) + 1);
      
      module_bare_init (&g_test_modules.test.metadata);
      module_bare_init (&g_test_modules.foo.metadata);
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
cleanup_test_modules (void)
{
  close_module (&g_test_modules.test.metadata);
  close_module (&g_test_modules.foo.metadata);

  free (g_test_modules.path);
}

/* Function to load 'test' module. */

int
load_module_test (test_module_set* modules)
{
  if (get_module_by_name  ("test", modules->path, &modules->test.metadata) == FAILURE) return FAILURE;
  if (get_module_function (modules->test.metadata, "sum_numbers",
                           (void**) &modules->test.sum_numbers) == FAILURE) return FAILURE;
  if (get_module_function (modules->test.metadata, "mul_numbers",
                           (void**) &modules->test.mul_numbers) == FAILURE) return FAILURE;
  
  return SUCCESS;
}

/* Function to load 'foo' module. */

int
load_module_foo (test_module_set* modules)
{
  if (get_module_by_name ("foo", modules->path, &modules->foo.metadata) == FAILURE) return FAILURE;
  if (get_module_function(modules->foo.metadata, "bar",
                          (void**) &modules->foo.bar) == FAILURE) return FAILURE;
  
  return SUCCESS;
}

/** Test the module loader.
 *  - Load a module and try to use it
 *  - Load a nonexistant module and try to use it
 *
 *  @param argc  Argument count.
 *  @param argv  Argument vector.
 *
 *  @return 0 is the test succeded, 1 otherwise.
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
  init_test_modules (MODPATH);

  /* Load the modules */
  printf ("Loading 'test' module\n");
  if (load_module_test (&g_test_modules) == FAILURE) return 1;

  printf ("Loading 'foo' module\n");
  if (load_module_foo (&g_test_modules) == SUCCESS) return 1;

  /* Run test functions */
  (*g_test_modules.test.sum_numbers) (2, 3, &ans);
  if (ans != 5) return 1;

  ans = (*g_test_modules.test.mul_numbers) (4, 5);
  if (ans != 20) return 1;
  
  /* Tidy up and close */
  cleanup_test_modules ();
  return 0;
}
