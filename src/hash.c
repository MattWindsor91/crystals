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
#include "util.h"         /* SUCCESS and FAILURE. */
#include "graphics.h"     /* free_image. */
#include "field/object.h" /* free_object. */


/* -- DEFINITIONS -- */

/* Return a basic hash of the given ASCII string, suitable for use in
 *  indexing for a hash table.
 */

hash_value_t
ascii_hash (const char string[])
{
  unsigned char *p;
  hash_value_t h;

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

hash_object_t *
create_hash_object (hash_object_t *head[],
                    const char name[], 
                    hash_type_t type,
                    void *data)
{
  hash_object_t *hash_object;
  hash_object_t *result;

  /* Sanity checking. */

  if (name == NULL)
    {
      error ("HASH - create_hash_object - Given name is NULL.");
      return NULL;
    }

  if (head == NULL) 
    {
      error ("HASH - create_hash_object - Given hash table for %s is NULL.", name);
      return NULL;
    }

  if (data == NULL)
    {
      error ("HASH - create_hash_object - Given data pointer for %s is NULL.", name);
      return NULL;
    }

  /* End sanity checking. */


  /* Now try to allocate the container. */

  hash_object = malloc (sizeof (hash_object_t));

  if (hash_object == NULL)
    {
      error ("HASH - create_hash_object - Could not allocate container for %s.", name);
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
      error ("HASH - create_hash_object - Name allocation failed for %s.", name);
      free_hash_object (hash_object);
      return NULL;
    }

  strncpy (hash_object->name, name, strlen (name) + 1);

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

      error ("HASH - create_hash_object - Could not add %s to hash table.", name);

      free_hash_object (hash_object);
      return NULL;      
    }

  return result;
}


/* Delete a hash object and its associated data. */

void
free_hash_object (hash_object_t *object)
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
          error ("HASH - free_hash_object - Hash object %s has data and no type.", 
                 object->name);
          break;
        case DATA_IMAGE:
          free_image ((struct image_t *) object->data);
          break;
        case DATA_SOUND:
          /* Insert sound freeing code here. */
          break;
        case DATA_OBJECT:
          free_object ((object_t *) object->data);
          break;
        }
    }

  if (object->name)
    free (object->name);

  free (object);
}


/* Remove a hash object from the given hash table. */

bool_t
delete_hash_object (hash_object_t *head[], const char name[])
{
  hash_value_t h;
  hash_object_t *object, *prev;

  h = ascii_hash (name);
  prev = NULL;


  /* Iterate through the hash bucket to find the correct object, then 
     delete its data and node. */

  for (object = head[h]; object != NULL; object = object->next)
    {
      prev = object;

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
clear_hash_objects (hash_object_t *head[])
{
  int i;
  hash_object_t *p, *next;
  
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

hash_object_t *
get_hash_object (hash_object_t *head[],
                 const char name[],
                 hash_object_t *add_pointer)
{
  hash_value_t h;
  hash_object_t *object;


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
              error ("HASH - get_hash_object - Duplicate object %s.", name);

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

hash_object_t *
find_hash_object (hash_object_t *head[],
                  const char name[])
{
  return get_hash_object (head, name, NULL);
}


/* Apply the given function to all members of the hash table. */

bool_t
apply_to_hash_objects (hash_object_t *head[],
                       bool_t (*function) (hash_object_t *object,
                                           void *data),
                       void *data)
{
  int i;
  bool_t result;
  bool_t temp_result;
  hash_object_t *hash_object;

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
