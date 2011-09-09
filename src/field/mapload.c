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

/** 
 * @file    src/field/mapload.c
 * @author  Matt Windsor
 * @brief   Map loader (ported from CrystalsMapEditor).
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


/* -- STATIC DECLARATIONS -- */

/**
 * Parses the given file as a map file and attempts to return a map
 * created from its contents.
 *
 * @param file  The file to read from.
 * @param map   The map to populate with the read data.
 *
 * @return      the new map, or NULL if an error occurred.
 */
static bool_t
parse_map_file (FILE *file, map_t *map); 


/**
 * Reads the map header from the file and allocate a new map from it.
 *
 * @param file  The file to read from.
 *
 * @return      TRUE for success, FALSE otherwise.
 */
static bool_t
read_map_header (FILE *file, map_t *map);


/**
 * Reads the map header contents from the file.
 *
 * @param file  The file pointer to read from.
 * @param map   The map to populate with the read data.
 */
static bool_t
read_map_header_contents (FILE *file, map_t *map);


/**
 * Reads the map layer tags chunk from a file.
 *
 * @param file  The file pointer to read from.
 * @param map   The map to populate with the read data.
 *
 * @return      TRUE for success, FALSE otherwise.
 */
static bool_t
read_layer_tags_chunk (FILE *file, map_t *map);


/**
 * Reads the map layer tags from a file.
 *
 * @param file  The file pointer to read from.
 * @param map   The map to populate with the read data.
 *
 * @return      TRUE for success, FALSE otherwise.
 */
static void
read_layer_tags (FILE *file, map_t *map);


/**
 * Reads the map value planes chunk from a file.
 *
 * @param file  The file pointer to read from.
 * @param map   The map to populate with the read data.
 *
 * @return      TRUE for success, FALSE otherwise.
 */
static bool_t
read_map_value_planes_chunk (FILE *file, map_t *map);


/**
 * Reads the map value planes from a file.
 *
 * @param file  The file pointer to read from.
 * @param map   The map to populate with the read data.
 *
 * @return      TRUE for success, FALSE otherwise.
 */
static bool_t
read_map_value_planes (FILE *file, map_t *map);


/**
 * Reads a map value plane from a file.
 *
 * Each plane corresponds to one layer of the map.
 *
 * @param file   The file pointer to read from.
 * @param map    The map to populate with the read data.
 * @param layer  The index of the layer being read.
 *
 * @return  TRUE for success, FALSE otherwise.
 */
static bool_t
read_map_value_plane (FILE *file, map_t *map, layer_index_t layer);


/**
 * Reads the map zone planes chunk from a file.
 *
 * @param file  The file pointer to read from.
 * @param map   The map to populate with the read data.
 *
 * @return  TRUE for success, FALSE otherwise.
 */
static bool_t
read_map_zone_planes_chunk (FILE *file, map_t *map);


/**
 * Reads the map zone planes from a file.
 *
 * @param file  The file pointer to read from.
 * @param map   The map to populate with the read data.
 *
 * @return  TRUE for success, FALSE otherwise.
 */
static bool_t
read_map_zone_planes (FILE *file, map_t *map);


/**
 * Read layer zone data from a file.
 *
 * @param file   The file pointer to read from.
 * @param map    The map to populate with the read data.
 * @param layer  The index of the layer being read.
 *
 * @return  TRUE for success, FALSE otherwise.
 */
static bool_t
read_layer_zone_plane (FILE *file, map_t *map, layer_index_t layer);


/**
 * Read the zone properties chunk from a file.
 *
 * @param file  The file pointer to read from.
 * @param map   The map to populate with the read data.
 *
 * @return  TRUE for success, FALSE otherwise.
 */
static bool_t
read_map_zone_properties_chunk (FILE *file, map_t *map);


/**
 * Read the properties of each zone in the map from a file.
 *
 * @param file  The file pointer to read from.
 * @param map   The map to populate with the read data.
 *
 * @return  TRUE for success, FALSE otherwise.
 */
