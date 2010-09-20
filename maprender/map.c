#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "map.h"
#include "main.h"
#include "graphics.h"

const char FN_TILESET[] = "tiles.png";

static unsigned char s_test_layers[4][101] = 
  {{ 9,  5,  5,  5,  5,  5,  5,  5,  5, 10, 
     8,  2,  2,  2,  2,  2,  2,  2,  2,  7,
     8, 13, 13, 13, 13, 13, 13, 13, 13,  7,
     8, 13, 13, 13, 13, 13, 13, 13, 13,  7,
     8, 13, 13, 13, 13, 13, 13, 13, 13,  7,
     8, 13, 13, 13, 13, 13, 13, 13, 13,  7,
     8, 13, 13, 13, 13, 13, 13, 13, 13,  7,
     8, 13, 13, 13, 13, 13, 13, 13, 13,  7,
    11, 13, 13, 13, 13, 13, 13, 13, 13, 12,
     1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 1},
   { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  3,  3,  3,  3,  3,  3,  3,  3,  0,
     0, 14, 14, 14, 14, 14, 14, 14, 14,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0}, 
   { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  3,  3,  3,  3,  3,  3,  3,  3,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0}, 
   { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
     0,  0,  0,  0,  4,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  4,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  4,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  4,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  4,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  4,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  4,  0,  0,  0,  0,  0,
     0,  6,  6,  6,  6,  6,  6,  6,  6,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0} 
  };

struct Map *
init_test_map (void)
{
  struct Map *map;

  map = init_map (10, 10, 4);

  if (map)
    {
      unsigned int i;
      for (i = 0; i < 4; i++) 
        {
          memcpy (map->data_layers[i], s_test_layers[i],
                  sizeof (unsigned char) * (map->width * map->height + 1));
        }
    }

  return map;
}

struct Map *
init_map (unsigned int width, 
          unsigned int height, 
          unsigned int num_layers)
{
  struct Map *map;

  map = malloc (sizeof (struct Map));

  if (map) 
    {
      map->width = width;
      map->height = height;
      map->num_layers = num_layers;

      map->data_layers = malloc (sizeof (unsigned char*) * num_layers);

      if (map->data_layers)
        {
          unsigned int i;

          for (i = 0; i < (num_layers); i++)
            {
              /* Allocate one extra char for the layer tag (at the end of the
                 data). */
              map->data_layers[i] = malloc (sizeof (unsigned char) * 
                                            ((width * height) + 1));
            }
        }
      else
        {
          /* Clean up the entire thing, as a map is useless without
             its data layers. */

          free (map);
          map = NULL;
        }
    }

  return map;
}

struct MapView *
init_mapview (struct Map *map)
{
  struct MapView *mapview;

  mapview = malloc (sizeof (struct MapView));

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

void
render_map (struct MapView *mapview)
{
  /* FIXME: only render dirty tiles. */

  if (mapview)
    {
      if (mapview->map)
        {
          struct Map *map;
          unsigned int l;

          map = mapview->map;

          /* Test! *shakes fist*
             fill_screen (0, 0, 0); */

          /* Render a layer, then the objects tagged with that layer. */
          for (l = 0; l < map->num_layers; l++)
            {
              render_map_layer (mapview, l);
              /* FIXME: render objects. */
            }
        }
    }
}

void
render_map_layer (struct MapView *mapview, unsigned int layer)
{
  int x, y, true_x, true_y, x_offset, y_offset;
  struct Map *map;

  map = mapview->map;
  x_offset = mapview->x_offset;
  y_offset = mapview->y_offset;

  /* FIXME: scrolling offsets. */

  for (x = -1; x < (SCREEN_W / TILE_W) + 1; x++)
    {
      true_x = x - (x_offset / TILE_W);

      if (true_x >= 0 && true_x < map->width)
        {
          for (y = -1; y < (SCREEN_H / TILE_H) + 1; y++)
            {
              true_y = y - (y_offset / TILE_H);

              if (true_y >= 0 && true_y < map->height
                  && mapview->dirty_tiles[true_x + (true_y * map->height)] != 0)
                {
                  draw_image (FN_TILESET,
                              map->data_layers[layer][true_x 
                                                      + (true_y * map->height)]
                              * TILE_W, 0,
                              (x * TILE_W) + (x_offset % TILE_W),
                              (y * TILE_H) + (y_offset % TILE_H),
                              TILE_W, TILE_H);

                  mapview->dirty_tiles[true_x + (true_y * map->height)]--;
                }
            }
        }
    }
}

void
scroll_map (struct MapView *mapview, int direction)
{
  /* FIXME: Render damage. */
  switch (direction)
    {
    case NORTH:
      mapview->y_offset -= 1;
      mark_dirty_rect (mapview, 
                       -mapview->x_offset / TILE_W,
                       (SCREEN_H / TILE_H) - (mapview->y_offset / TILE_H) - 1,
                       SCREEN_W / TILE_W, 1);
      break;
    case EAST:
      mapview->x_offset += 1;
      mark_dirty_rect (mapview, 
                       -mapview->x_offset / TILE_W,
                       -mapview->y_offset / TILE_H,
                       1, SCREEN_H / TILE_H);
      break;
    case SOUTH:
      mapview->y_offset += 1;
      mark_dirty_rect (mapview, 
                       -mapview->x_offset / TILE_W,
                       -mapview->y_offset / TILE_H,
                       SCREEN_W / TILE_W, 1);
      break;
    case WEST:
      mapview->x_offset -= 1;
      mark_dirty_rect (mapview, 
                       (SCREEN_W / TILE_W) - (mapview->x_offset / TILE_W) - 1,
                       -mapview->y_offset / TILE_H,
                       1, SCREEN_H / TILE_H);
      break;
    }


  scroll_screen (direction);
  render_map (mapview);
}

void
mark_dirty_rect (struct MapView *mapview,
                 int start_x, int start_y, int width, int height)
{
  int x, y;

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
cleanup_map (struct Map *map)
{
  if (map)
    {
      if (map->data_layers)
        {
          unsigned int i;

          for (i = 0; i < (map->num_layers); i++)
            {
              if (map->data_layers[i])
                free (map->data_layers[i]);
            }
        }
      free (map);
    }
}

void
cleanup_mapview (struct MapView *mapview)
{
  if (mapview)
    {
      if (mapview->dirty_tiles)
        free (mapview->dirty_tiles);

      free (mapview);
    }
}
