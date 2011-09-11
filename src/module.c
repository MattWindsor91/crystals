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

/** @file    src/module.c
 *  @author  Michael Walker
 *  @brief   Functions to handle loading and unloading of the modules
 *           that make up the engine.
 */

#include "crystals.h"

#ifdef TESTSUITE
#include "tests/module.h"
#endif /* TESTSUITE */

module_set g_modules; /* The set of all modules in use */


/* This initialises the struct of modules to NULL and sets the load path */

void
init_modules (const char *path)
{
  g_assert (g_module_supported () == TRUE);
  
  g_modules.path = xcalloc (strlen (path) + 1, sizeof (char));

  strncpy (g_modules.path, path, strlen (path) + 1);
  
  module_bare_init (&g_modules.gfx.metadata);
  module_bare_init (&g_modules.event.metadata);
}


/* This does the bare minimum initialisation for a module */

void
module_bare_init (module_data *module)
{
  module->lib_handle = NULL;
  module->init       = NULL;
  module->term       = NULL;
}


/* Get the path to the directory in which all modules are stored. */

void
get_module_root_path (char **module_path)
{
  /* If a string exists here, free it. */

  if (*module_path != NULL)
    {
      free (module_path);
      *module_path = NULL;
    }


  /* If configuration loading succeeded, try to grab the module path
     from the config file.  If this doesn't work, use the default
     path. */

  if (g_config)
    *module_path = cfg_get_str ("modules", "module_path", g_config);


  if (*module_path == NULL)
    {
      error ("UTIL - get_module_path - Cannot read module path from config.");

      /* Copy the default path to the pointer. */

      *module_path = xcalloc (strlen (DEFMODPATH) + 1, sizeof (char));

      strncpy (*module_path, DEFMODPATH, strlen (DEFMODPATH) + 1);
    }
}

/* This gets the path of a module, storing it in out */

void
get_module_path (const char* module, const char* modulespath, char** out)
{
  char *path;

  path = xcalloc (strlen (modulespath) + strlen (module)
                 + 1, sizeof (char));

  strncpy (path, modulespath, strlen (modulespath));
  strncat (path, module, strlen (module));

  *out = path;
}


/* This finds the filename of a module and calls get_module */

bool_t
get_module_by_name (const char* name, const char *modulespath, module_data *module)
{
  bool_t out;        /* Soon-to-be return value of get_module */
  char *modulepath;  /* Buffer in which to store path to the module. */

  get_module_path (name, modulespath, &modulepath);

  g_assert (modulepath != NULL);

  /* And get the module */

  out = get_module (modulepath, module);
  free (modulepath);
  return out;
}


/* This opens a module file and runs any init code */

bool_t
get_module (const char* modulepath, module_data *module)
{
  g_assert (module->lib_handle == NULL);

  module->lib_handle = g_module_open(modulepath, 0);

  if (module->lib_handle == NULL)
    {
      get_dll_error ("get_module");
      return FAILURE;
    }

  /* Get init and termination functions if present */

  get_module_function (*module, "init", (mod_function_ptr*) &module->init);
  get_module_function (*module, "term", (mod_function_ptr*) &module->term);


  /* Execute init function if present */

  if (module->init != NULL)
    (*module->init) ();

  return SUCCESS;
}


/* Raise the last DLL-acquisition error message, if any. */

void
get_dll_error (const char function_name[])
{
  const char *dlerr = g_module_error ();

  if (dlerr != NULL)
    error ("MODULE - %s - Failed with dlerr: %s", function_name, dlerr);
}



/* This loads a pointer to a function from a module */

bool_t
get_module_function (module_data module, const char *function, mod_function_ptr *func)
{
  gboolean success;
  
  success = g_module_symbol (module.lib_handle, function, func);

  if (!success)
    {
      get_dll_error ("get_module_function");
      return FAILURE;
    }

  return SUCCESS;
}


/* Load a graphics module. */

bool_t
load_module_gfx (const char* name, module_set* modules)
{
  if (get_module_by_name (name, modules->path, &modules->gfx.metadata) == FAILURE)
    return FAILURE;
  
  if (get_module_function (modules->gfx.metadata,
                           "init_screen_internal",
                           (mod_function_ptr*)
                           &modules->gfx.init_screen_internal)
      == FAILURE)
    return FAILURE;
  
  if (get_module_function (modules->gfx.metadata,
                           "draw_rect_internal",
                           (mod_function_ptr*)
                           &modules->gfx.draw_rect_internal)
      == FAILURE)
    return FAILURE;
  
  if (get_module_function (modules->gfx.metadata, "load_image_data",
                           (mod_function_ptr*)
                           &modules->gfx.load_image_data) == FAILURE)
    return FAILURE;
  
  if (get_module_function (modules->gfx.metadata, "free_image_data",
                           (mod_function_ptr*)
                           &modules->gfx.free_image_data) == FAILURE)
    return FAILURE;
  
  if (get_module_function (modules->gfx.metadata,
                           "draw_image_internal",
                           (mod_function_ptr*)
                           &modules->gfx.draw_image_internal)
      == FAILURE)
    return FAILURE;
 
  if (get_module_function (modules->gfx.metadata,
                           "add_update_rectangle_internal",
                           (mod_function_ptr*)
                           &modules->gfx.add_update_rectangle_internal) 
      == FAILURE)
    return FAILURE;
  
  if (get_module_function (modules->gfx.metadata,
                           "update_screen_internal",
                           (mod_function_ptr*)
                           &modules->gfx.update_screen_internal) 
      == FAILURE)
    return FAILURE;
  
  if (get_module_function (modules->gfx.metadata,
                           "scroll_screen_internal",
                           (mod_function_ptr*)
                           &modules->gfx.scroll_screen_internal)
      == FAILURE)
    return FAILURE;
  
  return SUCCESS;
}


/* Load an event module. */

bool_t
load_module_event (const char *name, module_set *modules)
{
  if (get_module_by_name (name, modules->path, &modules->event.metadata)
      == FAILURE)
    return FAILURE;
  
  if (get_module_function (modules->event.metadata,
                           "process_events_internal",
                           (mod_function_ptr*)
                           &modules->event.process_events_internal)
      == FAILURE)
    return FAILURE;

  if (get_module_function (modules->event.metadata,
                           "register_release_handle",
                           (mod_function_ptr*)
                           &modules->event.register_release_handle)
      == FAILURE)
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
      /* Close the lib handle */
      g_module_close (module->lib_handle);
    }
}


/* This closes any loaded modules, run before program termination */

void
cleanup_modules (void)
{
  close_module (&g_modules.event.metadata);
  close_module (&g_modules.gfx.metadata);

  free (g_modules.path);
}
/* vim: set ts=2 sw=2 softtabstop=2: */
