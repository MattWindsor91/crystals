#ifndef _GRAPHICS_H
#define _GRAPHICS_H

enum
  {
    GFX_HASH_NAME_LEN = 100, /**< Maximum size of the part of the
                                image filename used by the hashing
                                function. If the filename (relative
                                to the image directory) is larger,
                                then an error will be tripped. */

    GFX_HASH_VALS = 10, /**< Number of hash values used in the image
                           hash table, and therefore the number of
                           separate linked lists. Increasing this
                           may increase performance. */

    GFX_HASH_MUL  = 31, /**< Hash value used in the image hash table
                           algorithms. This value is taken from
                           Kernighan and Pike's ``The Practice of
                           Programming'', as is the algorithmic
                           concept. */


    SCREEN_W = 640,
    SCREEN_H = 480,
    SCREEN_D = 32
  };

/** An image node in the image hash table.
 *
 *  Since the contents of data will vary between graphics drivers, it
 *  is strongly recommended that nothing outside the graphics layer directly
 *  manipulates an ImageNode.
 *
 *  This will inevitably have to be changed!
 */
struct GfxImageNode
{
  char name[GFX_HASH_NAME_LEN]; /**< Name used to identify the
                                      image. */
  void *data;                   /**< Driver-dependent image data. */
  struct GfxImageNode *next; /**< The next node, if any. */
};

int
init_graphics (void);

void
fill_screen (unsigned char red,
             unsigned char green,
             unsigned char blue);

struct GfxImageNode *
load_image (const char filename[]);

void *
load_image_data (const char filename[]);

void
free_image (struct GfxImageNode *node);

/* This will move to module */
void
free_image_data (void *data);

int
draw_image (const char filename[],
            int image_x,
            int image_y,
            int screen_x,
            int screen_y,
            unsigned int width,
            unsigned int height);

/* This will move to module */
int
mod_draw_image (struct GfxImageNode *image,
                int image_x,
                int image_y,
                int screen_x,
                int screen_y,
                unsigned int width,
                unsigned int height);

int
ascii_hash (const char string[]);

int
delete_image (const char name[]);

void
clear_images (void);

struct GfxImageNode *
get_image (const char name[], struct GfxImageNode *add_pointer);

void
update_screen (void);

void
scroll_screen (unsigned int direction);

void
cleanup_graphics (void);

#endif /* _GRAPHICS_H */
