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

/** @file    mapview.c
 *  @author  Matt Windsor
 *  @brief   Map rendering functions.
 */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "mapview.h"
#include "map.h"
#include "util.h"
#include "module.h"
#include "graphics.h"

#include "object.h"

struct map_view *
init_mapview (struct map *map)
{
  struct map_view *mapview;

  unsigned int width, height; /* Temporary locations for shorthand purposes. */
  unsigned int i;

  if (map == NULL)
    {
      fprintf (stderr, "MAPVIEW: Error: Passed map pointer is null.\n");
      return NULL;
    }

  mapview = malloc (sizeof (struct map_view));

  if (mapview == NULL)
    {
      fprintf (stderr, "MAPVIEW: Error: Couldn't allocate map view.\n");
      return NULL;
    }
            
  mapview->x_offset = 0;
  mapview->y_offset = 0;
  mapview->map = map;
  mapview->object_queue = NULL;
  
  width = mapview->map->width;
  height = mapview->map->height;

  mapview->dirty_tiles = malloc (sizeof (unsigned char) * (width * height));

  if (mapview->dirty_tiles == NULL)
    {
      fprintf (stderr, "MAPVIEW: Error: Couldn't allocate dirty tilemap. \n");
      cleanup_mapview (mapview);
      return NULL;
    }

  /* Get the number of object queues to reserve, by finding the
     highest tag number in the map. */

  mapview->num_object_queues = get_max_tag (mapview->map);
  

  /* There should be at least one tag! */

  if (mapview->num_object_queues == 0)
    {
      fprintf (stderr, "MAPVIEW: Error: No tags in map; please fix map.\n");
      cleanup_mapview (mapview);
      return NULL;
    }

  /* Now allocate the queues. */

  mapview->object_queue = malloc (sizeof (struct object_image*)
                                  * mapview->num_object_queues);

  if (mapview->object_queue == NULL)
    {
      fprintf (stderr, "MAPVIEW: Error: Couldn't allocate object queues.");
      cleanup_mapview (mapview);
      return NULL;
    }

  for (i = 0; i < mapview->num_object_queues; i++)
    mapview->object_queue[i] = NULL;

  /* Set all tiles as dirty. */

  mark_dirty_rect (mapview, 0, 0, map->width, map->height);

  /*add_object_image (mapview, 1, "testobj.png", 0, 0, 60, 60, 16, 48);
  add_object_image (mapview, 1, "testobj.png", 0, 0, 70, 70, 16, 48);

  add_object_image (mapview, 1, "testobj.png", 16, 0, 100, 100, 16, 48);*/
  return mapview;
}

