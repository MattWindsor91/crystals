#ifndef _GRAPHICS_H
#define _GRAPHICS_H

enum
  {
    SCREEN_W = 640,
    SCREEN_H = 480,
    SCREEN_D = 32
  };

int
init_graphics (void);

void
fill_screen (unsigned char red,
             unsigned char green,
             unsigned char blue);

void
draw_image (const char filename[], 
            int image_x,
            int image_y, 
            int screen_x, 
            int screen_y,
            unsigned int width, 
            unsigned int height);

void
update_screen (void);

void
scroll_screen (unsigned int direction);

void
cleanup_graphics (void);

#endif /* _GRAPHICS_H */
