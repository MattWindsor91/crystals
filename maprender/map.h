#ifndef _MAP_H
#define _MAP_H

enum
  {
    TILE_W = 32,
    TILE_H = 32
  };

struct Map
{
  unsigned int width;
  unsigned int height;
  unsigned int num_ground_layers;
  unsigned int num_overlay_layers;
  unsigned char **data_layers;
};

extern const char FN_TILESET[];

extern struct Map *g_map;

struct Map *
init_test_map (void);

struct Map *
init_map (unsigned int width, 
          unsigned int height, 
          unsigned int num_ground_layers, 
          unsigned int num_overlay_layers);

void
render_map (struct Map *map);

void
render_map_layer (struct Map *map, unsigned int layer);

void
scroll_map (int direction);

void
cleanup_map (struct Map *map);

#endif /* _MAP_H */
