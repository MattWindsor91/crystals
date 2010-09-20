#ifndef _MAP_H
#define _MAP_H

enum
  {
    TILE_W = 32,
    TILE_H = 32
  };

struct MapView
{
  int x_offset;
  int y_offset;
  struct Map *map;
};

struct Map
{
  unsigned int width;
  unsigned int height;
  unsigned int num_layers;
  unsigned char **data_layers;
};

extern const char FN_TILESET[];

extern struct Map *g_map;

struct Map *
init_test_map (void);

struct Map *
init_map (unsigned int width, 
          unsigned int height, 
          unsigned int num_layers);

struct MapView *
init_mapview (struct Map *map);

void
render_map (struct MapView *mapview);

void
render_map_layer (struct Map *map, unsigned int layer,
                  int x_offset, int y_offset);

void
scroll_map (struct MapView *mapview, int direction);

void
cleanup_map (struct Map *map);

void
cleanup_mapview (struct MapView *mapview);

#endif /* _MAP_H */
