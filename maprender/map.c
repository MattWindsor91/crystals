#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "map.h"
#include "graphics.h"

const char FN_TILESET[] = "tiles.png";

static unsigned char s_test_layers[4][100] = 
  {{ 9,  5,  5,  5,  5,  5,  5,  5,  5, 10, 
     8,  2,  2,  2,  2,  2,  2,  2,  2,  7,
     8, 13, 13, 13, 13, 13, 13, 13, 13,  7,
     8, 13, 13, 13, 13, 13, 13, 13, 13,  7,
     8, 13, 13, 13, 13, 13, 13, 13, 13,  7,
     8, 13, 13, 13, 13, 13, 13, 13, 13,  7,
     8, 13, 13, 13, 13, 13, 13, 13, 13,  7,
     8, 13, 13, 13, 13, 13, 13, 13, 13,  7,
    11, 13, 13, 13, 13, 13, 13, 13, 13, 12,
     1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
   { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  3,  3,  3,  3,  3,  3,  3,  3,  0,
     0, 14, 14, 14, 14, 14, 14, 14, 14,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0}, 
   { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  3,  3,  3,  3,  3,  3,  3,  3,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0}, 
   { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
     0,  0,  0,  0,  4,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  4,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  4,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  4,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  4,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  4,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  4,  0,  0,  0,  0,  0,
     0,  6,  6,  6,  6,  6,  6,  6,  6,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0} 
  };

struct Map *
init_test_map (void)
{
  struct Map *map;

  map = init_map (10, 10, 2, 2);

  if (map)
    {
      unsigned int i;
      for (i = 0; i < 4; i++) 
        {
          memcpy (map->data_layers[i], s_test_layers[i],
                  sizeof (unsigned char) * map->width * map->height);
        }
    }

  return map;
}

struct Map *
init_map (unsigned int width, 
          unsigned int height, 
          unsigned int num_ground_layers, 
          unsigned int num_overlay_layers)
{
  struct Map *map;

  map = malloc (sizeof (struct Map));

  if (map) 
    {
      map->width = width;
      map->height = height;
      map->num_ground_layers = num_ground_layers;
      map->num_overlay_layers = num_overlay_layers;

      map->data_layers = malloc (sizeof (unsigned char*) * 
                                 (num_ground_layers + num_overlay_layers));

      if (map->data_layers)
        {
          unsigned int i;

          for (i = 0; i < (num_ground_layers + num_overlay_layers); i++)
            {
              map->data_layers[i] = malloc (sizeof (unsigned char) * 
                                            (width * height));
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

void
cleanup_map (struct Map *map)
{
  if (map)
    {
      if (map->data_layers)
        {
          unsigned int i;

          for (i = 0; i < (map->num_ground_layers
                           + map->num_overlay_layers); i++)
            {
              if (map->data_layers[i])
                free (map->data_layers[i]);
            }
        }
      free (map);
    }
}

void
render_map (struct Map *map)
{
  /* FIXME: only render dirty tiles. */

  if (map)
    {
      unsigned int l;

      /* Render ground layers. */
      for (l = 0; l < map->num_ground_layers; l++)
        {
          render_map_layer (map, l);
        }

      /* FIXME: render objects. */

      /* Render overlay layers. */
      for (l = map->num_ground_layers;
           l < map->num_ground_layers + map->num_overlay_layers; l++)
        {
          render_map_layer (map, l);
        }
    }
}

void
render_map_layer (struct Map *map, unsigned int layer)
{
  unsigned int x, y;

  /* FIXME: scrolling offsets. */

  for (x = 0; x < map->width; x++)
    {
      for (y = 0; y < map->height; y++)
        {
          draw_image (FN_TILESET,
                      map->data_layers[layer][x + (y * map->height)] * TILE_W, 
                      0, x * TILE_W, y * TILE_H, TILE_W, TILE_H);
        }
    }
}
