#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"
#include "parser.h"

int
config_parse_file (const char *path_name)
{
  /* simple booleans b_* */
  char b_ignore = FALSE;
  char b_value = FALSE;
  char b_whitespace = FALSE;

  char c;

  char *key="k", *sk;
  char *value="v", *sv;

  sk = key = malloc ((sizeof (char)) * 100);
  sv = value = malloc ((sizeof (char)) * 100);

  FILE *stream;

  int line_counter = 1;

  stream = fopen (path_name, "r");

  if (stream != NULL)
    {
      while ((c = fgetc (stream)) != EOF)
        {
          if (b_ignore)
            {
              if (c == '\n')
                {
                  if (key - sk > 0 && value - sv > 0)
                    {
                      *value = '\0';
                      key = key - (key - sk);
                      value = value - (value - sv);
                      add_pair (key, value);
                      b_value = FALSE;
                    }
                  ++line_counter;
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
                    if (key - sk > 0 && value - sv == 0)
                      {
                        fprintf (stderr, "Invalid config syntax at line %d \n",
                          line_counter);
                        return FAILURE;
                      }
                    else
                      {
                        b_ignore = TRUE;
                      }
                    break;
                  case '\n':
                    /* key is present but not a value */
                    if (key - sk > 0 && value - sv == 0)
                      {
                        fprintf (stderr, "Invalid config syntax at line %d \n",
                          line_counter);
                        return FAILURE;
                      }
                    else
                      {
                        if (key - sk > 0)
                          {
                            *value = '\0';
                            key = key - (key - sk);
                            value = value - (value - sv);
                            add_pair (key, value);
                            b_whitespace = FALSE;
                          }
                      }
                    b_value = FALSE;
                    ++line_counter;
                    break;
                  case '=':
                    if (b_value == FALSE)
                      {
                        *key = '\0';
                        b_value = TRUE;
                        b_whitespace = FALSE;
                      }
                    else
                      {
                        fprintf (stderr, "Invalid config syntax at line %d \n",
                          line_counter);
                        return FAILURE;
                      }
                    break;
                  case ' ':
                    if (b_whitespace == FALSE)
                      {
                        b_whitespace = TRUE;
                      }
                    break;
                  default:
                    if (b_whitespace == TRUE && value - sv > 0)
                      {
                        fprintf (stderr, "Invalid config syntax at line %d \n",
                          line_counter);
                        return FAILURE;
                      }
                    else
                      {
                        if (b_value == FALSE)
                          {
                            *key = c;
                            ++key;
                          }
                        else
                          {
                            *value = c;
                            ++value;
                          }
                        b_whitespace = FALSE;
                      }
                     break;
                }
            }
        }
    }
  else
    {
      fprintf(stderr, "Couldn't open the file");
      return FAILURE;
    }

  fclose (stream);
  free (key);
  free (value);

  return SUCCESS;
}

char*
config_get_value (const char *key)
{
  /* FIXME */
  return (char*) key;
}

static int
add_pair (char *key, char *value)
{
  /* FIXME */
  return 0;
}

/* vim: set ts=2 sw=2 softtabstop=2: */
