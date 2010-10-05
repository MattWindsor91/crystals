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

/** @file    src/field/map.c
 *  @author  Matt Windsor
 *  @brief   Low-level map handling functions.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "map.h"
#include "../graphics.h"

const char FN_TILESET[] = "gfx/tiles.png";

/* FIXME: Remove the data below when map loaders are available. */

static layer_t sg_test_layers[4][101] = 
  {{ 9,  5,  5,  5,  5,  5,  5,  5,  5, 10, 
     8,  2,  2,  2,  2,  2,  2,  2,  2,  7,
     8, 13, 13, 13, 13, 13, 13, 13, 13,  7,
     8, 13, 13, 13, 13, 13, 13, 13, 13,  7,
     8, 13, 13, 13, 13, 13, 13, 13, 13,  7,
     8, 13, 13, 13, 13, 13, 13, 13, 13,  7,
     8, 13, 13, 13, 13, 13, 13, 13, 13,  7,
     8, 13, 13, 13, 13, 13, 13, 13, 13,  7,
    11, 13, 13, 13, 13, 13, 13, 13, 13, 12,
     1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 0},
   { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  3,  3,  3,  3,  3,  3,  3,  3,  0,
     0, 14, 14, 14, 14, 14, 14, 14, 14,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 1}, 
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
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 2} 
  };

/* Initialise the test map. */

/* NOTE: This should be removed when map loading is implemented! */

struct map *
init_test_map (void)
{
  struct map *map;

  map = init_map (10, 10, 4);

  if (map)
    {
      unsigned int i;
      for (i = 0; i < 4; i++) 
        {
          /* We can assume that (map->width * map->height + 1) fits in unsigned */
          memcpy (map->data_layers[i], sg_test_layers[i],
                  sizeof (unsigned short)
                  * (unsigned int) (map->width * map->height + 1));
        }
    }

  return map;
}

/* Initialise a map. */

struct map *
init_map (int width, 
          int height, 
          unsigned char num_layers)
{
  struct map *map;

  /* Sanity check */

  if (width <= 0 || height <= 0 || num_layers == 0)
    {
      fprintf (stderr, "MAP: Error: init_map given a zero or negative parameter.\n");
      fprintf (stderr, "Width: %d; Height: %d; #Layers: %d\n",
               width, height, num_layers);
      return NULL;
    }

  map = malloc (sizeof (struct map));

  if (map) 
    {
      map->width = width;
      map->height = height;
      map->num_layers = num_layers;

      map->data_layers = malloc (sizeof (layer_t*) * num_layers);

      if (map->data_layers)
        {
          unsigned int i;

          for (i = 0; i < (num_layers); i++)
            {
              /* Allocate one extra slot for the layer tag (at the end of the
                 data). Assert that width and height are positive. */
              map->data_layers[i] = malloc (sizeof (layer_t)
                                            * (unsigned int) ((width * height)
                                                              + 1));
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

layer_t
get_tag (struct map *map, unsigned int layer)
{
  if (map == NULL)
    {
      fprintf (stderr, "MAP: Error: Tried to get tag for a NULL map.\n");
      return 0;
    }

  if (layer >= map->num_layers)
    {
      fprintf (stderr, "MAP: Error: Tried to get tag for an invalid layer.\n");
      return 0;
    }

  return map->data_layers[layer][map->width * map->height];
}

/* Get the highest tag number allocated on a map. */

layer_t
get_max_tag (struct map *map)
{
  unsigned int l;
  layer_t result;

  result = 0;

  if (map)
    {
      for (l = 0; l < map->num_layers; l++)
        {
          if (get_tag (map, l) > result)
            result = get_tag (map, l);
        }
    }

  return result;
}

/* De-initialise a map. */

void
cleanup_map (struct map *map)
{
  if (map)
    {
      /* Make sure to clean up all the data layers. */
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
