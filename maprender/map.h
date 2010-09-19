#ifndef __MAP_H__
#define __MAP_H__

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
cleanup_map (struct Map *map);

#endif /* __MAP_H__ */
