#ifndef _MAP_H
#define _MAP_H

enum
  {
    TILE_W = 32,
    TILE_H = 32
  };

struct Map
{
  int width;
  int height;
  unsigned int num_layers;
  unsigned char **data_layers;
};

extern struct Map *g_map;

struct Map *
init_test_map (void);

struct Map *
init_map (unsigned int width, 
          unsigned int height, 
          unsigned int num_layers);

void
cleanup_map (struct Map *map);

#endif /* _MAP_H */
