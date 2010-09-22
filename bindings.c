#include <stdlib.h>
#include <stdio.h>

#include "main.h"
#include "module.h"
#include "bindings.h"

int
init_bindings (void)
{
  if (load_module_bindings ("bindings-python") == FAILURE)
    {
      fprintf (stderr, "ERROR: Could not load bindings module.\n");
      return FAILURE;
    }

  return SUCCESS;
}
