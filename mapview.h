#ifndef _MAPVIEW_H
#define _MAPVIEW_H

#include "map.h"

struct map_view
{
  int x_offset;
  int y_offset;
  struct map *map;
  unsigned char *dirty_tiles;
};

struct map_view *
init_mapview (struct map *map);

extern const char FN_TILESET[];

void
render_map (struct map_view *mapview);

void
render_map_layer (struct map_view *mapview, unsigned int layer);

void
scroll_map (struct map_view *mapview, int direction);

void
mark_dirty_rect (struct map_view *mapview,
                 int start_x, int start_y, int width, int height);

void
cleanup_mapview (struct map_view *mapview);

#endif /* _MAPVIEW_H */
