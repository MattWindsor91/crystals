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

/** @file    src/field/mapload.c
 *  @author  Matt Windsor
 *  @brief   Map loader (ported from Crystalsmap_tEditor).
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "mapload.h"
#include "map.h"
#include "../util.h"
#include "../types.h"


/* -- CONSTANTS -- */

const char *MAGIC_HEADER     = "CMFT";
const char *MAGIC_TAGS       = "TAGS";
const char *MAGIC_VALUES     = "VALS";
const char *MAGIC_ZONES      = "ZONE";
const char *MAGIC_PROPERTIES = "PROP";


/* -- DEFINITIONS -- */

/**
 * Read a map from a file using the Crystals map format.
 *
 * The Crystals map format is detailed in the design document,
 * "The Crystals Map Format", available with the Crystals source.
 *
 * @param path  The path to the file to open.
 *
 * @return  the new map, or NULL if an error occurred.
 */

map_t *
load_map (const char path[])
{
  FILE *file;
  map_t *map;


  map = NULL;
  file = fopen (path, "rb");

  if (file == NULL)
    {
      g_critical ("MAPLOAD - read_map - Couldn't open file.");
      return NULL;
    }

  map = read_map_header (file);

  if (map == NULL)
    {
      g_critical ("MAPLOAD - load_map - Header read failed.");
      return NULL;
    }

  if (read_layer_tags (file, map) == FAILURE)
    {
      g_critical ("MAPLOAD - load_map - Layer tag read failed.");
      free_map (map);
      return NULL;
    }

  if (read_map_value_planes (file, map) == FAILURE)
    {
      g_critical ("MAPLOAD - load_map - Value planes failed.");
      free_map (map);
      return NULL;
    }

  if (read_map_zone_planes (file, map) == FAILURE)
    {
      g_critical ("MAPLOAD - load_map - Zone planes read failed.");
      free_map (map);
      return NULL;
    }

  if (read_map_zone_properties (file, map) == FAILURE)
    {
      g_critical ("MAPLOAD - load_map - Zone properties read failed.");
      free_map (map);
      return NULL;
    }

  return map;
}


/**
 * Read the map header from the file and allocate a new map from it.
 *
 * @param file  The FileOutputStream to read to.
 *
 * @return  the new map, or NULL if an error occurred.
 *
 * @throws  IOException
 */

map_t *
read_map_header (FILE *file)
{
  dimension_t width;
  dimension_t height;
  layer_index_t max_layer_index;
  zone_index_t max_zone_index;


  if (file == NULL)
    return NULL;

  /* Check magic string. */

  if (check_magic_sequence (file, MAGIC_HEADER) == FAILURE)
    {
      g_critical ("MAPLOAD - read_header - Not a map file.");
      return NULL;
    }


  /* Check map format version. */

  if (read_uint16 (file) != MAP_VERSION)
    {
      g_critical ("MAPLOAD - read_header - Incorrect map format version.");
      return NULL;
    }


  /* Read in map dimensions (width, height). */

  width  = read_uint16 (file);
  height = read_uint16 (file);


  /* Read in highest layer index. */

  max_layer_index = read_uint16 (file);


  /* Read in highest zone index. */

  max_zone_index = read_uint16 (file);


  /* Ignore unused bytes. */

  fgetc (file);
  fgetc (file);


  return init_map (width, height, max_layer_index, max_zone_index);
}


/**
 * Read the map layer tags from a file.
 *
 * @param file  The file pointer to read from.
 * @param map   The map to populate with the read data.
 *
 * @return  SUCCESS for success, FAILURE otherwise.
 */

bool_t
read_layer_tags (FILE *file, map_t *map)
{
  layer_index_t l;

  if (check_magic_sequence (file, MAGIC_TAGS) == FAILURE)
    return FAILURE;

  for (l = 0; l <= get_max_layer (map); l++)
    set_layer_tag (map, l, read_uint16 (file));

  return SUCCESS;
}


/* Read the map value data from a file. */

bool_t
read_map_value_planes (FILE *file, map_t *map)
{
  layer_index_t l;
  bool_t result;

  if (check_magic_sequence (file, MAGIC_VALUES) == FAILURE)
    return FAILURE;


  result = SUCCESS;

  for (l = 0; l <= get_max_layer (map) && result == SUCCESS; l++)
    result = read_layer_value_plane (file, map, l);

  return result;
}


