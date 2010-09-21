#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "main.h"
#include "events.h"
#include "module.h"
#include "graphics.h"
#include "mapview.h"

static struct event_base *sg_event_base;
static unsigned char sg_held_special_keys[256];

/* Test callbacks, woo */

static event_callback *sg_skeyupcb;
static event_callback *sg_skeydowncb;
static event_callback *sg_quitcb;

void
on_quit (event_t *event)
{
  event = event;
  g_running = 0;
}

void
on_special_key_up (event_t *event)
{
  if (event->skey.code == SK_ESCAPE)
    on_quit (event);

  sg_held_special_keys[(int) event->skey.code] = 0;
}

void
on_special_key_down (event_t *event)
{
  sg_held_special_keys[(int) event->skey.code] = 1;
}

void
handle_held_keys (void)
{
  if (sg_held_special_keys[SK_UP])
    scroll_map (g_mapview, NORTH);

  if (sg_held_special_keys[SK_RIGHT])
    scroll_map (g_mapview, EAST);

  if (sg_held_special_keys[SK_DOWN])
    scroll_map (g_mapview, SOUTH);

  if (sg_held_special_keys[SK_LEFT])
    scroll_map (g_mapview, WEST);
}

int
init_events (void)
{
  if (load_module_event ("event-sdl") == FAILURE)
    {
      fprintf (stderr, "ERROR: Could not load events module.\n");
      return FAILURE;
    }

  (*g_modules.event.register_release_handle) (event_release);

  sg_event_base = malloc (sizeof (struct event_base));

  if (sg_event_base == NULL)
    {
      fprintf (stderr, "ERROR: Could not allocate events base. \n");
      return FAILURE;
    }

  sg_event_base->callbacks = NULL;
  memset (sg_held_special_keys, 0, sizeof (unsigned char) * 256);

  /* Test callbacks, woo */

  if (init_callbacks () == FAILURE)
    {
      fprintf (stderr, "ERROR: Could not install event callbacks.\n");
    }

  return SUCCESS;
}

int
init_callbacks (void)
{
  sg_skeyupcb = install_callback (on_special_key_up, SPECIAL_KEY_UP_EVENT);
  sg_skeydowncb = install_callback (on_special_key_down, SPECIAL_KEY_DOWN_EVENT);
  sg_quitcb = install_callback (on_quit, QUIT_EVENT);

  if (sg_skeyupcb
      && sg_skeydowncb
      && sg_quitcb)
    return SUCCESS;
  else
    {
      /* Clean up callbacks if any failed. */
      cleanup_callbacks ();
      return FAILURE;
    }
}

void
cleanup_callbacks (void)
{ 
 if (sg_skeyupcb)
   unload_callback (sg_skeyupcb);

 if (sg_skeydowncb)
   unload_callback (sg_skeydowncb);

 if (sg_quitcb)
   unload_callback (sg_quitcb);

 sg_skeyupcb = sg_skeydowncb = sg_quitcb = NULL;
}

void cleanup_events (void)
{
  if (sg_event_base) {
    cleanup_callbacks ();
    free (sg_event_base);
    sg_event_base = NULL;
  }
}


struct event_callback *
install_callback (void (*callback) (event_t *event), int types)
{   
  struct event_callback *pnew, *p;
  
  pnew = malloc (sizeof (struct event_callback));

  if (pnew)
    {
      pnew->callback = callback;
      pnew->types = types;
      pnew->next = NULL;

      /* Link to list */
      if (sg_event_base->callbacks == NULL)
        sg_event_base->callbacks = pnew;
      else
        {
          for (p = sg_event_base->callbacks; p->next != NULL; p = p->next)
            ; /* Do nothing until p->next is NULL. */

          p->next = pnew;
      
        }
    }

  return pnew;
}

int
unload_callback (struct event_callback *ptr)
{
  struct event_callback *p;

  if (ptr)
    {
      /* Corner-case for if ptr is the first callback. */
      if (sg_event_base->callbacks == ptr)
        {
          sg_event_base->callbacks = ptr->next;
          free (ptr);
          return SUCCESS;
    }

    for (p = sg_event_base->callbacks; p->next != NULL; p = p->next) {
      if (p->next == ptr) {
        /* Now we've found the list node before this one, replace its 
           next pointer with the to-be-unloaded node's next pointer. */
        p->next = ptr->next;

        /* Now delete the callback. */
        free(ptr);
        return SUCCESS;
      }
    }
  }

  return FAILURE;
}

void
event_release (event_t *event)
{
  struct event_callback *p;

  for (p = sg_event_base->callbacks; p != NULL; p = p->next)
    {
      /* Trigger all callbacks with the relevant type. */
      if (p->types & event->type)
        p->callback (event);
    }
}