static bool_t
read_map_zone_properties (FILE *file, map_t *map);


/**
 * Check that a magic sequence is present.
 *
 * @param file      The file pointer to read from.
 * @param sequence  The byte sequence (one of the constant sequences from Map).
 *
 * @return          TRUE if it is present, FALSE otherwise.
 */
static bool_t
check_magic_sequence (FILE *file, const char sequence[]);


/**
 * Read an unsigned 16-bit integer (0-65535) from two bytes in big-endian format.
 *
 * @param  file  The file to read from.
 *
 * @return       the unsigned 16-bit integer.
 */
static uint16_t
read_uint16 (FILE *file);


/* -- DEFINITIONS -- */

/* Reads a map from a file using the Crystals map format. */
map_t *
load_map (const char path[])
{
  map_t *map = malloc (sizeof (map_t));
  FILE *file = fopen (path, "rb");

  if (map == NULL)
    {
      error ("MAPLOAD - load_map - Couldn't allocate map.");
      return NULL;
    }
  else if (file == NULL)
    {
      error ("MAPLOAD - load_map - Couldn't allocate file.");
      return NULL;
    }

  if (parse_map_file (file, map) == FAILURE)
    {
      error ("MAPLOAD - load_map - Map load failed.");
      free_map (map);
      map = NULL;
    }
  
  fclose (file);

  return map;
}


/* Parses the given file as a map file and attempts to return a map
 * created from its contents.
 */
static bool_t
parse_map_file (FILE *file, map_t *map)
{
  if (read_map_header (file, map) == FAILURE)
    {
      error ("MAPLOAD - load_map - Header read failed.");
      return FAILURE;
    }
  
  if (read_layer_tags_chunk (file, map) == FAILURE)
    {
      error ("MAPLOAD - load_map - Layer tags read failed.");
      free_map (map);
      return FAILURE;
    }
  
  if (read_map_value_planes_chunk (file, map) == FAILURE)
    {
      error ("MAPLOAD - load_map - Value planes read failed.");
      free_map (map);
      return FAILURE;
    }
  
  if (read_map_zone_planes_chunk (file, map) == FAILURE)
    {
      error ("MAPLOAD - load_map - Zone planes read failed.");
      free_map (map);
      return FAILURE;
    }
  
  if (read_map_zone_properties_chunk (file, map) == FAILURE)
    {
      error ("MAPLOAD - load_map - Zone properties read failed.");
      free_map (map);
      return FAILURE;
    }

  return SUCCESS;
}


/* Reads the map header from the file. */
static bool_t
read_map_header (FILE *file, map_t *map)
{
  if (check_magic_sequence (file, MAGIC_HEADER) == FAILURE)
    {
      error ("MAPLOAD - read_header - Not a map file.");
      return FAILURE;
    }
  else if (read_uint16 (file) != MAP_VERSION)
    {
      error ("MAPLOAD - read_header - Incorrect map format version.");
      return FAILURE;
    }

  return read_map_header_contents (file, map);
}


/* Reads the map header contents from the file. */
static bool_t
read_map_header_contents (FILE *file, map_t *map)
{
  dimension_t map_width = read_uint16 (file); /* In tiles */
  dimension_t map_height = read_uint16 (file); /* In tiles */
  layer_index_t max_layer_index = read_uint16 (file);
  zone_index_t max_zone_index = read_uint16 (file);
  
  /* Ignore unused bytes. */
  fgetc (file);
  fgetc (file);

  return init_map (map,
                   map_width,
                   map_height, 
                   max_layer_index, 
                   max_zone_index);
}


/* Read the map layer tags chunk from a file.  */
bool_t
read_layer_tags_chunk (FILE *file, map_t *map)
{
  if (check_magic_sequence (file, MAGIC_TAGS) == FAILURE)
    {
      return FAILURE;
    }

  read_layer_tags (file, map);
  return SUCCESS;
}


