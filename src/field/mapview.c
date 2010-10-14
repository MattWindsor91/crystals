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

#include "mapview.h"
#include "map.h"
#include "object.h"
#include "../util.h"
#include "../module.h"
#include "../graphics.h"

/* -- CONSTANTS -- */

const unsigned short TILE_W = 32; 
const unsigned short TILE_H = 32;

/* -- STATIC DECLARATIONS -- */

/** Add a new dirty rectangle to the mapview's dirty rectangle queue.
 *
 *  @param mapview  Pointer to the map view.
 *
 *  @param start_x  The X co-ordinate of the start of the rectangle,
 *                  as a tile offset from the left edge of the map.
 *
 *  @param start_y  The Y co-ordinate of the start of the rectangle, 
 *                  as a tile offset from the top edge of the map.
 *
 *  @param width    Width of the tile rectangle, in tiles.
 *
 *  @param height   Height of the tile rectangle, in tiles.
 *
 *  @return SUCCESS if there were no errors encountered; FAILURE
 *  otherwise.
 */

static int
add_dirty_rect (struct map_view *mapview,
                int start_x,
                int start_y,
                int width,
                int height);


/* -- DEFINITIONS -- */

struct map_view *
init_mapview (struct map *map)
{
  struct map_view *mapview;

  int width, height; /* Temporary locations for shorthand purposes. */
  unsigned int i;

  if (map == NULL)
    {
      error ("MAPVIEW - init_mapview - Passed map pointer is null.");
      return NULL;
    }

  mapview = malloc (sizeof (struct map_view));

  if (mapview == NULL)
    {
      error ("MAPVIEW - init_mapview - Couldn't allocate map view.");
      return NULL;
    }
            
  mapview->x_offset = 0;
  mapview->y_offset = 0;
  mapview->map = map;
  mapview->object_queue = NULL;
  mapview->dirty_rectangles = NULL;
  
  width = mapview->map->width;
  height = mapview->map->height;

  if (width <= 0 || height <= 0)
    {
      error ("MAPVIEW - init_mapview - Map W/H are non-positive.");
      return NULL;
    }

  mapview->dirty_tiles = malloc (sizeof (unsigned char) *
                                 (unsigned int) (width * height));

  if (mapview->dirty_tiles == NULL)
    {
      error ("MAPVIEW - init_mapview - Couldn't allocate dirty tilemap.");
      cleanup_mapview (mapview);
      return NULL;
    }

  /* Get the number of object queues to reserve, by finding the
     highest tag number in the map. */

  mapview->num_object_queues = get_max_tag (mapview->map);
  

  /* There should be at least one tag! */

  if (mapview->num_object_queues == 0)
    {
      error ("MAPVIEW - init_mapview - No tags in map; please fix map.");
      cleanup_mapview (mapview);
      return NULL;
    }

  /* Now allocate the queues. */

  mapview->object_queue = malloc (sizeof (struct object_image*)
                                  * mapview->num_object_queues);

  if (mapview->object_queue == NULL)
    {
      error ("MAPVIEW - init_mapview - Couldn't allocate object queues.");
      cleanup_mapview (mapview);
      return NULL;
    }

  for (i = 0; i < mapview->num_object_queues; i++)
    mapview->object_queue[i] = NULL;

  /* Set all tiles as dirty. */

  mark_dirty_rect (mapview, 0, 0, map->width * TILE_W, map->height * TILE_H);

  return mapview;
}

int
init_object_image (struct object_image *image, struct object_t *parent)
{
  /* Sanity checking. */

  if (image == NULL)
    {
      error ("MAPVIEW - init_object_image - Tried to init null object image.");
      return FAILURE;
    }

  if (parent == NULL)
    {
      error ("MAPVIEW - init_object_image - Object image has no parent.");
      return FAILURE;
    }

  image->parent = parent;
  image->filename = NULL;
  image->next = NULL;

  image->image_x = 0;
  image->image_y = 0;
  image->map_x = 0;
  image->map_y = 0;
  image->width = 0;
  image->height = 0;

  return SUCCESS;
}


int
add_object_image (struct map_view *mapview,
                  layer_t tag,
                  struct object_t *object)
{
  struct object_rnode *new_rnode;
  struct object_image *image;

  /* First, error checking! */

