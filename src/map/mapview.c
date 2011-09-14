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
 * @file    src/field/mapview.c
 * @author  Matt Windsor
 * @brief   Map rendering functions.
 */


#include "../crystals.h"


/* -- CONSTANTS -- */

const uint16_t TILE_W = 32;  /**< Width of each tile, in pixels. */
const uint16_t TILE_H = 32;  /**< Height of each tile, in pixels. */
const char FN_TILESET[] = "tiles.png";  /**< Tileset filename. */


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
 * Renders a given layer on a map.
 *
 * @param mapview    Pointer to the map view to render.
 *
 * @param layer      The layer to render.
 */
static void
render_map_layer (mapview_t *mapview,
                  layer_index_t layer);

/**
 * Renders a given rectangular layer "slice" on a map.
 *
 * @param rectangle  A pointer to the dirty rectangle to render.
 * @param data       A pointer to the data required to perform the render.
 */
static void
render_rect_layer (gpointer rectangle,
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
render_map_objects (mapview_t *mapview,
                    layer_index_t layer);


/**
 * Renders a map object.
 *
 * @param mapview  Pointer to the map view to render.
 * @param object   The object to render.
 */
static void
render_map_object (mapview_t *mapview,
                   object_t *object);


/**
 * Renders a map object.
 *
 * @param mapview  Pointer to the map view to render.
 * @param image    The object image to render.
 */
static void
render_map_object_image (mapview_t *mapview,
                         object_image_t *image);


/**
 * Compares two objects by the position of their image baselines.
 *
 * @param a  The first object to be used in the comparison.
 * @param b  The second object to be used in the comparison.
 *
 * @return a positive number if a should be rendered in front of b,
 *         a negative number if b should be rendered in front of a,
 *         or 0 if the two are equal in Y-order.
 */
static gint
compare_objects_by_image_y_order (gconstpointer a,
                                  gconstpointer b);


/** Structure of data needed during a rectangle layer render pass. */
typedef struct render_rect_layer_data
{
  mapview_t *mapview;
  image_t *tileset;
  layer_index_t layer;
} render_rect_layer_data_t;


/* -- DEFINITIONS -- */

mapview_t *
init_mapview (map_t *map)
{
  mapview_t *mapview = xcalloc (1, sizeof (mapview_t));
  g_assert (map != NULL);
  g_assert (map->width > 0 && map->height > 0);

  mapview->map = map;

  /* Get the number of object queues to reserve, by finding the
     highest tag number in the map. */
  mapview->num_object_queues = get_max_tag (mapview->map);

  /* There should be at least one tag! */
  g_assert (mapview->num_object_queues != 0);

  mapview->object_queue = xcalloc (mapview->num_object_queues,
                                   sizeof (GSList*));

  /* Set all tiles as dirty. */
  mark_dirty_rect (mapview, 0, 0, map->width * TILE_W, map->height * TILE_H);
  render_map (mapview);

  return mapview;
}


/* Adds an object sprite to the rendering queue. */
void
add_object_image (mapview_t *mapview,
                  object_t *object)
{
  g_assert (mapview != NULL);
  g_assert (object != NULL);
  g_assert (object->tag != NULL_TAG);
  g_assert (object->tag <= mapview->num_object_queues);
  g_assert (object->image != NULL);
  g_assert (object->image->filename != NULL);
  g_assert (object->image->width != 0
            && object->image->height != 0);

  mapview->object_queue[object->tag - 1] =
    g_slist_insert_sorted (mapview->object_queue[object->tag - 1],
                           object,
                           compare_objects_by_image_y_order);
}


/* Compares two objects by the position of their image baselines. */
static gint
compare_objects_by_image_y_order (gconstpointer a,
                                  gconstpointer b)
{
  object_t *ac = (object_t*) a;
  object_t *bc = (object_t*) b;
  uint32_t a_baseline;
  uint32_t b_baseline;

  g_assert (ac != NULL);
  g_assert (ac->image != NULL);
  g_assert (bc != NULL);
  g_assert (bc->image != NULL);

  a_baseline = ac->image->map_y + ac->image->height;
  b_baseline = bc->image->map_y + bc->image->height;

  return (a_baseline - b_baseline);
}


/* Renders the map. */
void
render_map (mapview_t *mapview)
{
  unsigned char l;

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

  /* Render a layer, then the objects tagged with that layer. */
  for (l = 0; l <= get_max_layer (mapview->map); l++)
    {
      render_map_layer (mapview, l);
      render_map_objects (mapview, l);
    }

  g_slist_free_full (mapview->dirty_rectangles, free);
  mapview->dirty_rectangles = NULL;
}


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

  /* Check to see if each object in the hash table is going to be dirtied. */
  apply_to_objects (dirty_object_test,
                    rectanglec);
}


