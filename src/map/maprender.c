/*
 * Crystals (working title)
 *
 * Copyright (c) 2010, 2011 Matt Windsor, Michael Walker and Alexander
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

/**
 * @file    src/map/maprender.c
 * @author  Matt Windsor
 * @brief   The map renderer.
 *
 * The map renderer takes a set of rectangular areas on a map marked
 * as requiring an update ("dirty") and renders the contents of those
 * visible on-screen to the display.
 *
 * @todo FIXME: Reduce coupling to mapview_t.
 */

#include "../crystals.h"


/* -- LOCAL STRUCTURES -- */

/**
 * Structure of data needed during a rectangle layer render pass.
 */
typedef struct render_map_layer_tile_rect_data
{
  mapview_t *mapview;
  image_t *tileset;
  layer_index_t layer;
} render_map_layer_tile_rect_data_t;

/* -- STATIC DECLARATIONS -- */


/**
 * Handles a dirty map rectangle.
 *
 * @param rectangle  A pointer to the dirty rectangle to handle.
 * @param mapview    A pointer to the mapview to enqueue changes to.
 */
static void
handle_dirty_rectangle (gpointer rectangle,
                        gpointer mapview);


/**
 * Renders the map layers, one by one, in order.
 *
 * @param mapview    Pointer to the map view to render.
 */
static void
render_map_layers (mapview_t *mapview);


/**
 * Renders the tile compnent of a given layer on a map.
 *
 * @param mapview    A pointer to the map view to render.
 * @param layer      The index of the layer to render.
 */
static void
render_map_layer_tiles (mapview_t *mapview,
                        layer_index_t layer);


/**
 * Renders a given rectangular layer "slice" on a map.
 *
 * @param rectangle  A pointer to the dirty rectangle to render.
 * @param data       A pointer to the data required to perform the
 *                   render.
 */
static void
render_map_layer_tile_rect (gpointer rectangle,
                            gpointer data);


/**
 * Render any map objects to be placed on top of this layer.
 *
 * This will, if this layer is the first defined with its tag, blit
 * all dirty objects of the same tag on top of this map layer in
 * z-order.
 *
 * @param mapview  Pointer to the map view to render.
 * @param layer    The layer to render.
 */
static void
render_map_layer_objects (mapview_t *mapview,
                          layer_index_t layer);


/**
 * Renders a map object.
 *
 * @param mapview  Pointer to the map view to render.
 * @param object   The object to render.
 */
static void
render_map_layer_object (mapview_t *mapview,
                         object_t *object);


/**
 * Renders a map object.
 *
 * @param mapview  Pointer to the map view to render.
 * @param image    The object image to render.
 */
static void
render_map_layer_object_image (mapview_t *mapview,
                               object_image_t *image);


/* -- GLOBAL DEFINITIONS -- */

/* Renders the map. */
void
render_map (mapview_t *mapview)
{
  g_assert (mapview != NULL);
  g_assert (mapview->map != NULL);

  if (mapview->dirty_rectangles == NULL)
    {
      /* Nothing to render! */
      return;
    }

  g_slist_foreach (mapview->dirty_rectangles,
                   handle_dirty_rectangle,
                   mapview);

  render_map_layers (mapview);

  g_slist_free_full (mapview->dirty_rectangles, free);
  mapview->dirty_rectangles = NULL;
}


/* -- STATIC DEFINITIONS -- */

/* Handles a dirty map rectangle. */
static void
handle_dirty_rectangle (gpointer rectangle,
                        gpointer mapview)
{
  dirty_rectangle_t *rectanglec = (dirty_rectangle_t *) rectangle;
  mapview_t *mapviewc = (mapview_t *) mapview;

  /* Check to see if the rectangle is off-screen.  If so, don't
   * bother rendering it!
   */
  if ((rectanglec->start_x >= SCREEN_W + mapviewc->x_offset)
      || (rectanglec->start_y >= SCREEN_H + mapviewc->y_offset)
      || (rectanglec->start_x + rectanglec->width
          <= mapviewc->x_offset)
      || (rectanglec->start_y + rectanglec->height
          <= mapviewc->y_offset))
    {
      return;
    }

  /* Translate absolute co-ordinates into screen ones, and
   * propagate the dirty rectangle to the graphics subsystem.
   */
  add_update_rectangle (rectanglec->start_x - mapviewc->x_offset,
                        rectanglec->start_y - mapviewc->y_offset,
                        rectanglec->width,
                        rectanglec->height);

  /* Check to see if each object in the hash table is going to be
   * dirtied.
   */
  apply_to_objects (dirty_object_test,
                    rectanglec);
}


