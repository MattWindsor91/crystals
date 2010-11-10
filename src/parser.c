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

/** @file   src/parser.c
 *  @author Alexander Preisinger
 *  @brief  Configuration parser.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"
#include "parser.h"


/* Initialise the config system. */

dict_t *
init_config (const char *config_path)
{
  dict_t *config;

  /* Sanity check the inbound path. */

  if (config_path == NULL)
    {
      error ("PARSER - init_config - Given NULL path to config file.");
      return NULL;
    }

  config = config_dict_init();

  if (config == NULL)
    {
      error ("PARSER - init_config - Failed to init config dict.");
      return NULL;
    }

  /* Now try to parse the configuration.  If we fail, destroy the dict. */

  if (config_parse_file (config_path, config) == FAILURE)
    {
      error ("PARSER - init_config - Could not parse config file.");
      config_free_dict (config);
      return NULL;
    }

  return config;
}


/* Parse configuration file. */

int
config_parse_file (const char *path_name, dict_t *root)
{
  /* simple booleans b_* */
  bool_t b_key = FALSE;
  bool_t b_value = FALSE;
  bool_t b_switch = FALSE;
  bool_t b_ignore = FALSE;
  bool_t b_whitespace = FALSE;
  bool_t b_error = FALSE;

  char c;
  int line_counter = 1;

  char *key, *sk;
  char *value, *sv;

  FILE *stream;

  sk = key = malloc (sizeof (char) * 100);
  sv = value = malloc (sizeof (char) * 100);

  stream = fopen (path_name, "r");

  if (stream != NULL)
    {
      while ((c = (char) fgetc (stream)) != EOF && b_error == FALSE)
        {
          /* check if someone wants to exploit us */
          if (key - sk == 99)
            {
              fprintf (stderr, "PARSER: Key is too long: %d \n", line_counter);
              break;
            }
          if (b_ignore)
            {
              if (c == '\n')
                {
                  if (b_key && b_value)
                    {
                      *value = '\0';
                      key = sk;
                      value = sv;
                      if (config_add_pair (key, value, root) == FAILURE)
                        {
                          fprintf (stderr, "PARSER: Key is already present: %d \n",
                            line_counter);
                          b_error = TRUE;
                        }
                    }
                  ++line_counter;
                  b_key = FALSE;
                  b_value = FALSE;
                  b_switch = FALSE;
                  b_ignore = FALSE;
                  b_whitespace = FALSE;
                }
            }
          else
            {
              switch (c)
                {
                  case '#':
                    /* key is present but not a value */
                    if (b_key == TRUE && b_value == FALSE)
                      {
                        fprintf (stderr, "PARSER: Invalid config syntax at line %d \n",
                          line_counter);
                        b_error = TRUE;
                      }
                    else
                      {
                        b_ignore = TRUE;
                      }
                    break;
                  case '\n':
                    /* key is present but not a value */
                    if (b_key == TRUE && b_value == FALSE)
                      {
                        fprintf (stderr, "PARSER: Invalid config syntax at line %d \n",
                          line_counter);
                        b_error = TRUE;
                      }
                    else
                      {
                        if (b_key)
                          {
                            *value = '\0';
                            key = sk;
                            value = sv;
                            if (config_add_pair (key, value, root) == FAILURE)
                              {
                                fprintf (stderr,
                                  "PARSER: Key is already present: %d \n",
                                  line_counter);
                                b_error = TRUE;
                              }
                          }
                      }
                    b_key = FALSE;
                    b_value = FALSE;
                    b_switch = FALSE;
                    b_whitespace = FALSE;
                    ++line_counter;
                    break;
                  case '=':
                    if (b_switch == FALSE)
                      {
                        *key = '\0';
                        b_whitespace = FALSE;
                        b_switch = TRUE;
                      }
                    else
                      {
                        fprintf (stderr, "PARSER: Invalid config syntax at line %d \n",
                          line_counter);
                        b_error = TRUE;
                      }
                    break;
                  case ' ':
                    if (b_whitespace == FALSE)
                      {
                        b_whitespace = TRUE;
                      }
                    break;
                  default:
                    if (b_whitespace == TRUE && b_value == TRUE)
                      {
                        fprintf (stderr, "PARSER: Invalid config syntax at line %d \n",
                          line_counter);
                        b_error = TRUE;
                      }
                    else
                      {
                        if (b_switch == FALSE)
                          {
                            b_key = TRUE;
                            *key = c;
                            ++key;
                          }
                        else
                          {
                            b_value = TRUE;
                            *value = c;
                            ++value;
                          }
                        b_whitespace = FALSE;
                      }
                     break;
                }
            }
        }

    fclose (stream);

    }
  else
    {
      fprintf(stderr, "PARSER: Could not open the file %s .\n", path_name);
    }

  free (key);
  free (value);

  if (b_error == TRUE)
    return FAILURE;
  else
    return SUCCESS;
}


/* Function for freeing allocated memory in nodes. */

void
config_free_dict (dict_t *node)
{
  if (node != NULL)
    {
      if (node->key == NULL)
        free (node);
      else
        {
          config_free_dict (node->left);
          config_free_dict (node->right);
          free (node->key);
          free (node->value);
          free (node);
        }
    }
}


/* Get the value of the appropriate key. */

char *
config_get_value (const char *key, dict_t *node)
{
  if (node->key != NULL)
    {
      if (strcmp (key, node->key) < 0)
        {
          return config_get_value (key, node->left);
        }
      else if (strcmp (key, node->key) > 0)
        {
          return config_get_value (key, node->right);
        }
      else
        {
          return node->value;
        }
    }
  else
    {
      return NULL;
    }
}


/* Function for adding a key-value pair to the tree. */

int
config_add_pair (const char *key, const char *value, dict_t *node)
{
  /* Sanity check */

  if (key == NULL)
    {
      error ("PARSER - config_add_pair - Key is NULL.");
      return FAILURE;
    }
  else if (value == NULL)
    {
      error ("PARSER - config_add_pair - Value is NULL.");
      return FAILURE;
    }
  else if (node == NULL)
    {
      error ("PARSER - config_add_pair - Node is NULL.");
      return FAILURE;
    }

  /* End sanity check. */


  if (node->key == NULL)
    {
      node->key = malloc (sizeof (char) * strlen (key) + 1);
      strncpy (node->key, key, strlen (value) + 1);

      node->value = malloc (sizeof (char) * strlen (value) + 1);
      strncpy (node->value, value, strlen (value) + 1);

      node->left = config_dict_init ();
      node->right = config_dict_init ();

      return SUCCESS;
    }
  else
    {
      if (strcmp (key, node->key) < 0)
        {
          return config_add_pair (key, value, node->left);
        }
      else if (strcmp (key, node->key) > 0)
        {
          return config_add_pair (key, value, node->right);
        }
      else
        {
          return FAILURE;
        }
    }
}


/* Return the numbers of items in the dictionary type */

int
config_item_count (dict_t *node)
{
  if (node->key == NULL)
    {
      return 0;
    }
  else
    {
      return config_item_count (node->left) + config_item_count (node->right) + 1;
    }
}


/* Initializing a node. */

dict_t*
config_dict_init (void)
{
  dict_t *node = malloc (sizeof (dict_t));
  node->key = NULL;
  node->value = NULL;
  node->left = NULL;
  node->right = NULL;
  return node;
}

/* vim: set ts=2 sw=2 softtabstop=2: */
