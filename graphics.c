#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "main.h"
#include "graphics.h"
#include "module.h"

static struct GfxImageNode *sg_images[GFX_HASH_VALS];

int
init_graphics (void)
{
  load_module_gfx ();

  if ((*g_modules.gfx.init_screen) (SCREEN_W, SCREEN_H, SCREEN_D))
    {
      unsigned int i;

      for (i = 0; i < GFX_HASH_VALS; i++)
        sg_images[i] = NULL;

      return SUCCESS;
    } 
  else 
    {
      fprintf (stderr, "ERROR: Could not init gfx!\n");
    }

  cleanup_graphics ();
  return FAILURE;
}

void
fill_screen (unsigned char red, 
             unsigned char green, 
             unsigned char blue)
{
  (*g_modules.gfx.draw_rect) (0, 0, SCREEN_W, SCREEN_H, red, green, blue);
}

struct GfxImageNode *
load_image (const char filename[])
{
  struct GfxImageNode *ptr;

  ptr = malloc (sizeof (struct GfxImageNode));

  if (ptr)
    {
      /* Load data. */
      strncpy (ptr->name, filename, GFX_HASH_NAME_LEN);
      ptr->data = (*g_modules.gfx.load_image_data) (filename);

      if (ptr->data) {
        /* Store the image. */
        return get_image (filename, ptr);
      } else {
        fprintf (stderr, "GFX: Could not load data for image %s", filename);
        return NULL;
      }
    }
  else
    {
      fprintf (stderr, "GFX: Could not allocate space for image %s", 
               filename);
      return NULL;
    }
}

void
free_image (struct GfxImageNode *node)
{
  (*g_modules.gfx.free_image_data) (node->data);
  free (node);
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
  struct GfxImageNode *img;

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
ascii_hash (const char string[])
{
  unsigned char *p;
  int h;

  h = 0;

  /* For each character in the string, multiply the current hash value by the 
   * hash multiplier and then add the value of the next character.
   *
   *  (derived from Kernighan and Pike, ``The Practice of Programming'')
   */

  for (p = (unsigned char*) string; *p != '\0'; p++)
    h = (GFX_HASH_MUL * h) + *p;

  /* Return the modulus so that the value is in between 0 and the hash 
     value upper bound. */
  return h % GFX_HASH_VALS;
}

int
delete_image (const char name[])
{
  int h;
  struct GfxImageNode *img, *prev;

  h = ascii_hash (name);
  prev = NULL;

  /* Iterate through the hash bucket to find the correct image, then 
     delete its data and node. */
  for (img = sg_images[h]; img != NULL; img = prev->next)
    {
      if (strcmp(name, img->name) == 0)
        {
          if (prev == NULL)
            sg_images[h] = img->next;
          else
            prev->next = img->next;
          
          free_image (img);
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
  struct GfxImageNode *p, *next;
  
  for (i = 0; i < GFX_HASH_VALS; i++)
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

struct GfxImageNode *
get_image (const char name[], struct GfxImageNode *add_pointer)
{
  int h; 
  struct GfxImageNode *img;

  /* Get the hash of the image's filename so we can search in the correct 
     bucket. */
  h = ascii_hash (name);

  /* Now try to find the image. */
  for (img = sg_images[h]; img != NULL; img = img->next)
    {
      if (strcmp (name, img->name) == 0)
        {
          /* If there is a pointer to add, then replace the found image's 
             data with the new pointer's data. */

          if (add_pointer)
            {
              free_image (img->data);
              img->data = add_pointer->data;
            }

          return img;
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
  ;
}

