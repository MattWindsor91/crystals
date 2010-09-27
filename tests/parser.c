
#include "../parser.h"
#include "../util.h"
#include <stdio.h>

int
main (int argc, char *argv[])
{
  if (argc > 1)
    {
      parser_init ();
      
      if (config_parse_file ("../config/default.cfg") == SUCCESS)
        {
          printf ("%s\n", config_get_value ((const char*) argv[1]));
        }
     
     cleanup_parser ();
    
    }
  return 0;
}

