#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "main.h"
#include "module.h"
#include "events.h"
#include "graphics.h"
#include "mapview.h"
#include "map.h"

struct map *g_map;
struct map_view *g_mapview;

int g_running;

int
main (int argc, char **argv)
{
  if (init () == SUCCESS)
    {
      g_running = TRUE;
      main_loop ();
    }
  cleanup ();
}

int
init (void)
{
  if (init_modules (MODPATH) == FAILURE)
    {
      fprintf (stderr, "ERROR: Could not init modules!\n");
      return FAILURE;
    }

  if (init_graphics () == FAILURE)
    {
      fprintf (stderr, "ERROR: Could not init gfx!\n");
      return FAILURE;
    }

  g_map = init_test_map ();

  if (g_map == NULL)
    {
      fprintf (stderr, "ERROR: Map did not init!\n");
      return FAILURE;
    }

  g_mapview = init_mapview (g_map);

  if (g_mapview == NULL)
    {
      fprintf (stderr, "ERROR: Map view did not init!\n");
      return FAILURE;
    }

  init_events ();

  return SUCCESS;
}



void
main_loop (void)
{
  render_map (g_mapview);

  while (g_running)
    {
      (*g_modules.gfx.update_screen) ();
      handle_events ();
    }
}

void
cleanup (void)
{
  cleanup_mapview (g_mapview);
  cleanup_map (g_map);
  cleanup_graphics ();
  cleanup_modules ();
}


