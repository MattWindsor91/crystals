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

/** @file    src/optionparser.c
 *  @author  Michael Walker
 *  @brief   Functions to parse a set of options (such as argc)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"
#include "optionparser.h"

/* This returns whether the option is null or not */
int
is_null_option (option check)
{
  if (check.longname  == NULLC &&
      check.shortname == NULLC &&
      check.paramtype == NULLC &&
      check.variable  == NULLC &&
      check.helptext  == NULLC)
    return SUCCESS;
  return FAILURE;
}

/* This returns an option that matches a given criteria */
option
get_option (const char shortname, const char* longname, option options[])
{
  int i       = -1;
  int looping = TRUE;

  while (looping == TRUE)
    {
      ++ i;

      if (is_null_option (options[i]))
        {
          looping = FALSE;
        }
      else
        {
          if (shortname != NULLC)
            {
              if (shortname == options[i].shortname) looping = FALSE;
            }
          else
            {
              if (options[i].longname != NULL && strcmp (longname, options[i].longname) == 0) looping = FALSE;
            }
        }
    }

  return options[i];
}

/* Print help text and exit */
void
print_help_text (option options[])
{
  int i;

  printf ("Crystals [version]\n\n");

  for (i = 0; ; ++i)
    {
      if (is_null_option (options[i]))
        {
          exit (0);
        }
      else
        {
          if (options[i].shortname != NULLC) printf ("-%c  ", options[i].shortname);
          if (options[i].longname != NULLC)  printf ("--%s", options[i].longname);

          printf ("\n    %s\n", options[i].helptext);
        }
    }
}

/* Print version text and exit */
void
print_version_text (void)
{
  /* TODO: Move this info to a set of macros */
  /* TODO: Decide on a versioning scheme */
  printf ("Crystals [version]\n");
  printf ("Copyright (C) 2010 Matt Windsor, Alexander Preisinger, and Michael Walker.\n");
  printf ("License RBSD: Revised (3-clause) BSD license <http://github.com/Barrucadu/crystals/blob/experimental/COPYING>.\n");
  printf ("This is free software: you are free to change and redistribute it.\n");
  printf ("There is NO WARRANTY, to the extent permitted by law.\n");

  exit (0);
}

