/*
 * Crystals (working title)
 *
 * * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following
 * disclaimer in the documentation and/or other materials provided
 * with the distribution.
 *
 * * The names of contributors may not be used to endorse or promote
 * products derived from this software without specific prior
 * written permission.
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

/** @file   src/main.c
 *  @author Matt Windsor
 *  @brief  Main functions.
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "main.h"
#include "util.h"
#include "object.h"
#include "module.h"
#include "events.h"
#include "graphics.h"
#include "bindings.h"
#include "mapview.h"
#include "map.h"

struct map *g_map;
struct map_view *g_mapview;

int g_running;

int
main (int argc, char **argv)
{
  /* Placeholder for command line stuff. */
  argc = argc;
  argv = argv;

  if (init () == SUCCESS)
    {
      g_running = TRUE;
      main_loop ();
    }
  cleanup ();
  return 0;
}

int
init (void)
{
  if (init_modules (DEFMODPATH) == FAILURE)
    {
      fprintf (stderr, "ERROR: Could not init modules!\n");
      return FAILURE;
    }

  if (init_graphics () == FAILURE)
    {
      fprintf (stderr, "ERROR: Could not init gfx!\n");
      return FAILURE;
    }

  if (init_objects () == FAILURE)
    {
      fprintf (stderr, "ERROR: Could not init objects!\n");
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

  init_bindings ();
  run_file ("tests/lua.lua"); 
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
      (*g_modules.event.process_events) ();
      handle_held_keys ();
    }
}

void
cleanup (void)
{
  cleanup_mapview (g_mapview);
  cleanup_map (g_map);
  cleanup_objects ();
  cleanup_events ();
  cleanup_graphics ();
  cleanup_bindings ();
  cleanup_modules ();
}

/* vim: set ts=2 sw=2 softtabstop=2: */
