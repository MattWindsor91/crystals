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
#include "../util.h"
#include "../graphics.h"


const char FN_TILESET[] = "gfx/tiles.png";

/* FIXME: Remove the data below when map loaders are available. */

static tag_t sg_test_tags[4] = {0, 1, 2, 0};

static layer_t sg_test_layers[4][100] = 
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

      for (i = 0; i < map->num_layers; i++) 
        {
          map->layer_tags[i] = sg_test_tags[i];

          /* We can assume that (map->width * map->height) fits in unsigned */
          memcpy (map->data_layers[i], sg_test_layers[i],
                  sizeof (layer_t) * (unsigned int) (map->width * map->height));
        }
    }

  return map;
}


/* Allocate and initialise a map. */

map_t *
init_map (int width, 
          int height, 
          unsigned char num_layers)
{
  map_t *map;
  unsigned int i;


  /* Sanity check */

  if (width <= 0 || height <= 0 || num_layers == 0)
    {
      error ("MAP - init_map - Given a zero or negative parameter.\nWidth: %d; Height: %d; #Layers: %d.",
               width, height, num_layers);
      return NULL;
    }

  map = malloc (sizeof (map_t));

  if (map == NULL)
    {
      error ("MAP - init_map - Map allocation failed.");
      return NULL;
    }

  map->layer_tags = calloc (num_layers, sizeof (tag_t));

  if (map->layer_tags == NULL)
    {
      error ("MAP - init_map - Map tag array allocation failed.");
      cleanup_map (map);
      return NULL;
    }

  map->data_layers = calloc (num_layers, sizeof (layer_t*));

  if (map->data_layers == NULL)
    {
      error ("MAP - init_map - Map data matrix allocation failed.");
      cleanup_map (map);
      return NULL;
    }

  map->width = width;
  map->height = height;
  map->num_layers = num_layers;

  for (i = 0; i < (num_layers); i++)
    {
      map->data_layers[i] = malloc (sizeof (layer_t) * (unsigned int) (width * height));
      if (map->data_layers[i] == NULL)
        {
          error ("MAP - init_map - Map data matrix row %d allocation failed.", i);
          cleanup_map (map);
        }
    }

  return map;
}


/* Get the tag associated with a layer. */

tag_t
get_tag (map_t *map, unsigned int layer)
{
  if (map == NULL)
    {
      error ("MAP - get_tag - Tried to get tag for a NULL map.");
      return NULL_TAG;
    }

  if (layer >= map->num_layers)
    {
      error ("MAP - get_tag - Tried to get tag for an invalid layer.");
      return NULL_TAG;
    }

  return map->layer_tags[layer];
}


/* Get the highest tag number allocated on a map. */

tag_t
get_max_tag (map_t *map)
{
  unsigned int l;
  tag_t result;

  result = NULL_TAG;

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

          if (map->layer_tags)
            free (map->layer_tags);

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
