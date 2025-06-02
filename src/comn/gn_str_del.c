#include <gn_str_del.h>
#include <stdio.h>
void
gn_str_del (gn_str_t ** const str)
{
    if (str == NULL) return;

    gn_str_deinit (*str);
    free (*str);
    *str = NULL;
}