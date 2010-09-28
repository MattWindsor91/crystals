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

/** @file     parser.h
 *  @author   Alexander Preisinger
 *  @brief    Prototypes and declarations for the configuration parser.
 */

#ifndef _PARSER_H
#define _PARSER_H

typedef struct node_t node_t;
typedef char bool_t;

/** Node struct for a key-value tree
 *
 * Stores key, value and two branches
 *
 * If the key to add or the key you want to get is smaller
 * it will stored or searche in the left branch, if the key is bigger
 * it will be stored or searched in the right brach.
 */

struct node_t
{
  char *key;            /**< key item of the node. */
  char *value;          /**< value item of the node. */
  /* strcmp == -1 */
  node_t *left;  /**< Left branch of the node. */
  /* strcmp == 1 */
  node_t *right; /**< Right branch of the node. */
};

/* GLOBAL VARIABLES */
static node_t sg_root;


/* FUNCTION PROTOTYPES */

/** Initializes the parser. */

void
parser_init (void);


/** Clean up the parser
 *
 * Free all allocated memory.
 */

void
cleanup_parser (void);


/** Parse configuration file.
 *
 * @param path_name The path to the configuration file.
 *
 * @return Returns SUCCESS, if file has valid syntax and can be parsed, else
 * it returns FAILURE. (Defined in util.h)
 */

int
config_parse_file (const char *path_name);


/** Get the value of the appropriate key.
 *
 * @param key Pass a string, which will be searched in the tree.
 *
 * @return Return the value to the appropriate key or NULL if none is found.
 */

char*
config_get_value (const char *key);


/** Internal Funtion for getting the value.
 *
 * @param key   The key to search for.
 * @param node  The next branch to search in.
 *
 * @return Return the value to the appropriate key or NULL if none is found.
 */

static char*
get_value (const char *key, struct node_t *node);


/** Internal Funtion for adding a key-value pair to the tree.
 *
 * @param key   The string which will be added as key.
 * @param value The string which will be added as key.
 * @param node  The branch in which the key-value pair will be added.
 *
 * @return Returns SUCCESS, if the key-value pair can be added, if not or
 * the key already exists return FAILURE. (Defined in util.h)
 */

static int
add_pair (char *key, char *value, struct node_t *node);


/** Internal Function for initializing a node.
 *
 * @param node The node which will be initialized.
 *
 * @param Return the initialized node.
 */

static struct node_t*
node_init (struct node_t *node);


/** Internal Function for freeing allocated memory in nodes.
 *
 * @param node The data in the node will be freed.
 */

static void
free_node (struct node_t *node);

#endif /* _PARSER_H */
/* vim: set ts=2 sw=2 softtabstop=2: */
