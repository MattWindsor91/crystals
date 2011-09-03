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
 * @file    util.c
 * @author  Matt Windsor
 * @brief   Miscellaneous utility functions.
 */


#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <limits.h>

#include "util.h"
#include "parser.h"
#include "main.h"


/* - DEFINITIONS - */

/* -- Path retrieval -- */

/* Get the path to the directory in which all modules are stored. */

void
get_module_root_path (char **module_path)
{
  /* If a string exists here, free it. */

  if (*module_path != NULL)
    {
      free (module_path);
      *module_path = NULL;
    }


  /* If configuration loading succeeded, try to grab the module path
     from the config file.  If this doesn't work, use the default
     path. */

  if (g_config)
    *module_path = cfg_get_str ("modules", "module_path", g_config);


  if (*module_path == NULL)
    {
      error ("UTIL - get_module_path - Cannot read module path from config.");

      /* Copy the default path to the pointer. */

      *module_path = calloc (strlen (DEFMODPATH) + 1, sizeof (char));

      if (*module_path == NULL)
        fatal ("UTIL - get_module_path - Module path could not be allocated.");
      else
        strncpy (*module_path, DEFMODPATH, strlen (DEFMODPATH) + 1);
    }
}


/* ~~ Safe type conversions */

/* Safely convert a long integer to a 16-bit unsigned integer. */

uint16_t
long_to_uint16 (long integer)
{
  if (integer < 0)
    {
      error ("UTIL - long_to_uint16 - Assertion n >= 0 failed.");
      error ("UTIL - Truncating value to 0.");
      integer = 0;
    }
  else if (integer > UINT16_MAX)
    {
      error ("UTIL - long_to_uint16 - Assertion n <= UINT16_MAX failed.");
      error ("UTIL - Truncating value to uint16_t maximum.");
      integer = UINT16_MAX;
    }

  return (uint16_t) integer;
}


/* Safely convert a long integer to a signed 16_bit integer. */

int16_t
long_to_int16 (long integer)
{
  if (integer < INT16_MIN)
    {
      error ("UTIL - long_to_int16 - Assertion n >= INT16_MIN failed.");
      error ("UTIL - Truncating value to int16_t minimum.");
      integer = INT16_MIN;
    }
  else if (integer > INT16_MAX)
    {
      error ("UTIL - long_to_int16 - Assertion n <= INT16_MAX failed.");
      error ("UTIL - Truncating value to int16_t minimum.");
      integer = INT16_MAX;
    }

  return (int16_t) integer;
}


/* Safely convert an unsigned long integer to an unsigned 16-bit integer. */

uint16_t
ulong_to_uint16 (unsigned long integer)
{
  if (integer > UINT16_MAX)
    {
      error ("UTIL - ulong_to_uint16 - Assertion n <= UINT16_MAX failed.");
      error ("UTIL - Truncating value to uint16_t maximum.");
      integer = UINT16_MAX;
    }

  return (uint16_t) integer;
}


/* Safely convert an unsigned long integer to a signed 16-bit integer. */

int16_t
ulong_to_int16 (unsigned long integer)
{
  if (integer > INT16_MAX)
      {
        error ("UTIL - ulong_to_int16 - Assertion n <= INT16_MAX failed.");
        error ("UTIL - Truncating value to int16_t minimum.");
        integer = INT16_MAX;
      }

  return (int16_t) integer;
}


/* ~~ Error reporting */

/* Fatal error. */

void
fatal (const char message[], ...)
{
  va_list ap;
  va_start (ap, message);
  ERROR_PROCEDURE (message, ap, TRUE);
}


/* Non-fatal error. */

void
error (const char message[], ...)
{
  va_list ap;
  va_start (ap, message);
  ERROR_PROCEDURE (message, ap, FALSE);
}


/* Standard error reporting procedure. */

void
std_error (const char message[], va_list ap, bool_t is_fatal)
{
  if (is_fatal)
    fprintf (stderr, "FATAL: ");
  else
    fprintf (stderr, "ERROR: ");

  vfprintf (stderr, message, ap);
  fprintf (stderr, "\n");
  va_end (ap);

  fflush (stderr);


  if (is_fatal)
    {
      cleanup ();
      exit (1);
    }
}


