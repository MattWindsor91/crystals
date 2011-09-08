/*
 * Crystals (working title) 
 *
 * Copyright (c) 2010 Matt Windsor, Michael Walker and Alexander
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

/** @file    src/field/mapview.c
 *  @author  Matt Windsor
 *  @brief   Map rendering functions.
 */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <glib-2.0/glib/gstrfuncs.h>
#include <glib-2.0/glib/gslist.h>

#include "mapview.h"
#include "map.h"
#include "object.h"
#include "object-image.h"
#include "../util.h"
#include "../module.h"
#include "../graphics.h"


/* -- CONSTANTS -- */

const uint16_t TILE_W = 32;
const uint16_t TILE_H = 32;


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
 *
 * @param layer    The layer to render.
 */

static void
render_map_objects (mapview_t *mapview, layer_index_t layer);


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
  if (map == NULL)
    {
      error ("MAPVIEW - init_mapview - Passed map pointer is null.");
      return NULL;
    }
  else if (map->width <= 0 || map->height <= 0)
    {
      error ("MAPVIEW - init_mapview - Map W/H are non-positive.");
      return NULL;
    }
  
  {
    mapview_t *mapview = calloc (1, sizeof (mapview_t));
    if (mapview == NULL)
      {
        error ("MAPVIEW - init_mapview - Couldn't allocate map view.");
        return NULL;
      }

    mapview->map = map;

    /* Get the number of object queues to reserve, by finding the
       highest tag number in the map. */
    mapview->num_object_queues = get_max_tag (mapview->map);  
    /* There should be at least one tag! */
    if (mapview->num_object_queues == 0)
      {
        error ("MAPVIEW - init_mapview - No tags in map; please fix map.");
        free_mapview (mapview);
        return NULL;
      }

    mapview->object_queue = calloc (mapview->num_object_queues,
                                    sizeof (object_image_t*));
    if (mapview->object_queue == NULL)
      {
        error ("MAPVIEW - init_mapview - Couldn't allocate object queues.");
        free_mapview (mapview);
        return NULL;
      }

    /* Set all tiles as dirty. */
    mark_dirty_rect (mapview, 0, 0, map->width * TILE_W, map->height * TILE_H);
    render_map (mapview);
    
    return mapview;
  }
}


/* Add an object sprite to the rendering queue. */

bool_t
add_object_image (mapview_t *mapview,
                  layer_value_t tag,
                  object_t *object)
{
  if (mapview == NULL)
    {
      error ("MAPVIEW - add_object_image - Tried to render to NULL mapview.");
      return FAILURE;
    }
  else if (tag == NULL_TAG)
    {
      /* Can't render to the null tag - it's the standard "don't render" tag
       * index.
       */
      error ("MAPVIEW - add_object_image - Tried to render to null tag.");
      return FAILURE;
    }
  else if (tag > mapview->num_object_queues)
    {
      /* Equally, can't render to a tag that's above the highest tag
       * number in the map viewpoint. 
       *
       * NOTE: We don't check to ensure that our tag is actually being
       * provided, just that it's a sane number. Possible FIXME in the
       * future?
       */
      error ("MAPVIEW - add_object_image - Tag specified too high.");
      return FAILURE;
    }
  else if (object == NULL)
    {
      error ("MAPVIEW - add_object_image - Tried to render a NULL object.");
      return FAILURE;
    }

  /* Assert that a non-NULL mapview must have a non-NULL render queue
   * array, due to the nature of the mapview init.
   */
  
  {
    object_image_t *image = get_object_image (object);
    if (image == NULL)
      {
        error ("MAPVIEW - add_object_image - Tried to render a NULL image.");
        return FAILURE;
      }
    else if (image->filename == NULL)
      {
        error ("MAPVIEW - add_object_image - Filename is NULL.");
        return FAILURE;
      }
    else if (image->width == 0 || image->height == 0)
      {
        error ("MAPVIEW - add_object_image - Zero object render width/height.");
        return FAILURE;
      }

    /* Now copy the image, if possible. */
    {
      render_node_t *new_rnode = malloc (sizeof (render_node_t));
      if (new_rnode == NULL)
        {
          error ("MAPVIEW - add_object_image - Allocation failed for rnode.");
          return FAILURE;
        }

      new_rnode->object = object;
      new_rnode->image = image;
      new_rnode->next = NULL;

      /* Finally, add to the proper tag queue.
       *
       *  If the queue is empty or the first queue item is in front, then
       *  just add it to the queue head.
       *  Otherwise, find a space in the queue before the first item with a
       *  screen_y higher (further down the screen) than this object, to
       *  preserve z-order.
       */  
      if (mapview->object_queue[tag - 1] == NULL
          || ((mapview->object_queue[tag - 1]->image->map_y
               + mapview->object_queue[tag - 1]->image->height)
              > (image->map_y + image->height)))
        {
          new_rnode->next = mapview->object_queue[tag - 1];
          mapview->object_queue[tag - 1] = new_rnode;

          return SUCCESS;
        }
      else
        {
          render_node_t *ptr;
 
          /* Skip to the first item that either has a null neighbour 
           * or a neighbour whose screen_y + width is bigger.
           */
          for (ptr = mapview->object_queue[tag - 1]; 
               ptr->next != NULL
                 && ((ptr->next->image->map_y + ptr->next->image->height)
                     <= (image->map_y + image->height)); 
               ptr = ptr->next)
            ;

          /* Insert the new image here. */
          new_rnode->next = ptr->next;
          ptr->next = new_rnode;

          return SUCCESS;
      }
    }
  }
}


