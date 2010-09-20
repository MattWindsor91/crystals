#include <SDL.h>
#include <SDL/SDL_image.h>

#include "main.h"
#include "graphics.h"

static SDL_Surface *s_screen;
static struct GfxImageNode *s_images[GFX_HASH_VALS];

int
init_graphics (void)
{
  if (SDL_Init (SDL_INIT_VIDEO) == 0)
    {
      s_screen = SDL_SetVideoMode (SCREEN_W,
                                   SCREEN_H,
                                   SCREEN_D,
                                   SDL_SWSURFACE);

      if (s_screen)
        {
          unsigned int i;
          for (i = 0; i < GFX_HASH_VALS; i++)
            s_images[i] = NULL;
          return 1;
        }
      else
        {
          fprintf (stderr, "ERROR: Couldn't get screen!\n");
        }
    } 
  else 
    {
      fprintf (stderr, "ERROR: Could not init SDL!\n");
    }

  cleanup_graphics ();
  return 0;
}

void
fill_screen (unsigned char red, 
             unsigned char green, 
             unsigned char blue)
{
  SDL_FillRect (s_screen, NULL, SDL_MapRGB (s_screen->format, red, green, blue));
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
      ptr->data = load_image_data (filename); /* FIXME: Point to module function. */

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

void *
load_image_data (const char filename[])
{ 
  SDL_Surface *surf;

  surf = IMG_Load (filename);

  if (surf == NULL)
    fprintf (stderr, "GFX-SDL: Couldn't load %s!\n", filename);

  return (void*) surf;
}


void
free_image (struct GfxImageNode *node)
{
  free_image_data(node->data); /* FIXME: Point to module function. */
  free(node);
}

void
free_image_data (void *data)
{
  if (data) {
    SDL_FreeSurface(data);
    data = NULL;
  }
}

int
draw_image (const char filename[],
            int image_x,
            int image_y,
            int screen_x,
            int screen_y,
            unsigned int width,
            unsigned int height)
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

  return mod_draw_image (img,
                         image_x,
                         image_y,
                         screen_x,
                         screen_y,
                         width,
                         height); /* FIXME: Point to module function. */
}

int
mod_draw_image (struct GfxImageNode *image,
                int image_x,
                int image_y,
                int screen_x,
                int screen_y,
                unsigned int width,
                unsigned int height)
{
  SDL_Rect srcrect, destrect;
  SDL_Surface *ptex;

  ptex = (SDL_Surface*) image->data;

  srcrect.x = image_x;
  srcrect.y = image_y;

  destrect.x = screen_x;
  destrect.y = screen_y;

  srcrect.w = destrect.w = width;
  srcrect.h = destrect.h = height;

  if (ptex)
    {
      SDL_BlitSurface (ptex, &srcrect, s_screen, &destrect);
      return 1;
    }
  else
    return 0;
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
  for (img = s_images[h]; img != NULL; img = prev->next)
    {
      if (strcmp(name, img->name) == 0)
        {
          if (prev == NULL)
            s_images[h] = img->next;
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
      for (p = s_images[i]; p != NULL; p = next)
        {
          next = p->next;
          /* Delete the image data and node */
          free_image (p);
        }
      s_images[i] = NULL;
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
  for (img = s_images[h]; img != NULL; img = img->next)
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
      add_pointer->next = s_images[h];
      s_images[h] = add_pointer;
      return s_images[h];
    }

  /* Return NULL, if all else fails. */
  return NULL;
}

void
update_screen (void)
{
  SDL_Flip (s_screen);
  SDL_Delay (30);
}

void
scroll_screen (unsigned int direction)
{
  SDL_Rect source, dest;

  source.x = dest.x = source.y = dest.y = 0;

  source.w = dest.w = SCREEN_W;
  source.h = dest.h = SCREEN_H;

  switch (direction)
    {
    case NORTH:
      source.y = 1;
      source.h -= 1;
      break;
    case EAST:
      dest.x = 1;
      source.w -= 1;
      break;
    case SOUTH:
      dest.y = 1;
      source.h -= 1;
      break;
    case WEST:
      source.x = 1;
      source.w -= 1;
      break;
    }

  SDL_BlitSurface (s_screen, &source, s_screen, &dest);
}

void
cleanup_graphics (void)
{
  SDL_Quit ();
}

