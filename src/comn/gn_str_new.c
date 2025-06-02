#include <gn_str_new.h>

__attribute__((warn_unused_result))
gn_str_t *
gn_str_new (const gn_str_len_t len)
{
    if (len > GN_STR_LEN_MAX) return NULL;

    gn_str_t * str = (gn_str_t *)malloc (sizeof (gn_str_t));
    if (str == NULL) return NULL;

    if (gn_str_init (str, len) != GN_STR_ERR_NONE)
    {
        free (str);
        str = NULL;
    }

    return str;
}