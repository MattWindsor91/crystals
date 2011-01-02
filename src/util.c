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

/** @file    util.c
 *  @author  Matt Windsor
 *  @brief   Miscellaneous utility functions.
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include "util.h"
#include "main.h"

#ifdef PLATFORM_WINDOWS

#include "platform/w32-util.h" /* Windows error procedures */
#define ERROR_PROCEDURE(x, y, z) w32_error((x), (y), (z))

#else /* not PLATFORM_WINDOWS */

#define ERROR_PROCEDURE(x, y, z) std_error((x), (y), (z))

#endif /* not PLATFORM_WINDOWS */

/* Fatal error. */

void
fatal (const char message[], ...)
{
  va_list ap;
  va_start (ap, message);
  ERROR_PROCEDURE(message, ap, TRUE);
}


/* Non-fatal error. */

void
error (const char message[], ...)
{
  va_list ap;
  va_start (ap, message);
  ERROR_PROCEDURE(message, ap, FALSE);
}


/* Standard error reporting procedure. */

void
std_error (const char message[], va_list ap, int is_fatal)
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
