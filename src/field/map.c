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
bool_t
init_map (map_t *map,
          dimension_t width,
          dimension_t height,
          layer_index_t max_layer_index,
          zone_index_t max_zone_index)
{
  if (width == 0 || height == 0)
    {
      error ("MAP - init_map - Given a zero width or height", width, height);
      return FAILURE;
    }

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

  map->layer_tags = calloc ((size_t) max_layer_index + 1, sizeof (layer_tag_t));
  if (map->layer_tags == NULL)
    {
      error ("MAP - init_map - Map tag array allocation failed.");
      return FAILURE;
    }


  /* Allocate properties array. */

  map->zone_properties = calloc ((size_t) max_zone_index + 1, sizeof (zone_prop_t));
  if (map->zone_properties == NULL)
    {
      error ("MAP - init_map - Map zone property array allocation failed.");
      return FAILURE;
    }


  allocate_plane_arrays (map);
  allocate_planes (map);

  return SUCCESS;
}


/* -- STATIC DEFINITIONS -- */

/* Allocate arrays to contain layer planes in the map. */
static void
allocate_plane_arrays (map_t *map)
{
  map->value_planes = calloc ((size_t) map->max_layer_index + 1,
                              sizeof (layer_value_t *));

  if (map->value_planes == NULL)
    {
      fatal ("MAP - init_map - Map value plane array allocation failed.");
    }


  map->zone_planes = calloc ((size_t) map->max_layer_index + 1,
                             sizeof (layer_zone_t *));
  if (map->zone_planes == NULL)
    {
      fatal ("MAP - init_map - Map zone plane array allocation failed.");
    }
}


/* Allocate planes for each layer in the map. */
static void
allocate_planes (map_t *map)
{
  int i;
  for (i = 0; i <= map->max_layer_index; i++)
    {
      map->value_planes[i] = calloc ((size_t) map->width * map->height,
                                     sizeof (layer_value_t));
      if (map->value_planes[i] == NULL)
        {
          fatal ("MAP - allocate_planes - Map value plane allocation failed for layer %d.", i);
        }

      map->zone_planes[i] = calloc ((size_t) map->width * map->height,
                                    sizeof (layer_zone_t));
      if (map->zone_planes[i] == NULL)
        {
          fatal ("MAP - allocate_planes - Map zone plane allocation failed for layer %d.", i);
        }
    }
}


/* Get the tag associated with a layer. */

layer_tag_t
get_layer_tag (map_t *map, layer_index_t layer)
{
  /* Sanity checking. */

  if (map == NULL)
    {
      error ("MAP - get_layer_tag - Tried to get tag for a NULL map.");
      return NULL_TAG;
    }

  if (layer > map->max_layer_index)
    {
      error ("MAP - get_layer_tag - Tried to get tag for an invalid layer.");
      return NULL_TAG;
    }

  /* End sanity checking. */


  return map->layer_tags[layer];
}


/* Set the tag value of a layer. */

bool_t
set_layer_tag (map_t *map, layer_index_t layer, layer_tag_t tag)
{
  /* Sanity checking. */

  if (map == NULL)
    {
      error ("MAP - set_tile_value - Given map pointer is NULL.");
      return FAILURE;
    }

  if (layer > map->max_layer_index)
    {
      error ("MAP - set_tile_value - Given layer does not exist.");
      return FAILURE;
    }

  /* End sanity checking */


  map->layer_tags[layer] = tag;

  return SUCCESS;
}


/* Set the property bitfield of a zone. */

bool_t
set_zone_properties (map_t *map, zone_index_t zone, zone_prop_t properties)
{
  /* Sanity checking. */

  if (map == NULL)
    {
      error ("MAP - set_tile_value - Given map pointer is NULL.");
      return FAILURE;
    }

  if (zone > map->max_zone_index)
    {
      error ("MAP - set_tile_value - Given zone does not exist.");
      return FAILURE;
    }

  /* End sanity checking. */


  map->zone_properties[zone] = properties;

  return SUCCESS;
}

/* Set the value of a tile. */

bool_t
set_tile_value (map_t *map, layer_index_t layer,
                dimension_t x, dimension_t y, layer_value_t value)
{
  /* Sanity checking. */

  if (map == NULL)
    {
      error ("MAP - set_tile_value - Given map pointer is NULL.");
      return FAILURE;
    }

  if (layer > map->max_layer_index)
    {
      error ("MAP - set_tile_value - Given layer does not exist.");
      return FAILURE;
    }

  if (x >= map->width || y >= map->height)
    {
      error ("MAP - set_tile_value - Tile out of bounds.");
      return FAILURE;
    }

  /* End sanity checking. */


  map->value_planes[layer][(y * map->width) + x] = value;

  return SUCCESS;
}


/* Set the zone of a tile. */

bool_t
set_tile_zone (map_t *map, layer_index_t layer,
               dimension_t x, dimension_t y, layer_zone_t zone)
{
  /* Sanity checking. */

  if (map == NULL)
    {
      error ("MAP - set_tile_zone - Given map pointer is NULL.");
      return FAILURE;
    }

  if (layer > map->max_layer_index)
    {
      error ("MAP - set_tile_zone - Given layer does not exist.");
      return FAILURE;
    }

  if (zone > map->max_zone_index)
    {
      error ("MAP - set_tile_zone - Given zone does not exist.");
      return FAILURE;
    }

  if (x >= map->width || y >= map->height)
    {
      error ("MAP - set_tile_zone - Tile out of bounds.");
      return FAILURE;
    }

  /* End sanity checking. */


  map->zone_planes[layer][(y * map->width) + x] = zone;

  return SUCCESS;
}


/* Get the width of a map, in tiles. */

dimension_t
get_map_width (map_t *map)
{
  /* Sanity checking. */

  if (map == NULL)
    {
      error ("MAP - get_map_width - Tried to get width of a NULL map.");
      return 0;
    }

  /* End sanity checking. */


  return map->width;
}


/* Get the height of a map, in tiles. */

dimension_t
get_map_height (map_t *map)
{
  /* Sanity checking. */

  if (map == NULL)
    {
      error ("MAP - get_map_height - Tried to get height of a NULL map.");
      return 0;
    }

  /* End sanity checking. */


  return map->height;
}


/* Get the highest tag number allocated on a map. */


layer_tag_t
get_max_tag (map_t *map)
{
  layer_index_t l;
  layer_tag_t result;


  /* Sanity checking. */

  if (map == NULL)
    {
      error ("MAP - get_max_tag - Map is NULL.");
      return NULL_TAG;
    }

  /* End sanity checking. */


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
  /* Sanity checking. */

  if (map == NULL)
    {
      error ("MAP - get_max_layer - Map is NULL.");
      return 0;
    }

  /* End sanity checking. */


  return map->max_layer_index;
}


/* Get the highest zone index allocated on a map. */

zone_index_t
get_max_zone (map_t *map)
{
  /* Sanity checking. */

  if (map == NULL)
    {
      error ("MAP - get_max_zone - Map is NULL.");
      return 0;
    }

  /* End sanity checking. */


  return map->max_zone_index;
}


/* De-initialise a map. */

void
free_map (map_t *map)
{
  if (map)
    {
      if (map->layer_tags)
        free (map->layer_tags);

      if (map->zone_properties)
        free (map->zone_properties);

      /* Make sure to clean up all the data planes. */

      if (map->value_planes)
        {
          unsigned int i;

          for (i = 0; i <= map->max_layer_index; i++)
            {
              if (map->value_planes[i])
                free (map->value_planes[i]);
            }

          free (map->value_planes);
        }

      if (map->zone_planes)
        {
          unsigned int i;

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
