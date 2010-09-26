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

/** @file    mapview.h
 *  @author  Matt Windsor
 *  @brief   Prototypes and declarations for rendering functions.
 *
 *  The map renderer makes a distinction between the map data itself 
 *  and the additional data set tracking the map viewpoint.  Whereas 
 *  functions and structures related to the former are available in 
 *  map.c and map.h, the map viewpoint system is described in this 
 *  and mapview.c.
 */

#ifndef _MAPVIEW_H
#define _MAPVIEW_H

#include "map.h"

/* -- STRUCTURES -- */

/** An object render queue node. */

struct object_image
{
  char *filename;        /**< Name of the image, used when looking up
                            the image in the image cache. */

  short image_x;         /**< X co-ordinate of the left edge of the
                            on-image rectangle from which to source
                            the rendered image, in pixels. */

  short image_y;         /**< Y co-ordinate of the top edge of the
                            on-image rectangle from which to source
                            the rendered image, in pixels. */

  unsigned int map_x;    /**< X co-ordinate of the left edge of the
                            on-map rectangle in which to render the
                            image, in pixels. */

  unsigned int map_y;    /**< Y co-ordinate of the top edge of the
                            on-map rectangle in which to render the
                            image, in pixels. */

  unsigned short width;  /**< Width of the object image, in pixels. */

  unsigned short height; /**< Height of the object image, in
                            pixels. */

  struct object_t *parent; /**< Pointer to the object parent, if any. */

  struct object_image *next; /**< Next node in the queue. */
};

/** A map viewpoint.
 *
 *  This contains data required to render a map, including the offset
 *  of the current viewpoint, which tiles to render on the next
 *  render pass, and suchlike.
 */

struct map_view
{
  int x_offset;               /**< Offset of the left edge of the
                                 screen, in pixels from the left edge
                                 of the map.  Can be negative. */

  int y_offset;               /**< Offset of the top edge of the
                                 screen, in pixels from the top edge
                                 of the map. Can be negative.*/

  struct map *map;            /**< Pointer to the map being viewed. */

  unsigned char *dirty_tiles; /**< Matrix of "dirty" tiles, or tiles
                                 to be re-rendered on the next
                                 rendering pass. 

                                 @note  To set a tile as dirty, index
                                 [tile x + (tile y * map width)
                                 should be set to the number of
                                 layers in the map.  This is because
                                 of the way the render code works.
                                 Eventually there will be a function
                                 for setting tiles to dirty without
                                 needing to pay attention to this. */

  unsigned int num_object_queues; /**< Number of object queues reserved
                                     (equal to the highest tag used
                                     by the map). */

  struct object_image **object_queue; /**< The head array of queues of
                                         object sprites to be rendered
                                         on the next pass.  There will
                                         be num_object_queues heads in
                                         this block.*/
};

#include "object.h" /* object_t; cannot be included before
                       structures. */

/* -- GLOBAL VARIABLES -- */

extern const char FN_TILESET[]; /**< Tileset filename. */

/* -- PROTOTYPES -- */

/** Initialise a map view.
 *
 *  @param map  Pointer to the map to associate with the map view.
 *
 *  @return  a pointer to the map view, or NULL for allocation
 *  failure.
 */

struct map_view *
init_mapview (struct map *map);

/** Set all the parameters of an object image node to default values.
 *
 *  @param image   Pointer to the object render queue node.
 *
 *  @param parent  Pointer to the parent object of the object image.
 *
 *  @return SUCCESS if there were no errors encountered; FAILURE
 *  otherwise.
 */

int
init_object_image (struct object_image *image, struct object_t *parent);

/** Add an object sprite to the rendering queue. 
 *
 *  This should be called from a higher-level function, as it does not
 *  take responsibility for making underlying tiles dirty, take into
 *  consideration the object's baseline or translate from map X and Y
 *  to screen X and Y.
 *
 *  @param mapview   Pointer to the map viewpoint to render on.
 *
 *  @param tag       Number of the layer tag to use. The object will
 *                   be rendered on top of the first map layer to use
 *                   the tag. This -must- be nonzero.
 *
 *  @param image     Pointer to the object render queue node to add to
 *                   the rendering queue.
 *
 *  @return SUCCESS if there were no errors encountered; FAILURE
 *  otherwise.
 */

int
add_object_image (struct map_view *mapview,
                  layer_t tag,
                  struct object_image *image);

/** Free an object image render queue node.
 *
 *  @param image  Pointer to the render node to free. 
 */

void
free_object_image (struct object_image *image);

/** Render the dirty tiles on a map.
 *
 *  @param mapview  Pointer to the map view to render.
 */

void
render_map (struct map_view *mapview);

/** Render a given layer on a map.
 *
 *  @param mapview  Pointer to the map view to render.
 *
 *  @param layer    The layer to render.
 */

void
render_map_layer (struct map_view *mapview, unsigned char layer);

/** Render any map objects to be placed on top of this layer.
 *
 *  This will, if this layer is the first defined with its tag, blit
 *  all dirty objects of the same tag on top of this map layer in
 *  z-order.
 *
 *  @param mapview  Pointer to the map view to render.
 *
 *  @param layer    The layer to render.
 */

void
render_map_objects (struct map_view *mapview, unsigned char layer);

/** Scroll the map on-screen, re-rendering it in its new position.
 *
 *  @param mapview    The map view to render.
 *
 *  @param direction  The cardinal direction (NORTH, SOUTH, EAST or
 *                    WEST) to scroll in.
 */

void
scroll_map (struct map_view *mapview, int direction);

/** Mark a rectangle of tiles as being dirty.
 *
 *  The X and Y tile offsets are absolute (that is, offset from 
 *  tile (0, 0) rather than the tile currently shown at the top-left 
 *  of the screen.
 *
 *  This will also render any objects whose images fall into the
 *  rectangle.
 *
 *  @param mapview  Pointer to the map view to render.
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

int
mark_dirty_rect (struct map_view *mapview,
                 long start_x,
                 long start_y,
                 unsigned int width,
                 unsigned int height);

/** De-initialise a mapview.
 *
 *  @warning This frees the mapview structure, but NOT the map
 *  connected to it.  Use cleanup_map to remove the map.
 *
 *  @param mapview  Pointer to the map view to de-allocate.
 */

void
cleanup_mapview (struct map_view *mapview);

#endif /* _MAPVIEW_H */