int
init_object_image (struct object_image *image, struct object_t *parent)
{
  /* Sanity checking. */

  if (image == NULL)
    {
      fprintf (stderr, "MAPVIEW: Error: Tried to init null object image.\n");
      return FAILURE;
    }

  if (parent == NULL)
    {
      fprintf (stderr, "MAPVIEW: Error: Object image has no parent.\n");
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
                  struct object_image *image)
{
  struct object_image *new_image;

  /* First, error checking! */

  /* We definitely need a mapview. */

  if (mapview == NULL)
    {
      fprintf (stderr, "MAPVIEW: Error: Tried to render to NULL mapview.\n");
      return FAILURE;
    }

  /* Also, the image should exist. */

  if (image == NULL)
    {
      fprintf (stderr, "MAPVIEW: Error: Tried to render a NULL rnode.\n");
      return FAILURE;
    }

  /* Assert that a non-NULL mapview must have a non-NULL render queue
     array, due to the nature of the mapview init. */

  /* Can't render to tag 0 - it's the standard "don't render" tag
     index. */

  if (tag == 0)
    {
      fprintf (stderr, "MAPVIEW: Error: Tried to render to tag 0.\n");
      return FAILURE;
    }

  /* Equally, can't render to a tag that's above the highest tag
     number in the map viewpoint. 

     NOTE: We don't check to ensure that our tag is actually being
     provided, just that it's a sane number. Possible FIXME in the
     future? */

  if (tag > mapview->num_object_queues)
    {
      fprintf (stderr, "MAPVIEW: Error: Tag specified too high.\n");
      return FAILURE;
    }

  if (image->filename == NULL)
    {
      fprintf (stderr, "MAPVIEW: Error: Filename is NULL.\n");
      return FAILURE;
    }

  if (image->width == 0 || image->height == 0)
    {
      fprintf (stderr, "MAPVIEW: Error: Zero object render width/height.\n");
      return FAILURE;
    }

  /* Now copy the image, if possible. */

  new_image = malloc (sizeof (struct object_image));

  if (new_image == NULL)
    {
      fprintf (stderr, "MAPVIEW: Error: Allocation failed for dupe rnode.\n");
      return FAILURE;
    }

  /* Blindly copy the existing stuff, then allocate new space for the
     filename and copy that over. */

  memcpy (new_image, image, sizeof (struct object_image));

  new_image->filename = malloc (sizeof (char)
                                * (strlen (image->filename) + 1));

  if (new_image->filename == NULL)
    {
      fprintf (stderr, "MAPVIEW: Error: Couldn't alloc new image FN.");
      free_object_image (new_image);
      return FAILURE;
    }

  strncpy (new_image->filename, image->filename,
           (strlen (image->filename) + 1));

  /* Finally, add to the proper tag queue.

     If the queue is empty, then just add it to the queue head.
     Otherwise, find a space in the queue before the first item with a
     screen_y higher (further down the screen) than this object, to
     preserve z-order.
  */
  
  if (mapview->object_queue[tag - 1] == NULL)
    {
      mapview->object_queue[tag - 1] = new_image;
      return SUCCESS;
    }
  else
    {
      struct object_image *ptr;
      
      /* Skip to the first item that either has a null neighbour 
         or a neighbour whose screen_y + width is bigger. */

      for (ptr = mapview->object_queue[tag - 1]; 
           ptr->next != NULL
             && ((ptr->next->map_y + ptr->next->height)
                 <= (new_image->map_y + new_image->height)); 
           ptr = ptr->next)
        ;

      /* Insert the new image here. */

      new_image->next = ptr->next;
      ptr->next = new_image;

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
  /* FIXME: only render dirty tiles. */

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
                  && mapview->dirty_tiles[true_x + (true_y * map->height)] != 0)
                {
                  if (map->data_layers[layer][true_x + (true_y * map->height)] 
                      != 0)
                    draw_image (FN_TILESET,
                                map->data_layers[layer][true_x 
                                                        + (true_y * map->height)]
                                * TILE_W, 0,
                                (x * TILE_W) - (short) (x_offset % TILE_W),
                                (y * TILE_H) - (short) (y_offset % TILE_H),
                                TILE_W, TILE_H);

                  mapview->dirty_tiles[true_x + (true_y * map->height)]--;
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
      struct object_image *next;

      while (mapview->object_queue[tag - 1] != NULL)
        {
          next = mapview->object_queue[tag - 1]->next;

          draw_image (mapview->object_queue[tag - 1]->filename,
                      mapview->object_queue[tag - 1]->image_x,
                      mapview->object_queue[tag - 1]->image_y,
                      (short) (mapview->object_queue[tag - 1]->map_x 
                       - mapview->x_offset),
                      (short) (mapview->object_queue[tag - 1]->map_y
                       - mapview->y_offset),
                      mapview->object_queue[tag - 1]->width,
                      mapview->object_queue[tag - 1]->height);

          /* Mark the parent object as no longer dirty. */
          if (mapview->object_queue[tag - 1]->parent != NULL)
            mapview->object_queue[tag - 1]->parent->is_dirty = FALSE;

          free_object_image (mapview->object_queue[tag - 1]);

          mapview->object_queue[tag - 1] = next;
        }
    } 
}

void
scroll_map (struct map_view *mapview, int direction)
{
  int adirection = -1;

  /* FIXME: Render damage. */
  switch (direction)
    {
    case NORTH:
      mapview->y_offset -= 1;
      mark_dirty_rect (mapview, 
                       mapview->x_offset / TILE_W,
                       mapview->y_offset / TILE_H,
                       SCREEN_W / TILE_W, 1);      
      adirection = SOUTH;
      break;
    case EAST:
      mapview->x_offset += 1;
      mark_dirty_rect (mapview, 
                       (SCREEN_W / TILE_W) + (mapview->x_offset / TILE_W) - 1,
                       mapview->y_offset / TILE_H,
                       1, SCREEN_H / TILE_H);
      adirection = WEST;
      break;
    case SOUTH:
      mapview->y_offset += 1;
      mark_dirty_rect (mapview, 
                       mapview->x_offset / TILE_W,
                       (SCREEN_H / TILE_H) + (mapview->y_offset / TILE_H) - 1,
                       SCREEN_W / TILE_W, 1);
      adirection = NORTH;
      break;
    case WEST:
      mapview->x_offset -= 1;
      mark_dirty_rect (mapview, 
                       mapview->x_offset / TILE_W,
                       mapview->y_offset / TILE_H,
                       1, SCREEN_H / TILE_H);
      adirection = EAST;
      break;
    }


  (*g_modules.gfx.scroll_screen) (adirection);
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

  /* Mark dirty tiles. */

  for (x = start_x; x < start_x + width; x++)
    {
      for (y = start_y; y < start_y + height; y++)
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

  /* Check to see if each object in the hash table is going to be dirtied. */

  apply_to_hash_objects (g_objects, 
                         dirty_object_test, 
                         mapview, 
                         start_x,
                         start_y,
                         width,
                         height);
 
  return SUCCESS;
}

void
cleanup_mapview (struct map_view *mapview)
{
  if (mapview)
    {
      if (mapview->dirty_tiles)
        free (mapview->dirty_tiles);

      /* Assert that the object queues will be empty, as the game will
         only gracefully terminate after processing them and before
         populating them. */

      if (mapview->object_queue)
        free (mapview->object_queue);
     

      free (mapview);
    }
}
