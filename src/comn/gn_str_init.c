#include <gn_str_init.h>

__attribute__((nonnull))
__attribute__((warn_unused_result))
gn_str_err_e
gn_str_init (gn_str_t * const str,
             const gn_str_len_t len)
{
    str->dat = NULL;
    str->len = 0;
    str->sz = 0;

    if (len > GN_STR_LEN_MAX) return GN_STR_ERR_LEN;

    str->dat = (char *)malloc (len + 1);
    if (str->dat == NULL) return GN_STR_ERR_ALLOC;

    str->dat[0] = '\0';
    str->sz = len + 1;
    return GN_STR_ERR_NONE;
}