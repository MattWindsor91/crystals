#include <stdio.h>

#include "main.h"
#include "events.h"
#include "graphics.h"
#include "map.h"

struct Map *g_map;

int g_running;

int
main (int argc, char **argv)
{
  if (init ())
    {
      g_running = 1;
      main_loop ();
      cleanup ();
    }
}

int
init (void)
{
  if (init_graphics ())
    {
      g_map = init_test_map ();

      if (g_map)
        {
          return 1;
        }
      else
        fprintf (stderr, "ERROR: Map did not init!\n");
    }
  else
    fprintf (stderr, "ERROR: Could not init gfx!\n");

  return 0;
}

void
main_loop (void)
{
  while (g_running)
    {
      render_map (g_map);
      update_screen ();
      handle_events ();
    }
}

void
cleanup (void)
{
  cleanup_map (g_map);
  cleanup_graphics ();
}
