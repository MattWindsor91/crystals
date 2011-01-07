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

/** @file    src/field/mapload.h
 *  @author  Matt Windsor
 *  @brief   Declarations and constants for map loader (ported from CrystalsMapEditor).
 */

#ifndef _MAPLOAD_H
#define _MAPLOAD_H

#include <stdlib.h>
#include <stdio.h>

#include "map.h"


/* -- CONSTANTS -- */

extern const char *MAGIC_HEADER;      /**< Sequence denoting the map header and also the map filetype. */
extern const char *MAGIC_TAGS;        /**< Sequence denoting the layer tag block.                      */
extern const char *MAGIC_VALUES;      /**< Sequence denoting the value plane block.                    */
extern const char *MAGIC_ZONES;       /**< Sequence denoting the zone plane block.                     */
extern const char *MAGIC_PROPERTIES;  /**< Sequence denoting the zone properties block.                */

enum
  {
    MAP_VERSION = 1                   /**< The file format version expected. */
  };

/* -- DEFINITIONS -- */

/**
 * Read a map from a file using the Crystals map format.
 *
 * The Crystals map format is detailed in the design document,
 * "The Crystals Map Format", available with the Crystals source.
 *
 * @param path  The path to the file to open.
 *
 * @return      the new map_t, or NULL if an error occurred.
 */

map_t *
load_map (const char path[]);


/**
 * Read the map header from the file and allocate a new map from it.
 *
 * @param file  The FileOutputStream to read to.
 *
 * @return      the new map, or NULL if an error occurred.
 */

map_t *
read_map_header (FILE *file);


/**
 * Read the map layer tags from a file.
 *
 * @param file  The file pointer to read from.
 * @param map   The map to populate with the read data.
 *
 * @return      TRUE for success, FALSE otherwise.
 */

int
read_layer_tags (FILE *file, map_t *map);


/**
 * Read the map value data from a file.
 *
 * @param file  The file pointer to read from.
 * @param map   The map to populate with the read data.
 *
 * @return      TRUE for success, FALSE otherwise.
 */

int
read_map_value_planes (FILE *file, map_t *map);


/**
 * Read layer value data from a file.
 *
 * @param file  The file pointer to read from.
 * @param map    The map to populate with the read data.
 * @param layer  The index of the layer being read.
 *
 * @return  TRUE for success, FALSE otherwise.
 */

int
read_layer_value_plane (FILE *file, map_t *map, layer_index_t layer);


/**
 * Read the map zone data from a file.
 *
 * @param file  The file pointer to read from.
 * @param map     The map to populate with the read data.
 *
 * @return  TRUE for success, FALSE otherwise.
 */

int
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

int
read_layer_zone_plane (FILE *file, map_t *map, layer_index_t layer);


/**
 * Read the properties of each zone in the map from a file.
 *
 * @param file    The file pointer to read from.
 * @param map     The map to populate with the read data.
 *
 * @return  TRUE for success, FALSE otherwise.
 */

int
read_map_zone_properties (FILE *file, map_t *map);


/**
 * Check that a magic sequence is present.
 *
 * @param file      The file pointer to read from.
 * @param sequence  The byte sequence (one of the constant sequences from Map).
 *
 * @return          TRUE if it is present, FALSE otherwise.
 */

int
check_magic_sequence (FILE *file, const char sequence[]);


/**
 * Read an unsigned short (0-65535) from two bytes in big-endian format.
 *
 * @param  file  The file to read from.
 *
 * @return       the unsigned short.
 */

unsigned short
read_ushort (FILE *file);


#endif /* not _MAPLOAD_H */
