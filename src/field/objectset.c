/*
 * Crystals (working title)
 *
 * Copyright (c) 2010, 2011 Matt Windsor, Michael Walker and Alexander
 *                          Preisinger.
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

/**
 * @file    src/field/objectset.c
 * @author  Matt Windsor
 * @brief   Functions for maintaining a set of objects.
 *
 * @todo FIXME: remove global state?
 */

#include "../crystals.h"


/* -- STATIC GLOBAL VARIABLES -- */

static GHashTable *sg_objects; /**< Objects base (TODO: remove?) */


/* -- STATIC DECLARATIONS -- */

/**
 * Frees an object and all associated data, given a gpointer to it.
 *
 * @param object  gpointer to the object to delete.
 */
static void free_object_from_gpointer (gpointer object);


/* -- PUBLIC DEFINITIONS -- */

/* Initialises the object base. */
void
init_objects (void)
{
  sg_objects = g_hash_table_new_full (g_str_hash,
                                      g_str_equal,
                                      g_free,
                                      free_object_from_gpointer);

  g_assert (sg_objects);
}


/* Creates a new object and add it to the object table. */
object_t *
add_object (const char object_name[],
            const char script_filename[])
{
  object_t *object = xcalloc (1, sizeof (object_t));

  g_assert (object_name != NULL);
  g_assert (script_filename != NULL);

  /* Try to allocate an object. */
  object->image = init_object_image ();
  g_assert (object->image != NULL);

  object->name = g_strdup (object_name);
  g_assert (object->name != NULL);

  object->script_filename = g_strdup (script_filename);
  g_assert (object->script_filename != NULL);

  /* Finally, nullify everything else. */
  object->tag = 0;
  object->is_dirty = FALSE;

  g_hash_table_insert (sg_objects,
                       g_strdup(object_name),
                       object);

  return object;
}


/* Retrieves an object. */
object_t *
get_object (const char object_name[])
{
  return g_hash_table_lookup (sg_objects, object_name);
}


/* Removes an object from the object table. */
bool
delete_object (const char object_name[])
{
  return g_hash_table_remove (sg_objects, object_name);
}


/* Applies the given function to all objects. */
void
apply_to_objects (GHFunc function,
                  void *data)
{
  g_hash_table_foreach (sg_objects,
                        function,
                        data);
}


/* Cleans up the objects subsystem. */
void
cleanup_objects (void)
{
  g_hash_table_destroy (sg_objects);
}


/* -- STATIC DEFINITIONS -- */

/* Frees an object and all associated data, given a gpointer to it. */
static void
free_object_from_gpointer (gpointer object)
{
  free_object ((object_t *) object);
}