  /* We definitely need a mapview. */

  if (mapview == NULL)
    {
      error ("MAPVIEW - add_object_image - Tried to render to NULL mapview.");
      return FAILURE;
    }

  /* Also, the object to add should exist. */

  if (object == NULL)
    {
      error ("MAPVIEW - add_object_image - Tried to render a NULL object.");
      return FAILURE;
    }

  image = get_object_image (object);

  if (image == NULL)
    {
      error ("MAPVIEW - add_object_image - Tried to render a NULL image.");
      return FAILURE;
    }

  /* Assert that a non-NULL mapview must have a non-NULL render queue
     array, due to the nature of the mapview init. */

  /* Can't render to tag 0 - it's the standard "don't render" tag
     index. */

  if (tag == 0)
    {
      error ("MAPVIEW - add_object_image - Tried to render to tag 0.");
      return FAILURE;
    }

  /* Equally, can't render to a tag that's above the highest tag
     number in the map viewpoint. 

     NOTE: We don't check to ensure that our tag is actually being
     provided, just that it's a sane number. Possible FIXME in the
     future? */

  if (tag > mapview->num_object_queues)
    {
      error ("MAPVIEW - add_object_image - Tag specified too high.");
      return FAILURE;
    }

  if (image->filename == NULL)
    {
      error ("MAPVIEW - add_object_image - Filename is NULL.");
      return FAILURE;
    }

  if (image->width == 0 || image->height == 0)
    {
      error ("MAPVIEW - add_object_image - Zero object render width/height.");
      return FAILURE;
    }

  /* Now copy the image, if possible. */

  new_rnode = malloc (sizeof (struct object_rnode));

  if (new_rnode == NULL)
    {
      error ("MAPVIEW - add_object_image - Allocation failed for rnode.");
      return FAILURE;
    }

  /* Link to the object and its image. */

  new_rnode->object = object;
  new_rnode->image = image;
  new_rnode->next = NULL;

