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

/** @file    hash.c
 *  @author  Matt Windsor
 *  @brief   Generic hash table implementation.
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "hash.h"
#include "util.h"     /* SUCCESS and FAILURE. */
#include "graphics.h" /* free_image. */
#include "object.h"   /* free_object. */

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
    h = (HASH_MUL * h) + *p;

  /* Return the modulus so that the value is in between 0 and the hash 
     value upper bound. */
  return h % HASH_VALS;
}

/* Create a hash object with the given data and link it to the given
   hash table. */

struct hash_object *
create_hash_object (struct hash_object *head[], 
                    const char name[], 
                    unsigned char type, 
                    void *data)
{
  struct hash_object *hash_object;
  struct hash_object *result;

  /* Sanity-checking. */

  if (name == NULL)
    {
      fprintf (stderr, "HASH: Error: Given name is NULL.\n");
      return NULL;
    }

  if (head == NULL) 
    {
      fprintf (stderr, "HASH: Error: Given hash table for %s is NULL.\n",  
               name);
      return NULL;
    }

  if (data == NULL)
    {
      fprintf (stderr, "HASH: Error: Given data pointer for %s is NULL.\n", 
               name);
      return NULL;
    }

  /* Now try to allocate the container. */

  hash_object = malloc (sizeof (struct hash_object));

  if (hash_object == NULL)
    {
      fprintf (stderr, "HASH: Error: Could not allocate container for %s.\n", 
               name);
      return NULL;
    }

  /* Nullify stuff for now. */

  hash_object->type = DATA_NULL;
  hash_object->data = NULL;
  hash_object->next = NULL;

  /* Try to allocate and copy the name. */

  hash_object->name = malloc (sizeof (char) * (strlen (name) + 1));

  if (hash_object->name == NULL)
    {
      fprintf (stderr, "HASH: Error: Name allocation failed for %s.\n",
               name);
      free_hash_object (hash_object);
      return NULL;
    }

  strncpy (hash_object->name, name, sizeof (char) * strlen (name) + 1);

  /* Now that everything else is done, link the data to the object. */

  hash_object->type = type;
  hash_object->data = data;

  /* Try to store the object. */

  result = get_hash_object (head, name, hash_object);

  if (result == NULL)
    {
      /* Blank the pointers so the original data isn't deleted. */

      hash_object->type = DATA_NULL;
      hash_object->data = NULL;     

      fprintf (stderr, "HASH: Error: Could not add %s to hash table.\n", 
               name);

      free_hash_object (hash_object);
      return NULL;      
    }

  return result;
}

/* Delete a hash object and its associated data. */

void
free_hash_object (struct hash_object *object)
{
  if (object == NULL)
    return;

  /* If there is data attached to the object, try to figure out how to
     free it from the given type. */

  if (object->data)
    {
      switch (object->type)
        {
        case DATA_NULL:
        default:
          fprintf (stderr,
                   "HASH: Warn: Hash object %s has data and no type.\n", 
                   object->name);
          break;
        case DATA_IMAGE:
          free_image ((struct image_t *) object->data);
          break;
        case DATA_SOUND:
          /* Insert sound freeing code here. */
          break;
        case DATA_OBJECT:
          free_object ((struct object_t *) object->data);
          break;
        }
    }

  if (object->name)
    free (object->name);

  free (object);
}

/* Remove a hash object from the given hash table. */

int
delete_hash_object (struct hash_object *head[], 
                    const char name[])
{
  int h;
  struct hash_object *object, *prev;

  h = ascii_hash (name);
  prev = NULL;

  /* Iterate through the hash bucket to find the correct object, then 
     delete its data and node. */
  for (object = head[h]; object != NULL; object = prev->next)
    {
      if (strcmp (name, object->name) == 0)
        {
          if (prev == NULL)
            head[h] = object->next;
          else
            prev->next = object->next;
          
          free_hash_object (object);
          return SUCCESS;
      }
    }
  return FAILURE;
}

/* Delete all hash objects. */

void
clear_hash_objects (struct hash_object *head[])
{
  int i;
  struct hash_object *p, *next;
  
  for (i = 0; i < HASH_VALS; i++)
    {
      for (p = head[i]; p != NULL; p = next)
        {
          next = p->next;
          /* Delete the object data and node */
          free_hash_object (p);
        }
      head[i] = NULL;
  }
}

/* Retrieve an hash table node, or add one to a hash table. */

struct hash_object *
get_hash_object (struct hash_object *head[], 
                 const char name[],
                 struct hash_object *add_pointer)
{
  int h; 
  struct hash_object *object;

  /* Get the hash of the object's filename so we can search in the correct 
     bucket. */
  h = ascii_hash (name);

  /* Now try to find the object. */
  for (object = head[h]; object != NULL; object = object->next)
    {
      if (strcmp (name, object->name) == 0)
        {
          /* If there is a pointer to add, and there's already a node
             with this object name, then raise an error.  Names
             should be unique. */

          if (add_pointer != NULL)
            {
              fprintf (stderr, "HASH: Error: Duplicate object %s!\n", 
                       name);

              free_hash_object (add_pointer);
              return NULL;
            }

          return object;
        }
    }

  /* If we are given a pointer to add, and the object doesn't already 
     exist, then add the object to the start of the linked list. */
  if (add_pointer)
    {
      add_pointer->next = head[h];
      head[h] = add_pointer;

      return head[h];
    }

  /* Return NULL, if all else fails. */
  return NULL;
} 

/* Wrapper to get_hash_object for use in finding hash objects. */

struct hash_object *
find_hash_object (struct hash_object *head[], 
                  const char name[])
{
  return get_hash_object (head, name, NULL);
}

/* Apply the given function to all members of the hash table. */

int
apply_to_hash_objects (struct hash_object *head[], 
                       int (*function) (struct hash_object *object, 
                                        void *data),
                       void *data)
{
  int i;
  int result;
  int temp_result;
  struct hash_object *hash_object;

  result = SUCCESS;

  for (i = 0; i < HASH_VALS; i++)
    {
      for (hash_object = head[i];
           hash_object != NULL;
           hash_object = hash_object->next)
        {
          temp_result = function (hash_object, data);

          if (temp_result == FAILURE)
            result = FAILURE;
        }
    }

  return result;
}
