#ifndef _MAP_H
#define _MAP_H

/* -- CONSTANTS -- */

enum
  {
    TILE_W = 32, /**< Width of one tile. (FIXME: read this from
                    tileset data.)
    TILE_H = 32  /**< Height of one tile. (FIXME: read this from
                    tileset data.)
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
 *  @return  a pointer to the test map.
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
 *  @return  a pointer to the map structure.
 */

struct map *
init_map (unsigned int width, 
          unsigned int height, 
          unsigned char num_layers);

/** De-initialise a map.
 *
 *  This de-allocates all memory consumed by the given map structure.
 *
 *  @param map  A pointer to the map to deallocate.
 */

void
cleanup_map (struct map *map);

#endif /* _MAP_H */
