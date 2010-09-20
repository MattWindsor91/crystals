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
          unsigned char num_layers)
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

void
cleanup_map (struct Map *map)
{
  if (map)
    {
      if (map->data_layers)
        {
          unsigned int i;

          for (i = 0; i < map->num_layers; i++)
            {
              if (map->data_layers[i])
                free (map->data_layers[i]);
            }

          free (map->data_layers);
        }
      free (map);
    }
}