/**
 * Read layer value data from a file.
 *
 * @param file   The file pointer to read from.
 * @param map    The map to populate with the read data.
 * @param layer  The index of the layer being read.
 *
 * @return       SUCCESS for success, FAILURE otherwise.
 */

bool_t
read_layer_value_plane (FILE *file, map_t *map, layer_index_t layer)
{
  dimension_t x;
  dimension_t y;
  bool_t result;

  result = SUCCESS;

  for (x = 0; x < get_map_width (map); x++)
    {
      for (y = 0; y < get_map_height (map) && result == SUCCESS; y++)
        {
          result = set_tile_value (map, layer, x, y, read_uint16 (file));
        }
    }

  return result;
}


/**
 * Read the map zone data from a file.
 *
 * @param file  The file pointer to read from.
 * @param map     The map to populate with the read data.
 *
 * @return  SUCCESS for success, FAILURE otherwise.
 *
 * @throws  IOException
 */

bool_t
read_map_zone_planes (FILE *file, map_t *map)
{
  layer_index_t l;
  bool_t result;


  if (check_magic_sequence (file, MAGIC_ZONES) == FAILURE)
    return FAILURE;

  result = SUCCESS;

  for (l = 0; l <= get_max_layer (map) && result == SUCCESS; l++)
    result = read_layer_zone_plane (file, map, l);

  return result;
}


/**
 * Read layer zone data from a file.
 *
 * @param file   The file pointer to read from.
 * @param map    The map to populate with the read data.
 * @param layer  The index of the layer being read.
 *
 * @return  SUCCESS for success, FAILURE otherwise.
 *
 * @throws  IOException
 */

bool_t
read_layer_zone_plane (FILE *file, map_t *map, unsigned short layer)
{
  dimension_t x;
  dimension_t y;


  for (x = 0; x < get_map_width (map); x++)
    {
      for (y = 0; y < get_map_height (map); y++)
        {
          set_tile_zone (map, layer, x, y, read_uint16 (file));
        }
    }

  return SUCCESS;
}


/**
 * Read the properties of each zone in the map from a file.
 *
 * @param file    The file pointer to read from.
 * @param map     The map to populate with the read data.
 *
 * @return  SUCCESS for success, FAILURE otherwise.
 */

bool_t
read_map_zone_properties (FILE *file, map_t *map)
{
  zone_index_t i;
  bool_t result;

  if (check_magic_sequence (file, MAGIC_PROPERTIES) == FAILURE)
    return FAILURE;

  result = SUCCESS;

  for (i = 0; i <= get_max_zone (map) && result == SUCCESS; i++)
    result = set_zone_properties (map, i, read_uint16 (file));

  return SUCCESS;
}


/**
 * Check that a magic sequence is present.
 *
 * @param file      The file pointer to read from.
 * @param sequence  The byte sequence (one of the constant sequences in mapload).
 *
 * @return          SUCCESS if it is present, FAILURE otherwise.
 */

bool_t
check_magic_sequence (FILE *file, const char sequence[])
{
  char *check;
  size_t i;

  check = malloc (strlen (sequence) + 1);

  if (check == NULL)
    {
      g_critical ("MAPLOAD - check_magic_sequence - Could not allocate check string.");
      return FAILURE;
    }

  fread (check, sizeof (char), strlen (sequence), file);

  for (i = 0; i < strlen (sequence); i++)
    {
      if (check[i] != sequence[i])
        {
          free (check);
          g_critical ("MAPLOAD - check_magic_sequence - Magic sequence not present.");
          return FAILURE;
        }
    }

  free (check);
  return SUCCESS;
}


/**
 * Read an unsigned 16-bit integer (0-65535) from two bytes in big-endian format.
 *
 * @param   file  The file to read from.
 *
 * @return  the unsigned 16-bit integer.
 */

uint16_t
read_uint16 (FILE *file)
{
  uint16_t ushort = 0;

  ushort |= (uint16_t) (fgetc (file) << 8); /* Most significant byte  */
  ushort |= (uint16_t) (fgetc (file));      /* Least significant byte */

  return ushort;
}
