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

/** @file    map.h
 *  @author  Matt Windsor
 *  @brief   Prototypes and declarations for low-level map handling
 *           functions.
 */

#ifndef _MAP_H
#define _MAP_H

/* -- CONSTANTS -- */

enum
  {
    TILE_W = 32, /**< Width of one tile. 
                    @todo FIXME: read this from tileset data. */
    TILE_H = 32  /**< Height of one tile. 
                    @todo FIXME: read this from tileset data. */
  };

/* -- TYPEDEFS -- */

typedef unsigned short layer_t; /**< Type for layer data. */

/* -- STRUCTURES -- */

/** The map data structure.
 *
 *  This contains the tile data and eventually the object list for a
 *  map.
 */
struct map
{
  unsigned int width;       /**< Width of the map, in tiles. */
  unsigned int height;      /**< Height of the map, in tiles. */
  unsigned char num_layers; /**< Number of arrays to store in the map. */
  layer_t **data_layers;    /**< Pointers to map layer arrays. */
};

/* -- GLOBAL VARIABLES -- */

extern struct map *g_map; /**< The map currently in use. (FIXME: is
                             global necessary?) */

/* -- PROTOTYPES -- */

/** Initialise the test map.
 *
 *  @todo  drop this.
 *
 *  @return  a pointer to the test map, or NULL for allocation
 *  failure.
 */

struct map *
init_test_map (void);

/** Initialise a map.
 *
 *  This allocates enough space for a map with the specified width,
 *  height and number of layers, returning a blank map that can then
 *  be populated with tile information
 *
 *  @param width       The width of the map.
 *  @param height      The height of the map.
 *  @param num_layers  The number of layers to reserve.
 *
 *  @return  a pointer to the map structure, or NULL for allocation
 *  failure.
 */

struct map *
init_map (unsigned int width, 
          unsigned int height, 
          unsigned char num_layers);

/** Get the tag number assigned to a particular layer.
 *
 *  @param map    Pointer to the map to query.
 *
 *  @param layer  ID of the layer to retrieve the tag number for.
 *
 *  @return  the highest tag number allocated on the map.
 *           If the map pointer given is NULL, or the layer does not 
 *           exist in the given map, 0 is returned and an error is raised.
 */

layer_t
get_tag (struct map *map, unsigned int layer);

/** Get the highest tag number allocated on a map.
 *
 *  @param map  Pointer to the map to query.
 *
 *  @return  the highest tag number allocated on the map.
 *           If the map pointer given is NULL, 0 is returned and an 
 *           error is raised.
 */

layer_t
get_max_tag (struct map *map);

/** De-initialise a map.
 *
 *  This de-allocates all memory consumed by the given map structure.
 *
 *  @param map  A pointer to the map to deallocate.
 */

void
cleanup_map (struct map *map);

#endif /* _MAP_H */
