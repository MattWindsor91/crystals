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

/** @file    src/field/mapview.h
 *  @author  Matt Windsor
 *  @brief   Prototypes and declarations for rendering functions.
 *
 *  The map renderer makes a distinction between the map data itself 
 *  and the additional data set tracking the map viewpoint.  Whereas 
 *  functions and structures related to the former are available in 
 *  map.c and map.h, the map viewpoint system is described in this 
 *  and mapview.c.
 *
 *  @todo FIXME: Remove recursion in dirty tile rectangle code.
 */

#ifndef _MAPVIEW_H
#define _MAPVIEW_H


#include "map.h"
#include "../types.h"

/* -- CONSTANTS -- */

/** Width of one tile, in pixels.
 *
 *  @todo FIXME: read this from tileset data.
 */

extern const uint16_t TILE_W;


/** Height of one tile, in pixels.
 *
 *  @todo FIXME: read this from tileset data.
 */

extern const uint16_t TILE_H;


/* -- STRUCTURES -- */

/** An object image. */

typedef struct object_image
{
  char *filename;        /**< Name of the image, used when looking up
                            the image in the image cache. */

  int16_t image_x;       /**< X co-ordinate of the left edge of the
                            on-image rectangle from which to source
                            the rendered image, in pixels. */

  int16_t image_y;       /**< Y co-ordinate of the top edge of the
                            on-image rectangle from which to source
                            the rendered image, in pixels. */

  int32_t map_x;         /**< X co-ordinate of the left edge of the
                            on-map rectangle in which to render the
                            image, in pixels. */

  int32_t map_y;         /**< Y co-ordinate of the top edge of the
                            on-map rectangle in which to render the
                            image, in pixels. */

  unsigned short width;  /**< Width of the object image, in pixels. */

  unsigned short height; /**< Height of the object image, in
                            pixels. */

  struct object *parent; /**< Pointer to the object parent, if any. */

  struct object_image *next; /**< Next node in the queue. */
} object_image_t;


/** An object render queue node. */

typedef struct object_rnode
{
  struct object   *object;    /**< Pointer to the object. */
  struct object_image *image; /**< Pointer to the object image linked
                                 to the object, for expedience. */
  struct object_rnode *next;  /**< Next node in the queue. */
} render_node_t;


/** A dirty rectangle queue node. */

typedef struct dirty_rectangle
{
  int start_x; /**< X co-ordinate of the left edge of the rectangle
                    (in pixels from left side of map). */

  int start_y; /**< Y co-ordinate of the top edge of the rectangle
                    (in pixels from top side of map). */

  int width;   /**< Width of the rectangle (in pixels). */
  int height;  /**< Height of the rectangle (in pixels). */

  struct map_view *parent;      /**< Parent map view. */
  struct dirty_rectangle *next; /**< Next node in the queue. */
} dirty_rectangle_t;


/** A map viewpoint.
 *
 *  This contains data required to render a map, including the offset
 *  of the current viewpoint, which tiles to render on the next
 *  render pass, and suchlike.
 */

typedef struct map_view
{
  int32_t x_offset;           /**< Offset of the left edge of the
                                 screen, in pixels from the left edge
                                 of the map.  Can be negative. */

  int32_t y_offset;           /**< Offset of the top edge of the
                                 screen, in pixels from the top edge
                                 of the map. Can be negative.*/

  map_t *map;                 /**< Pointer to the map being viewed. */

  layer_count_t *dirty_tiles; /**< Matrix of "dirty" tiles, or tiles
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

  render_node_t **object_queue; /**< The head array of queues of
                                         object pointers to be rendered
                                         on the next pass.  There will
                                         be num_object_queues heads in
                                         this block.*/
  
  dirty_rectangle_t *dirty_rectangles; /**< Stack of dirty
                                               rectangles. */
} mapview_t;


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

mapview_t *
init_mapview (map_t *map);


/** Set all the parameters of an object image node to default values.
 *
 *  @param image   Pointer to the object render queue node.
 *
 *  @param parent  Pointer to the parent object of the object image.
 *
 *  @return SUCCESS if there were no errors encountered; FAILURE
 *  otherwise.
 */

bool_t
init_object_image (object_image_t *image, struct object *parent);


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
 *  @param object    Pointer to the object whose image should be
 *                   rendered.
 *
 *  @return SUCCESS if there were no errors encountered; FAILURE
 *  otherwise.
 */

bool_t
add_object_image (mapview_t *mapview,
                  layer_value_t tag,
                  struct object *object);


/** Free an object image render queue node.
 *
 *  @param image  Pointer to the render node to free. 
 */

void
free_object_image (object_image_t *image);


/** Render the dirty tiles on a map.
 *
 *  @param mapview  Pointer to the map view to render.
 */

void
render_map (mapview_t *mapview);


/** Render a given layer on a map.
 *
 *  @param mapview  Pointer to the map view to render.
 *
 *  @param layer    The layer to render.
 */

void
render_map_layer (mapview_t *mapview, layer_index_t layer);


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
render_map_objects (mapview_t *mapview, layer_index_t layer);


/** Scroll the map on-screen, re-rendering it in its new position.
 *
 *  @param mapview   The map view to render.
 *
 *  @param x_offset  The X co-ordinate offset to scroll by, in pixels.
 *
 *  @param y_offset  The Y co-ordinate offset to scroll by, in pixels.
 */

void
scroll_map (mapview_t *mapview,
            int16_t x_offset,
            int16_t y_offset);


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
 *                  as a pixel offset from the left edge of the map.
 *
 *  @param start_y  The Y co-ordinate of the start of the rectangle, 
 *                  as a pixel offset from the top edge of the map.
 *
 *  @param width    Width of the tile rectangle, in pixels.
 *
 *  @param height   Height of the tile rectangle, in pixels.
 *
 *  @return SUCCESS if there were no errors encountered; FAILURE
 *  otherwise.
 */

bool_t
mark_dirty_rect (mapview_t *mapview,
                 int32_t start_x,
                 int32_t start_y,
                 int32_t width,
                 int32_t height);


/** De-initialise a mapview.
 *
 *  @warning This frees the mapview structure, but NOT the map
 *  connected to it.  Use free_map to remove the map.
 *
 *  @param mapview  Pointer to the map view to de-allocate.
 */

void
free_mapview (mapview_t *mapview);


#endif /* _MAPVIEW_H */
