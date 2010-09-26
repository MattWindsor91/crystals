#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "graphics.h"
#include "util.h"
#include "module.h"

static struct image_t *sg_images[HASH_VALS];

int
init_graphics (void)
{
  unsigned int i;

  if (load_module_gfx ("gfx-sdl", &g_modules) == FAILURE)
    {
      fprintf (stderr, "ERROR: Could not load graphics module.\n");
      return FAILURE;
    }

  if ((*g_modules.gfx.init_screen) (SCREEN_W, SCREEN_H, SCREEN_D) == FAILURE)
    {
      fprintf (stderr, "ERROR: Could not init screen.!\n");
      return FAILURE;
    }
 
  for (i = 0; i < HASH_VALS; i++)
    sg_images[i] = NULL;

  return SUCCESS;
}

void
fill_screen (unsigned char red, 
             unsigned char green, 
             unsigned char blue)
{
  (*g_modules.gfx.draw_rect) (0, 0, SCREEN_W, SCREEN_H, red, green, blue);
}

struct image_t *
load_image (const char filename[])
{
  struct image_t *image;
  struct image_t *result;

  /* Check to see if the filename is sane. */

  if (filename == NULL)
    {
      fprintf (stderr, "GFX: Error: Filename is NULL.\n");
      return NULL;
    }

  /* If so, first try to allocate space for the image. */

  image = malloc (sizeof (struct image_t));

  if (image == NULL)
    {
      fprintf (stderr, "GFX: Error: Allocation failed for %s.\n", 
               filename);
      return NULL;
    }

  /* Try to copy the filename over. */

  image->filename = malloc (sizeof (char) * (strlen (filename) + 1));

  if (image->filename == NULL)
    {
      fprintf (stderr, "GFX: Error: FN allocation failed for %s.\n", 
               filename);
      free_image (image);
      return NULL;
    }

  strncpy (image->filename, filename, sizeof (char) * strlen (filename) + 1);

  /* Finally, try to load data. */

  image->data = (*g_modules.gfx.load_image_data) (filename);

  if (image->data == NULL)
    {
      fprintf (stderr, "GFX: Error: Data load failed for %s.\n", 
               filename);
      free_image (image);
      return NULL;
    }

  /* Try to store the image. */

  result = get_image (filename, image);

  if (result == NULL)
    {
      fprintf (stderr, "GFX: Error: Store failed for %s.\n", 
               filename);
      free_image (image);
      return NULL;
    }

  return result;
}

void
free_image (struct image_t *image)
{
  if (image)
    {
      if (image->filename)
        free (image->filename);

      if (image->data)
        (*g_modules.gfx.free_image_data) (image->data);

      free (image);
    }
}


int
draw_image (const char filename[],
            short image_x,
            short image_y,
            short screen_x,
            short screen_y,
            unsigned short width,
            unsigned short height)
{
  struct image_t *img;

  img = get_image (filename, NULL);

  /* Image not preloaded - try to load it now. */

  if (img == NULL)
    {
      img = load_image (filename);

      if (img == NULL)
        {
          fprintf (stderr, "GFX: Cannot load non-preloaded image.");
          return 0;
        }
    }

  return (*g_modules.gfx.draw_image) (img->data,
                                      image_x,
                                      image_y,
                                      screen_x,
                                      screen_y,
                                      width,
                                      height);
}

int
delete_image (const char filename[])
{
  int h;
  struct image_t *image, *prev;

  h = ascii_hash (filename);
  prev = NULL;

  /* Iterate through the hash bucket to find the correct image, then 
     delete its data and node. */
  for (image = sg_images[h]; image != NULL; image = prev->next)
    {
      if (strcmp (filename, image->filename) == 0)
        {
          if (prev == NULL)
            sg_images[h] = image->next;
          else
            prev->next = image->next;
          
          free_image (image);
          return 1;
      }
    }
  return 0;
}

/* Delete all images. */
void
clear_images (void)
{
  int i;
  struct image_t *p, *next;
  
  for (i = 0; i < HASH_VALS; i++)
    {
      for (p = sg_images[i]; p != NULL; p = next)
        {
          next = p->next;
          /* Delete the image data and node */
          free_image (p);
        }
      sg_images[i] = NULL;
  }
}

struct image_t *
get_image (const char filename[], struct image_t *add_pointer)
{
  int h; 
  struct image_t *image;

  /* Get the hash of the image's filename so we can search in the correct 
     bucket. */
  h = ascii_hash (filename);

  /* Now try to find the image. */
  for (image = sg_images[h]; image != NULL; image = image->next)
    {
      if (strcmp (filename, image->filename) == 0)
        {
          /* If there is a pointer to add, then replace the found image's 
             data with the new pointer's data. */

          if (add_pointer != NULL)
            {
              (*g_modules.gfx.free_image_data) (image->data);
              image->data = add_pointer->data;

              free_image (add_pointer);
            }

          return image;
        }
    }

  /* If we are given a pointer to add, and the image doesn't already 
     exist, then add the image to the start of the linked list. */
  if (add_pointer)
    {
      add_pointer->next = sg_images[h];
      sg_images[h] = add_pointer;
      return sg_images[h];
    }

  /* Return NULL, if all else fails. */
  return NULL;
}

void
cleanup_graphics (void)
{
  clear_images ();
}

