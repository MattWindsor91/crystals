#ifndef _MAPVIEW_H
#define _MAPVIEW_H

#include "map.h"

struct MapView
{
  int x_offset;
  int y_offset;
  struct Map *map;
  unsigned char *dirty_tiles;
};

struct MapView *
init_mapview (struct Map *map);

extern const char FN_TILESET[];

void
render_map (struct MapView *mapview);

void
render_map_layer (struct MapView *mapview, unsigned int layer);

void
scroll_map (struct MapView *mapview, int direction);

void
mark_dirty_rect (struct MapView *mapview,
                 int start_x, int start_y, int width, int height);

void
cleanup_mapview (struct MapView *mapview);

#endif /* _MAPVIEW_H */