/* This parses a set of options */
int
parse_options(int argc, const char* argv[], option options[])
{
  int sopt    = TRUE;
  int looping = TRUE;
  int i, j;
  option opt;

  char* optname;
  char* optparam;
  char* working;

  int     *tmpintptr;
  char   **tmpstrptr;
  double  *tmpfloatptr;

  for (i = 0; i < argc; ++i)
    {
      looping = TRUE;

      /* Option to parse */
      if (strlen (argv[i]) >= 2)
        {
          sopt = ((argv[i][0] == '-' && argv[i][1] != '-') ? TRUE : FALSE);

          if (sopt == TRUE)
            {
              /* Short option */
              for (j = 1; j < (int) strlen (argv[i]) && looping == TRUE; ++j)
                {
                  opt = get_option (argv[i][j], (char*) NULL, options);
                  if (is_null_option (opt) == SUCCESS)
                    {
                      /* Try special options */
                      if (argv[i][j] == 'h')
                        {
                          print_help_text (options);
                        }
                      else if (argv[i][j] == 'v')
                        {
                          print_version_text ();
                        }
                      else
                        {
                          fprintf (stderr, "OPTIONS: Unknown option -%c.\n", argv[i][j]);
                          return FAILURE;
                        }
                    }
                  else
                    {
                      if (j == (int) strlen (argv[i]) - 1 && opt.paramtype != OPTION_PARAM_NONE)
                        {
                          /* At the end of this option, use the next */
                          if (i == argc - 1)
                            {
                              fprintf (stderr, "OPTIONS: Missing parameter for -%c.\n", argv[i][j]);
                              return FAILURE;
                            }

                          /* Use the next block of text as the option */
                          ++ i;
                          j = -1;
                          looping = FALSE;
                        }

                      switch (opt.paramtype)
                        {
                        case OPTION_PARAM_NONE:
                          tmpintptr = (int*) opt.variable;
                          *tmpintptr = TRUE;
                          break;

                        case OPTION_PARAM_INT:
                          tmpintptr = (int*) opt.variable;
                          *tmpintptr = atoi ((const char*) &argv[i][j + 1]);

                          looping = FALSE;
                          break;

                        case OPTION_PARAM_FLOAT:
                          tmpfloatptr = (double*) opt.variable;
                          *tmpfloatptr = atof ((const char*) &argv[i][j + 1]);
                          looping = FALSE;
                          break;

                        case OPTION_PARAM_STRING:
                          tmpstrptr = (char**) opt.variable;
                          *tmpstrptr = (char*) malloc (sizeof (char) * (strlen (&argv[i][j + 1]) + 1));
                          strcpy (*tmpstrptr, &argv[i][j + 1]);
                          looping = FALSE;
                          break;

                        default:
                          fprintf (stderr, "OPTIONS: Unknown option type %i.\n", opt.paramtype);
                          return FAILURE;
                          break;
                        }
                    }
                }
            }
          else
            {
              /* Long option */
              if (strstr (argv[i], "=") == NULL)
                {
                  /* No parameter */
                  opt = get_option (NULLC, argv[i] + 2, options);
                  if (is_null_option (opt) == SUCCESS)
                    {
                      /* Try special options */
                      if (strcmp (argv[i], "--help") == 0)
                        {
                          print_help_text (options);
                        }
                      else if (strcmp (argv[i], "--version") == 0)
                        {
                          print_version_text ();
                        }
                      else
                        {
                          fprintf (stderr, "OPTIONS: Unknown option %s.\n", argv[i]);
                          return FAILURE;
                        }
                    }
                  else
                    {
                      switch (opt.paramtype)
                        {
                        case OPTION_PARAM_NONE:
                          tmpintptr = (int*) opt.variable;
                          *tmpintptr = TRUE;
                          break;

                        case OPTION_PARAM_INT:
                        case OPTION_PARAM_FLOAT:
                        case OPTION_PARAM_STRING:
                          fprintf (stderr, "OPTIONS: Parameter missing for option %s.\n", argv[i]);
                          return FAILURE;
                          break;

                        default:
                          fprintf (stderr, "OPTIONS: Unknown option type %i.\n", opt.paramtype);
                          return FAILURE;
                          break;
                        }
                    }
                }
              else
                {
                  /* Has parameter */
                  working = (char*) malloc (sizeof (char) * (strlen (argv[i]) + 1));
                  strcpy (working, argv[i]);

                  optname  = strtok (working, "=");
                  optparam = strtok (NULL, "=");

                  optname += 2;

                  opt = get_option (NULLC, optname, options);

                  if (is_null_option (opt) == SUCCESS)
                    {
                      fprintf (stderr, "OPTIONS: Unknown option --%s (parameter '%s').\n", optname, optparam);

                      free (working);

                      return FAILURE;
                    }
                  else
                    {
                      switch (opt.paramtype)
                        {
                        case OPTION_PARAM_NONE:
                          fprintf (stderr, "OPTIONS: Unexpected parameter for argument --%s ('%s').\n", optname, optparam);

                          free (working);

                          return FAILURE;
                          break;

                        case OPTION_PARAM_INT:
                          tmpintptr = (int*) opt.variable;
                          *tmpintptr = atoi ((const char*) optparam);
                          break;

                        case OPTION_PARAM_FLOAT:
                          tmpfloatptr = (double*) opt.variable;
                          *tmpfloatptr = atof ((const char*) optparam);
                          break;

                        case OPTION_PARAM_STRING:
                          tmpstrptr = (char**) opt.variable;
                          *tmpstrptr = (char*) malloc (sizeof (char) * (strlen (optparam) + 1));
                          strcpy (*tmpstrptr, optparam);
                          break;

                        default:
                          fprintf (stderr, "OPTIONS: Unknown option type %i.\n", opt.paramtype);

                          free (working);

                          return FAILURE;
                          break;
                        }
                    }

                  free (working);
                }
            }
        }
    }
  
  return SUCCESS;
}