void
render_map (mapview_t *mapview)
{
  if (mapview == NULL)
    {
      error ("MAPVIEW - render_map - mapview is NULL");
    }
  else if (mapview->map == NULL)
    {
       error ("MAPVIEW - render_map - mapview->map is NULL");
    }
  else if (mapview->dirty_rectangles == NULL)
    {
      /* Nothing to render! */
      return;
    }

  g_slist_foreach (mapview->dirty_rectangles,
                   handle_dirty_rectangle,
                   mapview);

  {
    /* Render a layer, then the objects tagged with that layer. */
    unsigned char l;
    for (l = 0; l <= get_max_layer (mapview->map); l++)
      {
        render_map_layer (mapview, l);
        render_map_objects (mapview, l);
      }
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
render_map_layer (mapview_t *mapview, layer_index_t layer)
{
  image_t *tileset = load_image (FN_TILESET);
  if (tileset == NULL)
    {
      fatal ("MAPVIEW - render_map_layer - Couldn't load tileset.");
      return;
    }

  {
    render_rect_layer_data_t data;
    data.mapview = mapview;
    data.tileset = tileset;
    data.layer = layer;
    g_slist_foreach (mapview->dirty_rectangles,
                     render_rect_layer,
                     &data);
  }
}

/* Render a given rectangular "slice" of a layer on a map. */
static void
render_rect_layer (gpointer rectangle, gpointer data)
{
  dirty_rectangle_t *rectanglec = rectangle;
  render_rect_layer_data_t *datac = data;
  map_t *map = datac->mapview->map;
  image_t *tileset = datac->tileset;
  
  {
    /* Convert per-pixel dimensions into per-tile */
    int tile_start_x = rectanglec->start_x / TILE_W;
    int tile_start_y = rectanglec->start_y / TILE_H;
    int tile_end_x = (rectanglec->start_x + rectanglec->width) / TILE_W;
    int tile_end_y = (rectanglec->start_y + rectanglec->height) / TILE_H;
    
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
    
    {
      int x;
      int y;
      int screen_x;
      int screen_y;
      int tileset_x;
      layer_value_t tile;

      for (x = tile_start_x; x < tile_end_x; x += 1)
        {
          screen_x = (x * TILE_W) - datac->mapview->x_offset; 
          
          for (y = tile_start_y; y < tile_end_y; y += 1)
            {
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
  }
}


/* Render any map objects to be placed on top of this layer. */

static void
render_map_objects (mapview_t *mapview, layer_index_t layer)
{
  layer_value_t tag = get_layer_tag (mapview->map, layer);

  if (tag > NULL_TAG)
    {
      render_node_t *next;
      object_image_t *image;

      while (mapview->object_queue[tag - 1] != NULL)
        {
          next = mapview->object_queue[tag - 1]->next;

          image = mapview->object_queue[tag - 1]->image;

          if (image == NULL)
            {
              error ("MAPVIEW - render_map_objects - Failed to get object image.");
              return;
            }

          draw_image (image->filename,
                      image->image_x,
                      image->image_y,
                      (short) (image->map_x - mapview->x_offset),
                      (short) (image->map_y - mapview->y_offset),
                      image->width,
                      image->height);

          /* Mark the parent object as no longer dirty. */
          if (mapview->object_queue[tag - 1] != NULL)
            mapview->object_queue[tag - 1]->object->is_dirty = FALSE;

          free (mapview->object_queue[tag - 1]);

          mapview->object_queue[tag - 1] = next;
        }
    } 
}


/* Scroll the map on-screen, re-rendering it in its new position. */

void
scroll_map (mapview_t *mapview,
            int16_t x_offset,
            int16_t y_offset)
{
  if (mapview == NULL)
    {
      error ("MAPVIEW - scroll_map - Map view is NULL.");
      return;
    }

  /* This particular number will pose problems later 
     when flipping to negative, so don't allow it. */

  if (x_offset == SHRT_MIN
      || y_offset == SHRT_MIN)
    {
      error ("MAPVIEW - scroll_map - Offset too big.");
      return;
    }

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


/** Mark a rectangle of tiles as being dirty. */

bool_t
mark_dirty_rect (mapview_t *mapview,
                 int32_t start_x,
                 int32_t start_y,
                 int32_t width,
                 int32_t height)
{
  if (mapview == NULL)
    {
      error ("MAPVIEW - mark_dirty_rect - Rect dirtying passed NULL mapview.");
      return FAILURE;
    }
  else if (width <= 0 || height <= 0)
    {
      error ("MAPVIEW - mark_dirty_rect - Rect dirtying passed invalid width/height.");
      return FAILURE;
    }

  {
    dirty_rectangle_t *rect = malloc (sizeof (dirty_rectangle_t));
    if (rect == NULL)
      {
        error ("MAPVIEW - mark_dirty_rect - Couldn't allocate rect.");
        return FAILURE;
      }
  
    rect->parent = mapview;
    rect->start_x = start_x;
    rect->start_y = start_y;
    rect->width = width;
    rect->height = height;
  
    mapview->dirty_rectangles
      = g_slist_prepend (mapview->dirty_rectangles, rect);
  }

  return SUCCESS;
}


/* De-initialise a mapview. */

void
free_mapview (mapview_t *mapview)
{
  if (mapview)
    {
      if (mapview->object_queue)
        {
          unsigned int i;
          render_node_t *next;

          for (i = 0; i < mapview->num_object_queues; i++)
            {
              while (mapview->object_queue[i] != NULL)
                {             
                  next = mapview->object_queue[i]->next;
                  free (mapview->object_queue[i]);
                  mapview->object_queue[i] = next;
                }
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