static void
read_layer_tags (FILE *file, map_t *map)
{
  layer_index_t l;
  for (l = 0; l <= get_max_layer (map); l++)
    {
      set_layer_tag (map, l, read_uint16 (file));
    }
}


/* Reads the map value planes chunk from a file. */
static bool_t
read_map_value_planes_chunk (FILE *file, map_t *map)
{
  if (check_magic_sequence (file, MAGIC_VALUES) == FAILURE)
    {
      return FAILURE;
    }

  return read_map_value_planes (file, map);
}

/* Reads the map value planes from a file. */
static bool_t
read_map_value_planes (FILE *file, map_t *map)
{
  layer_index_t l;
  bool_t result = SUCCESS;

  for (l = 0; l <= get_max_layer (map) && result == SUCCESS; l++)
    {
      result = read_map_value_plane (file, map, l);
    }

  return result;
}


/* Reads a map value plane from a file. */
static bool_t
read_map_value_plane (FILE *file, map_t *map, layer_index_t layer)
{
  dimension_t x;
  dimension_t y;
  bool_t result = SUCCESS;
  for (x = 0; x < get_map_width (map); x++)
    {
      for (y = 0; y < get_map_height (map) && result == SUCCESS; y++)
        {
          result = set_tile_value (map, layer, x, y, read_uint16 (file));
        }
    }

  return result;
}


/* Reads the map zone plane chunk from a file. */
static bool_t
read_map_zone_planes_chunk (FILE *file, map_t *map)
{
  if (check_magic_sequence (file, MAGIC_ZONES) == FAILURE)
    {
      return FAILURE;
    }

  return read_map_zone_planes (file, map);
}


/* Reads the map zone data from a file. */
static bool_t
read_map_zone_planes (FILE *file, map_t *map)
{
  layer_index_t l;
  bool_t result = SUCCESS;

  for (l = 0; l <= get_max_layer (map) && result == SUCCESS; l++)
    {
      result = read_layer_zone_plane (file, map, l);
    }

  return result;
}


/* Reads layer zone data from a file. */
static bool_t
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


/* Reads the zone properties chunk from a file. */
static bool_t
read_map_zone_properties_chunk (FILE *file, map_t *map)
{
  if (check_magic_sequence (file, MAGIC_PROPERTIES) == FAILURE)
    {
      return FAILURE;
    }

  return read_map_zone_properties (file, map);
}


/* Reads the zone properties from a file. */
static bool_t
read_map_zone_properties (FILE *file, map_t *map)
{
  zone_index_t i;
  bool_t result = SUCCESS;
  for (i = 0; i <= get_max_zone (map) && result == SUCCESS; i++)
    {
      result = set_zone_properties (map, i, read_uint16 (file));
    }
  
  return SUCCESS;
}


/* Checks that a magic sequence is present. */
static bool_t
check_magic_sequence (FILE *file, const char sequence[])
{
  char *check = calloc (strlen (sequence) + 1, sizeof (char));
  if (check == NULL)
    {
      error ("MAPLOAD - check_magic_sequence - Could not allocate check string.");
      return FAILURE;
    }

  fread (check, sizeof (char), strlen (sequence), file);

  if (strcmp (sequence, check) != 0)
    {
      free (check);
      error ("MAPLOAD - check_magic_sequence - Magic sequence not present.");
      return FAILURE;
    }

  free (check);
  return SUCCESS;
}


/* Reads an unsigned 16-bit integer (0-65535) from two bytes in 
 * big-endian format.
 */
static uint16_t
read_uint16 (FILE *file)
{
  uint16_t ushort = 0;

  ushort |= (uint16_t) (fgetc (file) << 8); /* Most significant byte  */
  ushort |= (uint16_t) (fgetc (file));      /* Least significant byte */

  return ushort;
}