/* Render a given layer on a map. */
static void
render_map_layer (mapview_t *mapview,
                  layer_index_t layer)
{
  render_rect_layer_data_t data;
  image_t *tileset = load_image (FN_TILESET);

  if (tileset == NULL)
    {
      fatal ("MAPVIEW - render_map_layer - Couldn't load tileset.");
    }

  data.mapview = mapview;
  data.tileset = tileset;
  data.layer = layer;
  g_slist_foreach (mapview->dirty_rectangles,
                   render_rect_layer,
                   &data);
}


/* Render a given rectangular "slice" of a layer on a map. */
static void
render_rect_layer (gpointer rectangle,
                   gpointer data)
{
  dirty_rectangle_t *rectanglec = rectangle;
  render_rect_layer_data_t *datac = data;
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

  /* Because the end X and Y round up to the nearest tile, add another tile
   * to them if they don't already line up perfectly with the tile grid.
   * Else we'll have an off-by-one error.
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

          tile = map->value_planes[datac->layer][x + (y * map->height)];
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


/* Render any map objects to be placed on top of this layer. */
static void
render_map_objects (mapview_t *mapview,
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

      render_map_object (mapview,
                         object);
    }
}


/* Renders a map object. */
static void
render_map_object (mapview_t *mapview,
                   object_t *object)
{
  g_assert (object != NULL && object->image != NULL);

  render_map_object_image (mapview, object->image);
  object->is_dirty = FALSE;
}


/* Renders a map object image. */
static void
render_map_object_image (mapview_t *mapview,
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


/* Scroll the map on-screen, re-rendering it in its new position. */
void
scroll_map (mapview_t *mapview,
            int16_t x_offset,
            int16_t y_offset)
{
  g_assert (mapview != NULL);
  g_assert (x_offset != SHRT_MIN && y_offset != SHRT_MIN);

  /* Work out the dirty rectangles to mark. */

  /* West scroll. */
  if (x_offset < 0)
    {
       mark_dirty_rect (mapview,
                        mapview->x_offset,
                        mapview->y_offset,
                        (dimension_t) abs (x_offset), SCREEN_H);
    }

  /* East scroll. */
  else if (x_offset > 0)
    {
      mark_dirty_rect (mapview,
                       SCREEN_W + mapview->x_offset - x_offset,
                       mapview->y_offset,
                       (dimension_t) x_offset, SCREEN_H);
    }


  /* North scroll. */
  if (y_offset < 0)
    {
      mark_dirty_rect (mapview,
                       mapview->x_offset,
                       mapview->y_offset,
                       SCREEN_W, (dimension_t) abs (y_offset));
    }

  /* South scroll. */
  else if (y_offset > 0)
    {
      mark_dirty_rect (mapview,
                       mapview->x_offset,
                       SCREEN_H + mapview->y_offset - y_offset,
                       SCREEN_W, (dimension_t) y_offset);
    }

  mapview->x_offset += x_offset;
  mapview->y_offset += y_offset;

  scroll_screen ((short) -(x_offset),
                 (short) -(y_offset));

  render_map (mapview);
}


/* Marks a rectangle of tiles as being dirty. */
void
mark_dirty_rect (mapview_t *mapview,
                 int32_t start_x,
                 int32_t start_y,
                 int32_t width,
                 int32_t height)
{
  dirty_rectangle_t *rect;

  g_assert (mapview != NULL);
  g_assert (width > 0 && height > 0);

  rect = xcalloc (1, sizeof (dirty_rectangle_t));
  rect->parent = mapview;
  rect->start_x = start_x;
  rect->start_y = start_y;
  rect->width = width;
  rect->height = height;

  mapview->dirty_rectangles = g_slist_prepend (mapview->dirty_rectangles, rect);
}


/* De-initialises a mapview. */
void
free_mapview (mapview_t *mapview)
{
  if (mapview)
    {
      if (mapview->object_queue)
        {
          uint16_t i;
          for (i = 0; i < mapview->num_object_queues; i++)
            {
              /* Objects in queue are freed in the object system. */
              g_slist_free (mapview->object_queue[i]);
            }

          free (mapview->object_queue);
        }

      if (mapview->dirty_rectangles)
        {
          g_slist_free_full (mapview->dirty_rectangles, free);
          mapview->dirty_rectangles = NULL;
        }

      free (mapview);
    }
}
