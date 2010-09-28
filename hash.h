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

/** @file    hash.h
 *  @author  Matt Windsor
 *  @brief   Prototypes and declarations for generic hash table
 *  implementation.
 */

#ifndef _HASH_H
#define _HASH_H

/* -- CONSTANTS -- */

enum
  {
    HASH_VALS = 20, /**< Number of hash values used in a hash table,
                       and therefore the number of separate linked
                       lists. Increasing this may increase performance. */

    HASH_MUL  = 31, /**< Hash value used in the hash table
                       algorithms. This value is taken from Kernighan
                       and Pike's ``The Practice of Programming'', as
                       is the algorithmic concept. */

    /* Data types. */

    DATA_NULL = 0,  /**< No data stored in container. */
    DATA_IMAGE,     /**< Image data type. */
    DATA_SOUND,     /**< Sound data type. */
    DATA_OBJECT     /**< Object data type. */
  };

/* -- STRUCTURES -- */

/** A generic hash table data container. */

struct hash_object
{
  char *name;               /**< Unique name/identifier of the
                               object. */
  unsigned char type;       /**< The type ID of data held in the 
                               object. */
  void *data;               /**< Data held by the object. */
  struct hash_object *next; /**< The next object in the hash chain. */
};

/* -- PROTOTYPES -- */

/** Return a basic hash of the given ASCII string, suitable for use in
 *  indexing for a hash table.
 *
 *  @param string  The sequence of characters to hash. 
 *
 *  @return  A hash of the input string, from 0 to HASH_VALS - 1
 *  inclusive.
 */

int
ascii_hash (const char string[]);

/** Create a hash object with the given data and link it to the given
 *  hash table.
 *
 *  @param head  Double-pointer to the head of the hash table.
 *
 *  @param name  Unique name/identifier of the object to
 *  remove.
 * 
 *  @param type  The type identifier for the data.
 *
 *  @param data  A void pointer to the data to store. 
 *
 *  @return  A pointer to the newly added hash object if successful,
 *  or NULL otherwise.
 */

struct hash_object *
create_hash_object (struct hash_object *head[], 
                    const char name[], 
                    unsigned char type, 
                    void *data);

/** Delete a hash object and its associated data.
 *
 *  This will try to use the hash object's metadata to decide which
 *  function to call to destroy the hash object's contents.
 *
 *  @param object  Pointer to the hash object to delete.
 */

void
free_hash_object (struct hash_object *object);

/** Remove a hash object from the given hash table.
 *
 *  @param head  Double-pointer to the head of the hash table.
 *
 *  @param name  Unique name/identifier of the object to
 *  remove.
 *
 *  @return  SUCCESS for success; FAILURE otherwise (eg if no object
 *  was deleted).
 */ 

int
delete_hash_object (struct hash_object *head[], 
                    const char name[]);

/** Deletes all containers in the given hash table.
 *
 *  @param head  Double-pointer to the head of the hash table. 
 */

void
clear_hash_objects (struct hash_object *head[]);

/** Retrieve an hash table node, or add one to a hash table.
 *
 *  @param head  Double-pointer to the head of the hash table. 
 *
 *  @param name  Unique name/identifier of the object.
 *
 *  @param add_pointer  If this is non-NULL, the function will attempt
 *                      to add the object pointed to by this function
 *                      to the object table.  Otherwise, the function
 *                      will try to find existing objects with the
 *                      given name.
 *
 *  @return  A pointer to the hash table container with the given name
 *  if found or created, or NULL otherwise.
 */

struct hash_object *
get_hash_object (struct hash_object *head[], 
                 const char name[],
                 struct hash_object *add_pointer);

/** Wrapper to get_hash_object for use in finding hash objects.
 *
 *  @param head  Double-pointer to the head of the hash table. 
 *
 *  @param name  Unique name/identifier of the object.
 */

struct hash_object *
find_hash_object (struct hash_object *head[], 
                  const char name[]);

/** Apply the given function to all members of the hash table.
 *
 *  @param head      Double-pointer to the head of the hash table.
 *
 *  @param function  Pointer to the function to apply to the
 *                   object.  The function must take the hash object
 *                   as first parameter followed by a variadic
 *                   parameter set (va_list), and return a
 *                   SUCCESS/FAILURE int.
 *
 *  @return SUCCESS if all applications of the function succeeded,
 *  FAILURE otherwise.
 */

int
apply_to_hash_objects (struct hash_object *head[], 
                       int (*function) (struct hash_object *object, 
                                        va_list ap),
                       ...);

#endif /* not _HASH_H */
