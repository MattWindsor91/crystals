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

/** @file     src/platform/w32-util.c
 *  @author   Matt Windsor
 *  @brief    Miscellaneous utility functions for Windows.
 */


#include <stdarg.h>
#include <windows.h>

#include "../main.h"
#include "w32-main.h"
#include "w32-util.h"


/* -- DEFINITIONS -- */

/* Windows error reporting procedure. */

void
w32_error (const char message[], va_list ap, int is_fatal)
{
  HANDLE heap;
  LPTSTR error_string;

  /* Allocate enough memory to store the error string. (Potential TODO: dynamic allocation?) */

  heap = GetProcessHeap ();

  if (heap == NULL)
    return;

  error_string = HeapAlloc (heap,
                            HEAP_ZERO_MEMORY,
                            1024 * sizeof (TCHAR));

  if (error_string == NULL)
    return;

  /* Assemble the error string (FATAL/ERROR: <message with ap arguments>). */

  W32_SAFE_SPRINTF (error_string,
                    HeapSize (heap, 0, error_string) / sizeof (TCHAR),
                    TEXT (is_fatal ? "FATAL" : "ERROR"));

  W32_SAFE_VSPRINTF (error_string,
                     HeapSize (heap, 0, error_string) / sizeof (TCHAR) - 6,
                     message,
                     ap);

  /* Message box. */

  MessageBox (NULL,
              TEXT (error_string),
              TEXT (is_fatal ? "Fatal Error" : "Error"),
              MB_ICONERROR | MB_OK); 

  HeapFree (heap, 0, error_string);

  va_end (ap);

  if (is_fatal)
    {
	    cleanup ();
	    exit (1);
    }
}
