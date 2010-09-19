#ifndef __GRAPHICS_H__
#define __GRAPHICS_H__

enum
  {
    SCREEN_W = 640,
    SCREEN_H = 480,
    SCREEN_D = 32
  };

int
init_graphics (void);

void
draw_image (const char filename[], 
            unsigned int image_x,
            unsigned int image_y, 
            unsigned int screen_x, 
            unsigned int screen_y,
            unsigned int width, 
            unsigned int height);

void
update_screen (void);

void
cleanup_graphics (void);

#endif /* __GRAPHICS_H__ */