/* Renders the map layers, one by one, in order. */
static void
render_map_layers (mapview_t *mapview)
{
  layer_count_t l;
  for (l = 0; l <= get_max_layer (mapview->map); l += 1)
    {
      render_map_layer_tiles (mapview, l);
      render_map_layer_objects (mapview, l);
    }
}


/* Renders the tile component of a given layer on a map. */
static void
render_map_layer_tiles (mapview_t *mapview,
                        layer_index_t layer)
{
  render_map_layer_tile_rect_data_t data;
  image_t *tileset = load_image (FN_TILESET);

  if (tileset == NULL)
    {
      fatal ("MAPVIEW - render_map_layer_tiles - Couldn't load tileset.");
    }

  data.mapview = mapview;
  data.tileset = tileset;
  data.layer = layer;
  g_slist_foreach (mapview->dirty_rectangles,
                   render_map_layer_tile_rect,
                   &data);
}


/* Renders a given rectangular "slice" of a layer on a map. */
static void
render_map_layer_tile_rect (gpointer rectangle,
                            gpointer data)
{
  dirty_rectangle_t *rectanglec = rectangle;
  render_map_layer_tile_rect_data_t *datac = data;
  map_t *map = datac->mapview->map;
  image_t *tileset = datac->tileset;

  /* Convert per-pixel dimensions into per-tile */
  int tile_start_x = rectanglec->start_x / TILE_W;
  int tile_start_y = rectanglec->start_y / TILE_H;
  int tile_end_x = (rectanglec->start_x + rectanglec->width) / TILE_W;
  int tile_end_y = (rectanglec->start_y + rectanglec->height) / TILE_H;

  int x;
  int y;
  int screen_x;
  int screen_y;
  int tileset_x;
  layer_value_t tile;

  /* Because the end X and Y round up to the nearest tile, add another
   * tile to them if they don't already line up perfectly with the
   * tile grid.  Else we'll have an off-by-one error.
   */
  if ((rectanglec->start_x + rectanglec->width) % TILE_W > 0)
    {
      tile_end_x += 1;
    }

  if ((rectanglec->start_y + rectanglec->height) % TILE_H > 0)
    {
      tile_end_y += 1;
    }

  for (x = tile_start_x; x < tile_end_x; x += 1)
    {
      if (x > map->width || x < 0)
        continue;

      screen_x = (x * TILE_W) - datac->mapview->x_offset;

      for (y = tile_start_y; y < tile_end_y; y += 1)
        {
          if (y > map->height || y < 0)
            continue;

          screen_y = (y * TILE_H) - datac->mapview->y_offset;

          tile
            = map->value_planes[datac->layer][x + (y * map->height)];
          /* 0 = transparency */
          if (tile > 0)
            {
              tileset_x = TILE_W * tile;
              draw_image_direct (tileset,
                                 (int16_t) tileset_x, 0,
                                 (int16_t) screen_x,
                                 (int16_t) screen_y,
                                 TILE_W, TILE_H);
            }
        }
    }
}


/* Renders any map objects to be placed on top of the given layer. */
static void
render_map_layer_objects (mapview_t *mapview,
                          layer_index_t layer)
{
  layer_value_t tag = get_layer_tag (mapview->map, layer);
  gpointer object;

  if (tag == NULL_TAG)
    {
      return;  /* Null-tag layers do not have objects on them. */
    }

  while (mapview->object_queue[tag - 1] != NULL)
    {
      object = g_slist_nth_data (mapview->object_queue[tag - 1], 0);
      mapview->object_queue[tag - 1] =
        g_slist_remove (mapview->object_queue[tag - 1],
                        object);

      render_map_layer_object (mapview,
                         object);
    }
}


/* Renders a map object. */
static void
render_map_layer_object (mapview_t *mapview,
                         object_t *object)
{
  g_assert (object != NULL && object->image != NULL);

  render_map_layer_object_image (mapview, object->image);
  object->is_dirty = FALSE;
}


/* Renders a map object image. */
static void
render_map_layer_object_image (mapview_t *mapview,
                               object_image_t *image)
{
  g_assert (image != NULL && image->filename != NULL);

  draw_image (image->filename,
              image->image_x,
              image->image_y,
              (short) (image->map_x - mapview->x_offset),
              (short) (image->map_y - mapview->y_offset),
              image->width,
              image->height);
}
