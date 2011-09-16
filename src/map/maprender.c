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
static void handle_dirty_rectangle (gpointer rectangle,
				    gpointer mapview);


/**
 * Propagates a dirty rectangle to the graphics subsystem.
 *
 * @param rectangle  Pointer to the rectangle to propagate.
 * @param mapview    Pointer to the map viewport to use in the
 *                   propagation.
 */
static void propagate_rectangle_to_screen (dirty_rectangle_t
					   *rectangle,
					   mapview_t *mapview);


/**
 * Clamps a dirty rectangle to the map dimensions in place.
 *
 * @param rectangle  Pointer to the rectangle to clamp.  The rectangle
 *                   is modified by this procedure.
 */
static void clamp_rectangle (dirty_rectangle_t *rectangle);


/**
 * Renders the map layers, one by one, in order.
 *
 * @param mapview    Pointer to the map view to render.
 */
static void render_map_layers (mapview_t *mapview);


/**
 * Renders the tile compnent of a given layer on a map.
 *
 * @param mapview    A pointer to the map view to render.
 * @param layer      The index of the layer to render.
 */
static void render_map_layer_tiles (mapview_t *mapview,
				    layer_index_t layer);


/**
 * Renders a given rectangular layer "slice" on a map.
 *
 * @param rectangle  A pointer to the dirty rectangle to render.
 * @param data       A pointer to the data required to perform the
 *                   render.
 */
static void render_map_layer_tile_rect (gpointer rectangle,
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
static void render_map_layer_objects (mapview_t *mapview,
				      layer_index_t layer);


/**
 * Renders a map object.
 *
 * @param mapview  Pointer to the map view to render.
 * @param object   The object to render.
 */
static void render_map_layer_object (mapview_t *mapview,
				     object_t *object);


/**
 * Renders a map object.
 *
 * @param mapview  Pointer to the map view to render.
 * @param image    The object image to render.
 */
static void render_map_layer_object_image (mapview_t *mapview,
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
		   handle_dirty_rectangle, mapview);

  render_map_layers (mapview);

  g_slist_free_full (mapview->dirty_rectangles, free);
  mapview->dirty_rectangles = NULL;
}


/* -- STATIC DEFINITIONS -- */

/* Handles a dirty map rectangle. */
static void
handle_dirty_rectangle (gpointer rectangle, gpointer mapview)
{
  dirty_rectangle_t *rectanglec = (dirty_rectangle_t *) rectangle;
  mapview_t *mapviewc = (mapview_t *) mapview;

  /* Check to see if the rectangle is off-screen.  If so, don't
   * bother rendering it!
   */
  if ((rectanglec->start_x
       >= (int32_t) (SCREEN_W + mapviewc->x_offset))
      || (rectanglec->start_y
	  >= (int32_t) (SCREEN_H + mapviewc->y_offset))
      || (mapviewc->x_offset >= 0
	  && ((uint32_t) (rectanglec->start_x + rectanglec->width)
	      <= (uint32_t) mapviewc->x_offset))
      || (mapviewc->y_offset >= 0
	  && ((uint32_t) (rectanglec->start_y + rectanglec->height)
	      <= (uint32_t) mapviewc->y_offset)))
    return;

  propagate_rectangle_to_screen (rectanglec, mapview);
  clamp_rectangle (rectanglec);

  /* Check to see if each object in the hash table is going to be
   * dirtied.
   */
  apply_to_objects (dirty_object_test, rectanglec);
}


/* Propagates a dirty rectangle to the graphics subsystem. */
static void
propagate_rectangle_to_screen (dirty_rectangle_t *rectangle,
			       mapview_t *mapview)
{
  int16_t screen_x;
  int16_t screen_y;
  uint16_t screen_width;
  uint16_t screen_height;

  /* Clamp rectangle to screen.
     If the rectangle starts above or to the left of the screen,
     chop the unseen part off. */
  if (rectangle->start_x < mapview->x_offset)
    {
      screen_x = 0;
      screen_width =
	(uint16_t) (rectangle->width -
		    (mapview->x_offset - rectangle->start_x));
    }
  else
    {
      screen_x = (int16_t) (rectangle->start_x - mapview->x_offset);
      screen_width = rectangle->width;
    }
  if (rectangle->start_y < mapview->y_offset)
    {
      screen_y = 0;
      screen_height =
	(uint16_t) (rectangle->height -
		    (mapview->y_offset - rectangle->start_y));
    }
  else
    {
      screen_y = (int16_t) (rectangle->start_y - mapview->y_offset);
      screen_height = rectangle->height;
    }


  /* This is done to hide artefacts when the map is fully
     scrolled. */
  draw_rectangle (screen_x, screen_y, screen_width, screen_height,
		  0, 0, 0);

  /* Propagate to graphics subsystem. */
  add_update_rectangle (screen_x, screen_y, screen_width,
			screen_height);
}


