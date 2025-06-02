#include <gn_str_deinit.h>

void
gn_str_deinit (gn_str_t * const str)
{
    if (str == NULL) return;

    free (str->dat);
    str->dat = NULL;
    str->len = 0;
    str->sz = 0;
}