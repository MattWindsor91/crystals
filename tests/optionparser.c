/* tests/optionparser - a sanity test for the option parser */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../util.h"
#include "../optionparser.h"

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
