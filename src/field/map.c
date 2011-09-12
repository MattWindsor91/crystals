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


#include "../crystals.h"


const char FN_TILESET[] = "tiles.png";

/* -- STATIC DEFINITIONS -- */

/**
 * Allocate arrays to contain layer planes in the map.
 *
 * @param map  The map to populate.
 */
static void
allocate_plane_arrays (map_t *map);


/**
 * Allocate arrays to contain layer planes in the map.
 *
 * @param map  The map to populate.
 */
static void
allocate_planes (map_t *map);


/* -- DECLARATIONS -- */

/* Initialise a map. */
void
init_map (map_t *map,
          dimension_t width,
          dimension_t height,
          layer_index_t max_layer_index,
          zone_index_t max_zone_index)
{
  g_assert (width > 0 && height > 0);

  /* Set parameters and nullify pointers. */

  map->width = width;
  map->height = height;
  map->max_layer_index = max_layer_index;
  map->max_zone_index = max_zone_index;

  map->layer_tags = NULL;
  map->zone_properties = NULL;
  map->value_planes = NULL;
  map->zone_planes = NULL;


  /* Allocate tag array. */

  map->layer_tags = xcalloc ((size_t) max_layer_index + 1, sizeof (layer_tag_t));

  /* Allocate properties array. */

  map->zone_properties = xcalloc ((size_t) max_zone_index + 1, sizeof (zone_prop_t));

  allocate_plane_arrays (map);
  allocate_planes (map);
}


/* -- STATIC DEFINITIONS -- */

/* Allocate arrays to contain layer planes in the map. */
static void
allocate_plane_arrays (map_t *map)
{
  map->value_planes = xcalloc ((size_t) map->max_layer_index + 1,
                              sizeof (layer_value_t *));

  map->zone_planes = xcalloc ((size_t) map->max_layer_index + 1,
                             sizeof (layer_zone_t *));
}


/* Allocate planes for each layer in the map. */
static void
allocate_planes (map_t *map)
{
  int i;
  for (i = 0; i <= map->max_layer_index; i++)
    {
      map->value_planes[i] = xcalloc ((size_t) map->width * map->height,
                                     sizeof (layer_value_t));

      map->zone_planes[i] = xcalloc ((size_t) map->width * map->height,
                                    sizeof (layer_zone_t));
    }
}


/* Get the tag associated with a layer. */

layer_tag_t
get_layer_tag (map_t *map, layer_index_t layer)
{
  g_assert (map != NULL);
  g_assert (layer <= map->max_layer_index);


  return map->layer_tags[layer];
}


/* Set the tag value of a layer. */

void
set_layer_tag (map_t *map, layer_index_t layer, layer_tag_t tag)
{
  g_assert (map != NULL);
  g_assert (layer <= map->max_layer_index);

  map->layer_tags[layer] = tag;
}


/* Set the property bitfield of a zone. */

void
set_zone_properties (map_t *map, zone_index_t zone, zone_prop_t properties)
{
  g_assert (map != NULL);
  g_assert (zone <= map->max_zone_index);

  map->zone_properties[zone] = properties;
}

/* Set the value of a tile. */

void
set_tile_value (map_t *map, layer_index_t layer,
                dimension_t x, dimension_t y, layer_value_t value)
{
  g_assert (map != NULL);
  g_assert (layer <= map->max_layer_index);
  g_assert (x < map->width && y < map->height);

  map->value_planes[layer][(y * map->width) + x] = value;
}


/* Set the zone of a tile. */

void
set_tile_zone (map_t *map, layer_index_t layer,
               dimension_t x, dimension_t y, layer_zone_t zone)
{
  g_assert (map != NULL);
  g_assert (layer <= map->max_layer_index);
  g_assert (zone <= map->max_zone_index);
  g_assert (x < map->width && y < map->height);

  map->zone_planes[layer][(y * map->width) + x] = zone;
}


/* Get the width of a map, in tiles. */

dimension_t
get_map_width (map_t *map)
{
  g_assert (map != NULL);

  return map->width;
}


/* Get the height of a map, in tiles. */

dimension_t
get_map_height (map_t *map)
{
  g_assert (map != NULL);

  return map->height;
}


/* Get the highest tag number allocated on a map. */


layer_tag_t
get_max_tag (map_t *map)
{
  layer_index_t l;
  layer_tag_t result;


  g_assert (map != NULL);

  result = NULL_TAG;

  for (l = 0; l <= map->max_layer_index; l++)
    {
      if (get_layer_tag (map, l) > result)
        result = get_layer_tag (map, l);
    }

  return result;
}


/* Get the highest layer index allocated on a map. */

layer_index_t
get_max_layer (map_t *map)
{
  g_assert (map != NULL);

  return map->max_layer_index;
}


/* Get the highest zone index allocated on a map. */

zone_index_t
get_max_zone (map_t *map)
{
  g_assert (map != NULL);

  return map->max_zone_index;
}


/* De-initialise a map. */

void
free_map (map_t *map)
{
  unsigned int i;
  
  if (map)
    {
      if (map->layer_tags)
        free (map->layer_tags);

      if (map->zone_properties)
        free (map->zone_properties);

      /* Make sure to clean up all the data planes. */

      if (map->value_planes)
        {
          for (i = 0; i <= map->max_layer_index; i++)
            {
              if (map->value_planes[i])
                free (map->value_planes[i]);
            }

          free (map->value_planes);
        }

      if (map->zone_planes)
        {
          for (i = 0; i <= map->max_layer_index; i++)
            {
              if (map->zone_planes[i])
                free (map->zone_planes[i]);
            }

          free (map->zone_planes);
        }

      free (map);
    }
}
