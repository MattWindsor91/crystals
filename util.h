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

/** @file    util.h
 *  @author  Matt Windsor
 *  @brief   Miscellaneous utility prototypes and declarations.
 *
 *  This, and its corresponding C file, contain things that are
 *  undeserving of their own code file, but nevertheless useful for
 *  the engine.
 */

#ifndef _UTIL_H
#define _UTIL_H

/* -- CONSTANTS -- */

enum
  {
    GFX_HASH_NAME_LEN = 100, /**< Maximum size of the part of the
                                image filename used by the hashing
                                function. If the filename (relative
                                to the image directory) is larger,
                                then an error will be tripped. */

    HASH_VALS = 10, /**< Number of hash values used in a hash table,
                       and therefore the number of separate linked
                       lists. Increasing this may increase performance. */

    HASH_MUL  = 31, /**< Hash value used in the hash table
                       algorithms. This value is taken from Kernighan
                       and Pike's ``The Practice of Programming'', as
                       is the algorithmic concept. */

    SUCCESS = 1, /**< Value raised by functions upon success. */
    FAILURE = 0, /**< Value raised by functions upon failure. */
    TRUE = 1,    /**< Boolean true value. */
    FALSE = 0,   /**< Boolean false value. */
    NORTH = 0,   /**< North (up) cardinal direction. */
    EAST,        /**< East (right) cardinal direction. */
    SOUTH,       /**< South (down) cardinal direction. */
    WEST,        /**< West (left) cardinal direction. */
    NULLC = '\0' /**< Null character (necessary?) */
  };

/* -- FUNCTION MACROS -- */

#define MAX(x, y) ((x) > (y) ? (x) : (y)) /**< Get the maximum of two
                                             values. */

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

#endif /* _UTIL_H */
