/* undef.c: undefined routines */

#include "angband.h"


extern short        log_index;

void 
init_files()
{
}

int 
_new_log()
{
    log_index = 0;
    return 1;
}
