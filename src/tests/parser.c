#include "../parser.h"
#include "../util.h"
#include <stdio.h>

int main(void)
{
    dict_t *mydict = config_dict_init ();

    if (config_parse_file ("../../config/default.cfg",mydict) == SUCCESS)
        {
            printf("module_path = %s\n", config_get_value("module_path", mydict));
        }

    config_free_dict (mydict);
    return 0;

}
