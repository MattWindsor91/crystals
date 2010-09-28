#include <stdlib.h>
#include <stdio.h>

#include "util.h"
#include "module.h"
#include "bindings.h"

int
init_bindings (void)
{
  if (load_module_bindings ("bindings-lua", &g_modules) == FAILURE)
    {
      fprintf (stderr, "ERROR: Could not load bindings module.\n");
      return FAILURE;
    }

  return SUCCESS;
}

int
run_file(const char *path)
{
  return (*g_modules.bindings.run_file) (path);
}

void
cleanup_bindings (void)
{
}

/* vim: set ts=2 sw=2 softtabstop=2: */
