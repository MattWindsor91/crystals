#include <stdlib.h>
#include <stdio.h>

#include "main.h"
#include "events.h"
#include "graphics.h"
#include "map.h"

struct Map *g_map;
struct MapView *g_mapview;

int g_running;

int
main (int argc, char **argv)
{
  if (init ())
    {
      g_running = 1;
      main_loop ();
    }
  cleanup ();
}

int
init (void)
{
  if (init_graphics ())
    {
      g_map = init_test_map ();

      if (g_map)
        {
          g_mapview = init_mapview (g_map);

          if (g_mapview)
            {
              init_events ();

              return 1;
            }
          else
            {
              fprintf (stderr, "ERROR: Map view did not init!\n");
            }
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
  render_map (g_mapview);

  while (g_running)
    {
      update_screen ();
      handle_events ();
    }
}

void
cleanup (void)
{
  cleanup_mapview (g_mapview);
  cleanup_map (g_map);
  cleanup_graphics ();
}
