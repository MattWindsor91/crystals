/* tests/optionparser - a sanity test for the option parser */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../main.h"
#include "../optionparser.h"

int
main (int argc, char* argv[])
{
  /* Options and stuff */
  int   argn   = 10;
  char* args[] = {"-a",    "-b5",     "-c1.3",     "-dhello", "-e", "8",
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

  option options[] = {{NULLC, 'a',   OPTION_PARAM_NONE,   &a,   "Presence flag."},
                      {NULLC, 'b',   OPTION_PARAM_INT,    &b,   "Integer parameter."},
                      {NULLC, 'c',   OPTION_PARAM_FLOAT,  &c,   "Float parameter."},
                      {"dah", 'd',   OPTION_PARAM_STRING, &d,   "String parameter."},
                      {NULLC, 'e',   OPTION_PARAM_INT,    &e,   "Integer parameter."},
                      {"foo", 'f',   OPTION_PARAM_NONE,   &foo, "Presence flag."},
                      {"bar", NULLC, OPTION_PARAM_INT,    &bar, "Integer parameter."},
                      {"baz", NULLC, OPTION_PARAM_FLOAT,  &baz, "Float parameter."},
                      {"qux", 'q',   OPTION_PARAM_STRING, &qux, "String parameter."},
                      {NULLC, NULLC, NULLC, NULLC, NULLC}};

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

  return 0;
}
