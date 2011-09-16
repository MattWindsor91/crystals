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

/**
 * Width of each tile, in pixels.
 */
const uint16_t TILE_W = 32;


/**
 * Height of each tile, in pixels.
 */
const uint16_t TILE_H = 32;


const char FN_TILESET[] = "tiles.png";	/**< Tileset filename. */


/* -- STATIC DECLARATIONS -- */

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
compare_objects_by_image_y_order (gconstpointer a, gconstpointer b);


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
				   sizeof (struct GSList *));

  /* Set all tiles as dirty. */
  mark_dirty_rect (mapview,
		   0, 0,
                   (int32_t) map->width * TILE_W,
                   (int32_t) map->height * TILE_H);
  render_map (mapview);

  return mapview;
}


/* Adds an object sprite to the rendering queue. */
void
add_object_image (mapview_t *mapview, struct object *object)
{
  g_assert (mapview != NULL);
  g_assert (object != NULL);
  g_assert (object->tag != NULL_TAG);
  g_assert (object->tag <= mapview->num_object_queues);
  g_assert (object->image != NULL);
  g_assert (object->image->filename != NULL);
  g_assert (object->image->width != 0 && object->image->height != 0);

  mapview->object_queue[object->tag - 1] =
    g_slist_insert_sorted (mapview->object_queue[object->tag - 1],
			   object, compare_objects_by_image_y_order);
}


/* Scroll the map on-screen, re-rendering it in its new position. */
void
scroll_map (mapview_t *mapview, int16_t x_offset, int16_t y_offset)
{
  g_assert (mapview != NULL);
  g_assert (x_offset != (int16_t) SHRT_MIN);
  g_assert (y_offset != (int16_t) SHRT_MIN);

  /* Work out the dirty rectangles to mark. */

  /* West scroll. */
  if (x_offset < 0)
    {
      mark_dirty_rect (mapview,
		       mapview->x_offset,
		       mapview->y_offset,
		       (dimension_t) abs ((int) x_offset),
                       SCREEN_H);
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
		       SCREEN_W,
                       (dimension_t) abs ((int) y_offset));
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

  scroll_screen ((int16_t) -(x_offset), (int16_t) -(y_offset));

  render_map (mapview);
}


/* Marks a rectangle of tiles as being dirty. */
void
mark_dirty_rect (mapview_t *mapview,
		 int32_t start_x,
		 int32_t start_y, int32_t width, int32_t height)
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

  mapview->dirty_rectangles =
    g_slist_prepend (mapview->dirty_rectangles, rect);
}


/* De-initialises a mapview. */
void
free_mapview (mapview_t *mapview)
{
  if (mapview)
    {
      if (mapview->object_queue)
	{
	  layer_tag_t i;
	  for (i = 0; i < mapview->num_object_queues; i += 1)
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


/* -- STATIC DEFINITIONS -- */

/* Compares two objects by the position of their image baselines. */
static gint
compare_objects_by_image_y_order (gconstpointer a, gconstpointer b)
{
  object_t *ac = (object_t *) a;
  object_t *bc = (object_t *) b;
  uint32_t a_baseline;
  uint32_t b_baseline;

  g_assert (ac != NULL);
  g_assert (ac->image != NULL);
  g_assert (bc != NULL);
  g_assert (bc->image != NULL);

  a_baseline = (uint32_t) (ac->image->map_y + ac->image->height);
  b_baseline = (uint32_t) (bc->image->map_y + bc->image->height);

  return (gint)(a_baseline - b_baseline);
}
