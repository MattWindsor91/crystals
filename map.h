#ifndef _MAP_H
#define _MAP_H

enum
  {
    TILE_W = 32,
    TILE_H = 32
  };

typedef unsigned short layer_t; /**< Type for layer data. */

struct map
{
  unsigned int width;       /**< Width of the map, in tiles. */
  unsigned int height;      /**< Height of the map, in tiles. */
  unsigned char num_layers; /**< Number of arrays to store in the map. */
  layer_t **data_layers;    /**< Pointers to map layer arrays. */
};

extern struct map *g_map;

struct map *
init_test_map (void);

struct map *
init_map (unsigned int width, 
          unsigned int height, 
          unsigned char num_layers);

void
cleanup_map (struct map *map);

#endif /* _MAP_H */
