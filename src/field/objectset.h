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
 * @file    src/field/objectset.h
 * @author  Matt Windsor
 * @brief   Public interface for the object set management functions.
 *
 * @todo FIXME: remove global state?
 */

#ifndef __OBJECTSET_H
#define __OBJECTSET_H


/* -- DECLARATIONS -- */

/**
 * Initialises the object base.
 */
void init_objects (void);


/**
 * Creates a new object and add it to the object table.
 *
 * @param object_name      The name of the object, used to look it up
 *                         in the object table.
 * @param script_filename  The filename of the script to associate
 *                         with the object. This script's
 *                         initialisation code will be called once
 *                         the object is installed.
 *
 * @return  a pointer to the new object, or NULL if there was an
 *          error while creating it.
 */
/*@dependent@*/ object_t *add_object (const char object_name[],
                                      const char script_filename[]);


/**
 * Removes an object from the object table.
 *
 * @param object_name  The name of the object to remove from the
 *                     table.
 *
 * @return  SUCCESS for success; FAILURE otherwise (eg if no object
 *          was deleted).
 */
bool_t delete_object (const char object_name[]);


/**
 * Deletes all objects in the object table.
 */
void clear_objects (void);


/**
 * Retrieves an object.
 *
 * @param object_name  The name of the object concerned.
 *
 * @return  A pointer to the object with the given name if found, or
 *          NULL otherwise.
 */
object_t *get_object (const char object_name[]);


/**
 * Apply the given function to all objects.
 *
 * @param function  Pointer to the function to apply to the
 *                  object.  The function must take the object
 *                  as first parameter followed by a void pointer for
 *                  data.
 * @param data      gpointer to the data to pass to the function.
 */
void apply_to_objects (GHFunc function, gpointer data);


/**
 * Clean up the objects subsystem.
 */
void cleanup_objects (void);


#endif /* not __OBJECTSET_H */
