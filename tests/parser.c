
#include "../parser.h"
#include "../util.h"
#include <stdio.h>

int
main (int argc, char *argv[])
{
  parser_init ();
  if (config_parse_file ("parser.txt") == SUCCESS)
  {
    printf("SUCCESS\n");
    printf ("%s\n", config_get_value ("wo"));
  }
  cleanup_parser ();
  return 0;
}