/* Clamps a dirty rectangle to the map dimensions in place. */
static void
clamp_rectangle (dirty_rectangle_t *rectangle)
{
  /* TODO: clamp in other direction too */
  if (rectangle->start_x < 0)
    {
      rectangle->width -= rectangle->start_x;
      rectangle->start_x = 0;
    }
  if (rectangle->start_y < 0)
    {
      rectangle->height -= rectangle->start_y;
      rectangle->start_y = 0;
    }
}

/* Renders the map layers, one by one, in order. */
static void
render_map_layers (mapview_t *mapview)
{
  layer_index_t l;
  for (l = 0; l <= get_max_layer (mapview->map); l += 1)
    {
      render_map_layer_tiles (mapview, l);
      render_map_layer_objects (mapview, l);
    }
}


/* Renders the tile component of a given layer on a map. */
static void
render_map_layer_tiles (mapview_t *mapview, layer_index_t layer)
{
  render_map_layer_tile_rect_data_t data;
  image_t *tileset = load_image (FN_TILESET);

  if (tileset == NULL)
    {
      fatal
	("MAPVIEW - render_map_layer_tiles - Couldn't load tileset.");
    }

  data.mapview = mapview;
  data.tileset = tileset;
  data.layer = layer;
  g_slist_foreach (mapview->dirty_rectangles,
		   render_map_layer_tile_rect, &data);
}


/* Renders a given rectangular "slice" of a layer on a map. */
static void
render_map_layer_tile_rect (gpointer rectangle, gpointer data)
{
  dirty_rectangle_t *rectanglec = rectangle;
  render_map_layer_tile_rect_data_t *datac = data;
  map_t *map = datac->mapview->map;
  image_t *tileset = datac->tileset;

  /* Convert per-pixel dimensions into per-tile */
  dimension_t tile_start_x =
    (dimension_t) (rectanglec->start_x / TILE_W);
  dimension_t tile_start_y =
    (dimension_t) (rectanglec->start_y / TILE_H);
  dimension_t tile_end_x =
    (dimension_t) ((rectanglec->start_x +
		    rectanglec->width) / TILE_W);
  dimension_t tile_end_y =
    (dimension_t) ((rectanglec->start_y +
		    rectanglec->height) / TILE_H);

  dimension_t x;
  dimension_t y;
  int screen_x;
  int screen_y;
  int tileset_x;
  layer_value_t tile;


  /* Because the end X and Y round up to the nearest tile, add another
   * tile to them if they don't already line up perfectly with the
   * tile grid.  Else we'll have an off-by-one error.
   */
  if ((rectanglec->start_x + rectanglec->width) % TILE_W > 0)
    tile_end_x += 1;

  if ((rectanglec->start_y + rectanglec->height) % TILE_H > 0)
    tile_end_y += 1;

  /* Ensure rectangle doesn't go out of map bounds. */
  if (map->width <= tile_end_x)
    tile_end_x = map->width - 1;
  if (map->height <= tile_end_y)
    tile_end_y = map->height - 1;

  /* Ensure rectangle is still a rectangle */
  if (tile_start_x >= tile_end_x)
    return;
  if (tile_start_y >= tile_end_y)
    return;

  for (x = tile_start_x; x < tile_end_x; x += 1)
    {
      screen_x = (x * TILE_W) - datac->mapview->x_offset;

      for (y = tile_start_y; y < tile_end_y; y += 1)
	{
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
				 (int16_t) screen_y, TILE_W, TILE_H);
	    }
	}
    }
}


/* Renders any map objects to be placed on top of the given layer. */
static void
render_map_layer_objects (mapview_t *mapview, layer_index_t layer)
{
  layer_value_t tag = get_layer_tag (mapview->map, layer);
  gpointer object;

  /* Null-tag layers do not have objects on them. */
  if (tag == NULL_TAG)
    return;

  while (mapview->object_queue[tag - 1] != NULL)
    {
      object = g_slist_nth_data (mapview->object_queue[tag - 1], 0);
      mapview->object_queue[tag - 1] =
	g_slist_remove (mapview->object_queue[tag - 1], object);

      render_map_layer_object (mapview, object);
    }
}


/* Renders a map object. */
static void
render_map_layer_object (mapview_t *mapview, object_t *object)
{
  g_assert (object != NULL && object->image != NULL);

  render_map_layer_object_image (mapview, object->image);
  object->is_dirty = false;
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
	      (int16_t) (image->map_x - mapview->x_offset),
	      (int16_t) (image->map_y - mapview->y_offset),
	      image->width, image->height);
}
