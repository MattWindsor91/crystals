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

/** @file     src/tests/optionparser.c
 *  @author   Michael Walker
 *  @brief    A sanity test for the option parser. 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../util.h"
#include "../optionparser.h"

/** Main function of option parser test.
 *
 *  @param argc  Argument count.
 *  @param argv  Argument vector.
 *
 *  @return  0 for test success; other values for failure.
 */

int
main (int argc, char* argv[])
{
  /* Options and stuff */
  int         argn   = 10;
  const char* args[] = {"-a",    "-b5",     "-c1.3",     "-dhello", "-e", "8",
                        "--foo", "--bar=8", "--baz=9.6", "--qux=world"};

  int    a;
  int    b;
  double c;
  char*  d;
  int    e;
  int    foo;
  int    bar;
  double baz;
  char*  qux;

  option options[] = {{NULLC,         'a',   OPTION_PARAM_NONE,   NULL, (char*) "Presence flag."},
                      {NULLC,         'b',   OPTION_PARAM_INT,    NULL, (char*) "Integer parameter."},
                      {NULLC,         'c',   OPTION_PARAM_FLOAT,  NULL, (char*) "Float parameter."},
                      {(char*) "dah", 'd',   OPTION_PARAM_STRING, NULL, (char*) "String parameter."},
                      {NULLC,         'e',   OPTION_PARAM_INT,    NULL, (char*) "Integer parameter."},
                      {(char*) "foo", 'f',   OPTION_PARAM_NONE,   NULL, (char*) "Presence flag."},
                      {(char*) "bar", NULLC, OPTION_PARAM_INT,    NULL, (char*) "Integer parameter."},
                      {(char*) "baz", NULLC, OPTION_PARAM_FLOAT,  NULL, (char*) "Float parameter."},
                      {(char*) "qux", 'q',   OPTION_PARAM_STRING, NULL, (char*) "String parameter."},
                      {NULLC, NULLC, NULLC, NULLC, NULLC}};

  options[0].variable = &a;
  options[1].variable = &b;
  options[2].variable = &c;
  options[3].variable = &d;
  options[4].variable = &e;
  options[5].variable = &foo;
  options[6].variable = &bar;
  options[7].variable = &baz;
  options[8].variable = &qux;

  /* Don't want these */
  (void) argc;
  (void) argv;

  if (parse_options (argn, args, options) == FAILURE) return 1;

  if (a != TRUE)                  return 2;
  if (b != 5)                     return 3;
  if (c != 1.3)                   return 4;
  if (strcmp (d, "hello") != 0)   return 5;
  if (e != 8)                     return 6;
  if (foo != TRUE)                return 7;
  if (bar != 8)                   return 8;
  if (baz != 9.6)                 return 9;
  if (strcmp (qux, "world") != 0) return 10;

  free (d);
  free (qux);

  return 0;
}