  /* Finally, add to the proper tag queue.

     If the queue is empty or the first queue item is in front, then
     just add it to the queue head.
     Otherwise, find a space in the queue before the first item with a
     screen_y higher (further down the screen) than this object, to
     preserve z-order.
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
      struct object_rnode *ptr;
 
      /* Skip to the first item that either has a null neighbour 
         or a neighbour whose screen_y + width is bigger. */

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


void
free_object_image (struct object_image *image)
{
  if (image)
    {
      if (image->filename)
        free (image->filename);

      free (image);
    }
}


void
render_map (struct map_view *mapview)
{
  if (mapview)
    {
      if (mapview->map)
        {
          unsigned char l;

          /* Render a layer, then the objects tagged with that layer. */
          for (l = 0; l < mapview->map->num_layers; l++)
            {
              render_map_layer (mapview, l);
              render_map_objects (mapview, l);
            }
        }
    }
}


void
render_map_layer (struct map_view *mapview, unsigned char layer)
{
  short x, y;
  int true_x, true_y, x_offset, y_offset;
  struct map *map;
  struct hash_object *tileset_object;
  void *tileset;

  /* Try to grab the tileset. */

  tileset_object = load_image (FN_TILESET);

  if (tileset_object == NULL)
    {
      fatal ("MAPVIEW - render_map_layer - Couldn't load tileset.");
      return;
    }

  tileset = tileset_object->data;

  if (tileset == NULL)
    {
      fatal ("MAPVIEW - render_map_layer - Tileset has NULL data.");
      return;
    }

  map = mapview->map;
  x_offset = mapview->x_offset;
  y_offset = mapview->y_offset;

  /* The true x and y refer to the tiles on the map, whereas 
     standard x and y refer to the tiles on-screen. To 
     translate from x and y to true_x and true_y, we need 
     only apply the x and y offsets divided to convert from 
     pixel offsets to tile offsets.

     We also scan the tiles just outside screen bounds to 
     cater for tiles that are only partially on the screen. */

  for (x = -1; x < (SCREEN_W / TILE_W) + 1; x++)
    {
      true_x = x + (x_offset / TILE_W);

      if (true_x >= 0 && true_x < map->width)
        {
          for (y = -1; y < (SCREEN_H / TILE_H) + 1; y++)
            {
              true_y = y + (y_offset / TILE_H);

              /* In order to blit an image, the true X and Y must be within the 
                 map bounds, the tile must not be 0 (no tile) and the tile must 
                 be dirty. */

              if (true_y >= 0
                  && true_y < map->height
                  && mapview->dirty_tiles[true_x + (true_y * map->width)] != 0)
                {
                  /* Practically, the drawing X and Y should NEVER be
                     bigger than a short, but it's good to check. 

                     Also, and this will be changed eventually, but
                     the tileset X cannot be bigger than a short.
                  */

                  int screen_x = (x * TILE_W) - (x_offset % TILE_W);
                  int screen_y = (y * TILE_H) - (y_offset % TILE_H);
                  int layer_offset = true_x  + (true_y * map->height);
                  int tileset_x = TILE_W * map->data_layers[layer][layer_offset];

                  if (screen_x < SHRT_MIN
                      || screen_x > SHRT_MAX
                      || screen_y < SHRT_MIN
                      || screen_y > SHRT_MAX)
                    {
                      error ("MAPVIEW - render_map_layer - Screen X and Y are outside short bounds.");
                      fatal ("^-- THIS SHOULD NEVER HAPPEN!");
                      return;
                    }

                  if (layer_offset < 0 
                      || layer_offset >= (map->width * map->height))
                    {
                      fatal ("MAPVIEW - render_map_layer - Layer offset out of bounds.");
                      return;
                    }


                  if (tileset_x < 0
                      || tileset_x > SHRT_MAX)
                    {
                      fatal ("MAPVIEW - render_map_layer - Tileset X is outside short bounds.");
                      return;
                    }

                  if (map->data_layers[layer][layer_offset] 
                      != 0)
                    draw_image_direct (tileset,
                                       (short) tileset_x, 0,
                                       (short) screen_x,
                                       (short) screen_y,
                                       TILE_W, TILE_H);

                  mapview->dirty_tiles[layer_offset]--;
                }
            }
        }
    }
}


void
render_map_objects (struct map_view *mapview, unsigned char layer)
{
  layer_t tag;
  tag = get_tag (mapview->map, layer);

  if (tag > 0)
    {
      struct object_rnode *next;
      struct object_image *image;

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


void
scroll_map (struct map_view *mapview,
            short x_offset,
            short y_offset)
{

  /* Sanity checking. */

  if (mapview == NULL)
    {
      error ("MAPVIEW - scroll_map - Map view is NULL.");
      return;
    }

  /* Work out the dirty rectangles to mark. */

  /* West scroll. */

  if (x_offset < 0)
    {
       mark_dirty_rect (mapview, 
                        mapview->x_offset,
                        mapview->y_offset,
                        -(x_offset), SCREEN_H);     
    }

  /* East scroll. */

  else if (x_offset > 0)
    {
      mark_dirty_rect (mapview, 
                       SCREEN_W + mapview->x_offset - x_offset,
                       mapview->y_offset,
                       x_offset, SCREEN_H);
    }


  /* North scroll. */

  if (y_offset < 0) 
    {
      mark_dirty_rect (mapview, 
                       mapview->x_offset,
                       mapview->y_offset,
                       SCREEN_W, -(y_offset)); 
    }

  /* South scroll. */

  else if (y_offset > 0)
    {
      mark_dirty_rect (mapview, 
                       mapview->x_offset,
                       SCREEN_H + mapview->y_offset - y_offset,
                       SCREEN_W, y_offset);
    }

  mapview->x_offset += x_offset;
  mapview->y_offset += y_offset;

  scroll_screen (-(x_offset), -(y_offset));
  render_map (mapview);
}


int
mark_dirty_rect (struct map_view *mapview,
                 int start_x,
                 int start_y,
                 int width,
                 int height)
{
  int x, y;
  struct dirty_rectangle *next;

  /* Sanity checking. */

  if (mapview == NULL)
    {
      fprintf (stderr, "MAPVIEW: Error: Rect dirtying passed NULL mapview.\n");
      return FAILURE;
    }

  if (width <= 0 || height <= 0)
    {
      fprintf (stderr, "MAPVIEW: Error: Rect dirtying passed insane W/H.\n");
      return FAILURE;
    }

  /* If this is the first rectangle to go onto the queue, then this
     function call will take the responsibility of managing the
     queue. Otherwise, just push this rectangle onto the queue for the
     parent call to handle. */

  if (mapview->dirty_rectangles != NULL)
    return add_dirty_rect (mapview, start_x, start_y, width, height);

  /* Otherwise, try adding and then process the queue. */

  if (add_dirty_rect (mapview, start_x, start_y, width, height) 
      == FAILURE)
    return FAILURE;

  while (mapview->dirty_rectangles != NULL)
    {
      /* Mark dirty tiles. */

      int xmax, ymax;

      /* Make sure maxima are rounded up. */

      xmax = ((mapview->dirty_rectangles->start_x
               + mapview->dirty_rectangles->width) / TILE_W);

      if ((mapview->dirty_rectangles->start_x
            + mapview->dirty_rectangles->width) % TILE_W > 0)
        xmax++;

      ymax = ((mapview->dirty_rectangles->start_y
               + mapview->dirty_rectangles->height) / TILE_H);

      if ((mapview->dirty_rectangles->start_y
            + mapview->dirty_rectangles->height) % TILE_H > 0)
        ymax++;

      for (x = mapview->dirty_rectangles->start_x / TILE_W;
           x < xmax; x++)
        {
          for (y = mapview->dirty_rectangles->start_y / TILE_H;
               y < ymax; y++)
            {
              if (x >= 0
                  && y >= 0
                  && x < mapview->map->width
                  && y < mapview->map->height)
                mapview->dirty_tiles[x + (y * mapview->map->width)] = \
                  mapview->map->num_layers;
            }
        }

      /* Mark dirty objects. */

      /* Transform the dirty rectangle so it extends to all tiles it
         intersects. */

      x = mapview->dirty_rectangles->start_x / TILE_W;
      y = mapview->dirty_rectangles->start_y / TILE_H;

      mapview->dirty_rectangles->start_x = x * TILE_W;
      mapview->dirty_rectangles->start_y = y * TILE_H;

      mapview->dirty_rectangles->width = (xmax - x + 1) * TILE_W;
      mapview->dirty_rectangles->height = (ymax - y + 1) * TILE_H;

      /* Check to see if each object in the hash table is going to be dirtied. */

      apply_to_objects (dirty_object_test, 
                        mapview->dirty_rectangles);

      /* Now can the current dirty rectangle and move to the next, if
         any. */

      next = mapview->dirty_rectangles->next;

      free (mapview->dirty_rectangles);

      mapview->dirty_rectangles = next;
    }
 
  return SUCCESS;
}


static int
add_dirty_rect (struct map_view *mapview,
                int start_x,
                int start_y,
                int width,
                int height)
{
  struct dirty_rectangle *rect;
  struct dirty_rectangle *p;

  /* Sanity checking shouldn't be needed - this should never be called
     directly. */

  /* First, try to allocate a rect structure. */

  rect = malloc (sizeof (struct dirty_rectangle));

  if (rect == NULL)
    {
      fprintf (stderr, "MAPVIEW: Error: Cannot alloc a dirty rectangle.\n");
      return FAILURE;
    }

  /* Now move the members in. */

  rect->start_x = start_x;
  rect->start_y = start_y;
  rect->width = width;
  rect->height = height;
  rect->parent = mapview;
  rect->next = NULL;

  /* If the queue is empty, then replace the queue pointer with the
     new rect pointer. */

  if (mapview->dirty_rectangles == NULL)
    {
      mapview->dirty_rectangles = rect;
      return SUCCESS;
    }

  /* Otherwise, skip to the first item in the queue with a NULL next
     pointer and add the new rect pointer there. */

  for (p = mapview->dirty_rectangles; 
       p->next != NULL; 
       p = p->next)
    ;

  p->next = rect;

  return SUCCESS;
}


void
cleanup_mapview (struct map_view *mapview)
{
  if (mapview)
    {
      if (mapview->dirty_tiles)
        free (mapview->dirty_tiles);

      if (mapview->object_queue)
        {
          unsigned int i;
          struct object_rnode *next;

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
          struct dirty_rectangle *next;

          while (mapview->dirty_rectangles != NULL)
            {
              next = mapview->dirty_rectangles->next;
              free (mapview->dirty_rectangles);
              mapview->dirty_rectangles = next;
            }
        }

      free (mapview);
    }
}
