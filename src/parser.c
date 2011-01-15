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

/** 
 * @file    src/parser.c
 * @author  Alexander Preisinger
 * @brief   Configuration parser.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"
#include "parser.h"

#ifdef TESTSUITE
void error (const char message[], ...) { (void) message; }
#endif

/* Initialise the config system. */

dict_t *
init_config (const char *config_path)
{
  dict_t *config = NULL;

  /* Sanity check the inbound path. */

  if (config_path == NULL)
    {
      error ("PARSER - init_config - Given NULL path to config file.");
    }
  else 
    {
      if ((cfg_parse (config_path, &config)) == FAILURE)
        {
          error ("PARSER - init_config - Could not parse config file.");
          cfg_free (config);
          config = NULL;
        }
    }
    
  return config;
}


/* Parse configuration file. */

bool_t
cfg_parse (const char *path_name, dict_t **root)
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

  char *key, *key_head;
  char *value, *value_head;
  
  FILE *stream;

  key_head = key = calloc (100, sizeof (char));
  value_head = value = calloc (100, sizeof (char));

  stream = fopen (path_name, "r");

  if (stream != NULL)
    {
      while ((c = (char) fgetc (stream)) != EOF && b_error == FALSE)
        {
          /* check if someone wants to exploit us */
          if (key - key_head == 99)
            {
              error ("PARSER - cfg_parse - Key is too long: %d.", line_counter);
              return FAILURE;
            }
          if (b_ignore)
            {
              if (c == '\n')
                {
                  if (b_key && b_value)
                    {
                      *value = '\0';
                      key = key_head;
                      value = value_head;
                      if (cfg_add (key, value, root) == FAILURE)
                        {
                          error (
                            "PARSER - cfg_parse - Key is already present: %d.",
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
                        error ("PARSER - cfg_parse - Invalid config syntax at line %d.",
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
                        error ("PARSER - cfg_parse - Invalid config syntax at line %d.", 
                              line_counter);
                        b_error = TRUE;
                      }
                    else
                      {
                        if (b_key)
                          {
                            *value = '\0';
                            key = key_head;
                            value = value_head;
                            if (cfg_add (key, value, root) == FAILURE)
                              {
                                error ("PARSER - cfg_parse - Key is already present: %d.", 
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
                        error ("PARSER - cfg_parse - Invalid config syntax at line %d.",
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
                        error ("PARSER - cfg_parse - Invalid config syntax at line %d.",
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
      error ("PARSER - cfg_parse - Could not open the file %s.", path_name);
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
cfg_free (dict_t *node)
{
  if (node != NULL)
    {
      cfg_free (node->left);
      cfg_free (node->right);
      free (node->key);
      free (node->value);
      free (node);
      node = NULL;
    }
}


/* Get the value of the appropriate key. */

char *
cfg_get (const char *key, dict_t *node)
{
  if (node != NULL)
    {
      if (strcmp (key, node->key) < 0)
        {
          return cfg_get (key, node->left);
        }
      else if (strcmp (key, node->key) > 0)
        {
          return cfg_get (key, node->right);
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

bool_t
cfg_add (const char *key, const char *value, dict_t **root)
{
  /* Sanity check */
    
  dict_t *node = *root;
  dict_t *parent = *root;
  bool_t left = FALSE;

  if (key == NULL)
    {
      error ("PARSER - cfg_add - Key is NULL.");
      return FAILURE;
    }
  else if (value == NULL)
    {
      error ("PARSER - cfg_add - Value is NULL.");
      return FAILURE;
    }
    
  /* End sanity check. */
  
  while (node != NULL) 
    {
      parent = node; /* keep track of the previous node */
      
      if (strcmp (key, node->key) < 0) 
        {
          node = node->left;
          left = TRUE;
        } 
      else if (strcmp (key, node->key) > 0)
        {
          node = node->right;
          left = FALSE;
        }
      else 
        {
          error ("PARSER - cfg_add - Duplicate keys.");
          return FAILURE;
        }
    }
  
  node = malloc (sizeof (dict_t));
  
  node->key = calloc (strlen (key) + 1, sizeof (char));
  strncpy (node->key, key, strlen (key));

  node->value = calloc (strlen (value) + 1, sizeof (char));
  strncpy (node->value, value, strlen (value));

  node->left = NULL;
  node->right = NULL;
  
  if (parent != NULL) 
    {
      if (left)
        parent->left = node;
      else
        parent->right = node;
    }
  else
      *root = node;
    
  return SUCCESS;
}


/* Return the numbers of items in the dictionary type */

unsigned int
cfg_items (dict_t *node)
{
  if (node == NULL)
    {
      return 0;
    }
  else
    {
      return cfg_items (node->left) 
           + cfg_items (node->right)
           + 1; /* The node itself */
    }
}

/* vim: set ts=2 sw=2 softtabstop=2: */
