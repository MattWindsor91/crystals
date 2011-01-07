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

/** @file    src/field/map.h
 *  @author  Matt Windsor
 *  @brief   Prototypes and declarations for low-level map handling
 *           functions.
 */


#ifndef _MAP_H
#define _MAP_H


/* -- TYPEDEFS -- */

typedef unsigned short dimension_t;    /**< Type for tile-based map dimensions. */

typedef unsigned short layer_tag_t;    /**< Type for layer tags. */

typedef signed int     layer_count_t;  /**< Type large enough to hold a layer count. */
typedef unsigned short layer_index_t;  /**< Type for layer indices. */
typedef unsigned short layer_value_t;  /**< Type for layer value data. */
typedef unsigned short layer_zone_t;   /**< Type for layer zone data. */

typedef signed int     zone_count_t;   /**< Type large enough to hold a zone count. */
typedef unsigned short zone_index_t;   /**< Type for zone indices. */
typedef unsigned short zone_prop_t;    /**< Type for zone properties bitfields. */

/* -- CONSTANTS -- */

enum 
  {
    NULL_TAG = 0 /**< The tag ID reserved as a null value. */
  };


/* -- STRUCTURES -- */

/** The map data structure.
 *
 *  This contains the tile data and eventually the object list for a
 *  map.
 *
 *  @note width and height are currently stored as ints due to quirks
 *        in the map viewing code. This may change later.
 */

typedef struct map
{
  dimension_t width;                /**< Width of the map, in tiles. */
  dimension_t height;               /**< Height of the map, in tiles. */

  zone_index_t    max_zone_index;   /**< Highest zone index in the map. */
  zone_prop_t    *zone_properties;  /**< Array of zone property bitfields. */

  layer_index_t   max_layer_index;  /**< Highest layer index in the map. */
  layer_tag_t    *layer_tags;       /**< Array of map layer tags. */
  layer_value_t **value_planes;     /**< Pointers to map layer value planes. */
  layer_zone_t  **zone_planes;      /**< Pointers to map layer value planes. */

} map_t;


/* -- GLOBAL VARIABLES -- */

extern struct map *g_map; /**< The map currently in use. (FIXME: is
                             global necessary?) */


/* -- PROTOTYPES -- */

/** 
 * Initialise the test map.
 *
 * @todo  drop this.
 *
 * @return  a pointer to the test map, or NULL for allocation
 *          failure.
 */

map_t *
init_test_map (void);


/**
 * Initialise a map.
 *
 * This allocates enough space for a map with the specified width,
 * height and number of layers, returning a blank map that can then
 * be populated with tile information
 *
 * @param width            The width of the map, in tiles.
 * @param height           The height of the map, in tiles.
 * @param max_layer_index  The maximum layer index in the map
 *                         (number of layers to reserve, minus one).
 * @param max_zone_index   The maximum zone index in the map
 *                         (number of zones to reserve, minus one).
 *
 * @return  a pointer to the map structure, or NULL if an error occurred
 *          during initialisation.
 */

map_t *
init_map (dimension_t width,
          dimension_t height,
          layer_index_t max_layer_index,
          zone_index_t max_zone_index);


/**
 * Set the tag value of a layer.
 *
 * @param map    Pointer to the map to modify.
 * @param layer  Index of the layer on the map to modify.
 * @param tag    The new layer tag.
 *
 * @return       SUCCESS if the operation succeeded, FAILURE otherwise.
 */

int
set_layer_tag (map_t *map, layer_index_t layer, layer_tag_t tag);


/**
 * Set the properties bitfield of a zone..
 *
 * @param map         Pointer to the map to modify.
 * @param zone        Index of the zone on the map to modify.
 * @param properties  The new properties bitfield.
 *
 * @return            SUCCESS if the operation succeeded, FAILURE otherwise.
 */

int
set_zone_properties (map_t *map, zone_index_t zone, zone_prop_t properties);


/**
 * Set the value of a tile.
 *
 * @param map    Pointer to the map to modify.
 * @param layer  Index of the layer on the map to modify.
 * @param x      X co-ordinate, in tiles, of the tile to modify.
 * @param y      Y co-ordinate, in tiles, of the tile to modify.
 * @param value  The new value of the tile.
 *
 * @return       SUCCESS if the operation succeeded, FAILURE otherwise.
 */

int
set_tile_value (map_t *map, layer_index_t layer, dimension_t x, dimension_t y, layer_value_t value);


/**
 * Set the zone of a tile.
 *
 * @param map    Pointer to the map to modify.
 * @param layer  Index of the layer on the map to modify.
 * @param x      X co-ordinate, in tiles, of the tile to modify.
 * @param y      Y co-ordinate, in tiles, of the tile to modify.
 * @param zone   The new zone of the tile.
 *
 * @return  SUCCESS if the operation succeeded, FAILURE otherwise.
 */

int
set_tile_zone (map_t *map, layer_index_t layer, dimension_t x, dimension_t y, layer_zone_t value);


/**
 * Get the width of a map, in tiles.
 *
 * @param map  Pointer to the map to query.
 *
 * @return     The width of the map, in tiles.
 *             If the map is NULL, an error will be raised and 0
 *             will be returned.
 */

dimension_t
get_map_width (map_t *map);


/**
 * Get the height of a map, in tiles.
 *
 * @param map  Pointer to the map to query.
 *
 * @return     The height of the map, in tiles.
 *             If the map is NULL, an error will be raised and 0
 *             will be returned.
 */

dimension_t
get_map_height (map_t *map);


/** 
 * Get the tag number assigned to a particular layer.
 *
 * @param map    Pointer to the map to query.
 * @param layer  ID of the layer to retrieve the tag number for.
 *
 * @return  the highest tag number allocated on the map.
 *          If the map pointer given is NULL, or the layer does not 
 *          exist in the given map, 0 is returned and an error is raised.
 */

layer_tag_t
get_layer_tag (map_t *map, layer_index_t layer);


/** 
 * Get the highest tag number allocated on a map.
 *
 * @param map  Pointer to the map to query.
 *
 * @return     the highest tag number allocated on the map.
 *             If the map pointer given is NULL, 0 is returned and an
 *             error is raised.
 */

layer_tag_t
get_max_tag (map_t *map);


/**
 * Get the highest layer index allocated on a map.
 *
 * @param map  Pointer to the map to query.
 *
 * @return     the highest layer index allocated on the map.
 *             If the map pointer given is NULL, 0 is returned and an
 *             error is raised.
 */

layer_index_t
get_max_layer (map_t *map);


/**
 * Get the highest zone index allocated on a map.
 *
 * @param map  Pointer to the map to query.
 *
 * @return     the highest zone index allocated on the map.
 *             If the map pointer given is NULL, 0 is returned and an
 *             error is raised.
 */

zone_index_t
get_max_zone (map_t *map);


/**
 * De-initialise a map.
 *
 * This de-allocates all memory consumed by the given map structure.
 *
 * @param map  A pointer to the map to deallocate.
 */

void
free_map (map_t *map);


#endif /* _MAP_H */
