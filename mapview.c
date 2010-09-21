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

/** @file    mapview.c
 *  @author  Matt Windsor
 *  @brief   Map rendering functions.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "mapview.h"
#include "map.h"
#include "module.h"
#include "graphics.h"
#include "main.h"

struct map_view *
init_mapview (struct map *map)
{
  if (map)
    {
      struct map_view *mapview;

      mapview = malloc (sizeof (struct map_view));

      if (mapview)
        {              
          mapview->x_offset = 0;
          mapview->y_offset = 0;
          mapview->map = map;

          mapview->dirty_tiles = malloc (sizeof (unsigned char)
                                         * (mapview->map->width * 
                                            mapview->map->height));

          if (mapview->dirty_tiles)
            {
              memset (mapview->dirty_tiles, mapview->map->num_layers,
                      sizeof (unsigned char) * (mapview->map->width * 
                                                mapview->map->height));
            }
          else
            {
              /* Can't have the mapview without the dirty tiles array. */
              free (mapview);
              mapview = NULL;
            }
        }

      return mapview;
    }
  else
    {
      fprintf (stderr, "ERROR: Couldn't init mapview: Map does not exist!\n");
      return NULL;
    }
}

void
render_map (struct map_view *mapview)
{
  /* FIXME: only render dirty tiles. */

  if (mapview)
    {
      if (mapview->map)
        {
          unsigned char l;

          /* Render a layer, then the objects tagged with that layer. */
          for (l = 0; l < mapview->map->num_layers; l++)
            {
              render_map_layer (mapview, l);
              /** @todo FIXME: render objects. */
            }
        }
    }
}

void
render_map_layer (struct map_view *mapview, unsigned char layer)
{
  short x, y;
  long true_x, true_y, x_offset, y_offset;
  struct map *map;

  map = mapview->map;
  x_offset = mapview->x_offset;
  y_offset = mapview->y_offset;

  /* The true x and y refer to the tiles on the map, whereas 
     standard x and y refer to the tiles on-screen. To 
     translate from x and y to true_x and true_y, we need 
     only apply the x and y offsets divided to convert from 
     pixel offsets to tile offsets.

     We also scan the tiles just outside screen bounds to 
     cater for tiles that are only partially on the screen. */

  for (x = -1; x < (SCREEN_W / TILE_W) + 1; x++)
    {
      true_x = x + (x_offset / TILE_W);

      if (true_x >= 0 && true_x < map->width)
        {
          for (y = -1; y < (SCREEN_H / TILE_H) + 1; y++)
            {
              true_y = y + (y_offset / TILE_H);

              if (true_y >= 0 && true_y < map->height
                  && mapview->dirty_tiles[true_x + (true_y * map->height)] != 0)
                {
                  draw_image (FN_TILESET,
                              map->data_layers[layer][true_x 
                                                      + (true_y * map->height)]
                              * TILE_W, 0,
                              (x * TILE_W) - (short) (x_offset % TILE_W),
                              (y * TILE_H) - (short) (y_offset % TILE_H),
                              TILE_W, TILE_H);

                  mapview->dirty_tiles[true_x + (true_y * map->height)]--;
                }
            }
        }
    }
}

void
scroll_map (struct map_view *mapview, int direction)
{
  int adirection;

  /* FIXME: Render damage. */
  switch (direction)
    {
    case NORTH:
      mapview->y_offset -= 1;
      mark_dirty_rect (mapview, 
                       mapview->x_offset / TILE_W,
                       mapview->y_offset / TILE_H,
                       SCREEN_W / TILE_W, 1);      
      adirection = SOUTH;
      break;
    case EAST:
      mapview->x_offset += 1;
      mark_dirty_rect (mapview, 
                       (SCREEN_W / TILE_W) + (mapview->x_offset / TILE_W) - 1,
                       mapview->y_offset / TILE_H,
                       1, SCREEN_H / TILE_H);
      adirection = WEST;
      break;
    case SOUTH:
      mapview->y_offset += 1;
      mark_dirty_rect (mapview, 
                       mapview->x_offset / TILE_W,
                       (SCREEN_H / TILE_H) + (mapview->y_offset / TILE_H) - 1,
                       SCREEN_W / TILE_W, 1);
      adirection = NORTH;
      break;
    case WEST:
      mapview->x_offset -= 1;
      mark_dirty_rect (mapview, 
                       mapview->x_offset / TILE_W,
                       mapview->y_offset / TILE_H,
                       1, SCREEN_H / TILE_H);
      adirection = EAST;
      break;
    }


  (*g_modules.gfx.scroll_screen) (adirection);
  render_map (mapview);
}

void
mark_dirty_rect (struct map_view *mapview,
                 int start_x, int start_y, int width, int height)
{
  long x, y;

  for (x = start_x; x < start_x + width; x++)
    {
      for (y = start_y; y < start_y + height; y++)
        {
          if (x >= 0
              && y >= 0
              && x < mapview->map->width
              && y < mapview->map->height)
            mapview->dirty_tiles[x + (y * mapview->map->width)] = \
              mapview->map->num_layers;
        }
    }
}

void
cleanup_mapview (struct map_view *mapview)
{
  if (mapview)
    {
      if (mapview->dirty_tiles)
        free (mapview->dirty_tiles);

      free (mapview);
    }
}
