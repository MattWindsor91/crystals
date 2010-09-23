#ifndef _GRAPHICS_H
#define _GRAPHICS_H

enum
  {
    SCREEN_W = 640, /**< Width of the screen (in pixels). 
                       @todo FIXME: Make this changeable at runtime. */

    SCREEN_H = 480, /**< Height of the screen (in pixels).
                       @todo FIXME: Make this changeable at runtime. */

    SCREEN_D = 32   /**< Colour depth of the screen (in bits per
                       pixel.) */
  };

/** An image node in the image hash table.
 *
 *  Since the contents of data will vary between graphics drivers, it
 *  is strongly recommended that nothing outside the graphics layer directly
 *  manipulates an ImageNode.
 *
 *  This will inevitably have to be changed!
 */

struct image_t
{
  char *filename;       /**< Filename of the image. */
  void *data;           /**< Driver-dependent image data. */
  struct image_t *next; /**< The next node, if any. */
};

int
init_graphics (void);

void
fill_screen (unsigned char red,
             unsigned char green,
             unsigned char blue);

struct image_t *
load_image (const char filename[]);

void
free_image (struct image_t *image);

int
draw_image (const char filename[],
            short image_x,
            short image_y,
            short screen_x,
            short screen_y,
            unsigned short width,
            unsigned short height);



int
delete_image (const char filename[]);

void
clear_images (void);

struct image_t *
get_image (const char filename[], struct image_t *add_pointer);

void
update_screen (void);

void
scroll_screen (unsigned int direction);

void
cleanup_graphics (void);

#endif /* _GRAPHICS_H */
