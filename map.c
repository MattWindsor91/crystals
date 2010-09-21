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

/** @file    map.c
 *  @author  Matt Windsor
 *  @brief   Low-level map handling functions.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "map.h"
#include "main.h"
#include "graphics.h"

const char FN_TILESET[] = "tiles.png";

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
          memcpy (map->data_layers[i], sg_test_layers[i],
                  sizeof (unsigned short) * (map->width * map->height + 1));
        }
    }

  return map;
}

/* Initialise a map. */

struct map *
init_map (unsigned int width, 
          unsigned int height, 
          unsigned char num_layers)
{
  struct map *map;

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
                 data). */
              map->data_layers[i] = malloc (sizeof (layer_t) * 
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
