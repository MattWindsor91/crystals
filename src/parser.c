#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"
#include "parser.h"

void
parser_init (void)
{
  node_init (&sg_root);
}

int
config_parse_file (const char *path_name)
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
              fprintf (stderr, "Key is too long: %d \n", line_counter);
              b_error = TRUE;
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
                      if (add_pair (key, value, &sg_root) == FAILURE)
                        {
                          fprintf (stderr, "Key is already present: %d \n",
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
                        fprintf (stderr, "Invalid config syntax at line %d \n",
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
                        fprintf (stderr, "Invalid config syntax at line %d \n",
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
                            if (add_pair (key, value, &sg_root) == FAILURE)
                              {
                                fprintf (stderr,
                                  "Key is already present: %d \n",
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
                        fprintf (stderr, "Invalid config syntax at line %d \n",
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
                        fprintf (stderr, "Invalid config syntax at line %d \n",
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
      fprintf(stderr, "Couldn't open the file");
      b_error = TRUE;
    }

  free (key);
  free (value);

  if (b_error)
    return FAILURE;
  else
    return SUCCESS;
}

char*
config_get_value (const char *key)
{
  return get_value (key, &sg_root);
}

void
cleanup_parser (void)
{
  free_node (sg_root.left);
  free_node (sg_root.right);
  free (sg_root.key);
  free (sg_root.value);
}

static char*
get_value(const char *key, node_t *node)
{
  if (node->key != NULL)
    {
      if (strcmp (key, node->key) == -1)
        {
          return get_value (key, node->left);
        }
      else if (strcmp (key, node->key) == 1)
        {
          return get_value (key, node->right);
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

static int
add_pair (char *key, char *value, node_t *node)
{
  node_t *n_left;
  node_t *n_right;

  if (node->key == NULL)
    {
      node->key = malloc (sizeof (char) * strlen (key) + 1);
      strcpy (node->key, key);

      node->value = malloc (sizeof (char) * strlen (value) + 1);
      strcpy (node->value, value);

      n_left = malloc (sizeof (node_t));
      n_right = malloc (sizeof (node_t));

      node->left = node_init (n_left);
      node->right = node_init (n_right);
      return SUCCESS;
    }
  else
    {
      if (strcmp (key, node->key) == -1)
        {
          return add_pair (key, value, node->left);
        }
      else if (strcmp (key, node->key) == 1)
        {
          return add_pair (key, value, node->right);
        }
      else
        {
          return FAILURE;
        }
    }
}

static node_t*
node_init (node_t *node)
{
  node->key = NULL;
  node->value = NULL;
  node->left = NULL;
  node->right = NULL;
  return node;
}

static void
free_node (node_t *node)
{
  if (node->key == NULL)
    {
      free (node);
    }
  else
    {
      free_node (node->left);
      free_node (node->right);
      free (node->key);
      free (node->value);
      free (node);
    }
}

/* vim: set ts=2 sw=2 softtabstop=2: */
