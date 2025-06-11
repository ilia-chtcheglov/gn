#include <gn_str_lshift.h>

__attribute__((nonnull))
__attribute__((warn_unused_result))
gn_str_err_e
gn_str_lshift (gn_str_t * const str,
               const gn_str_len_t num)
{
    if (str->dat == NULL) return GN_STR_ERR_INTERN_DAT;
    if (str->sz == 0) return GN_STR_ERR_INTERN_SZ;
    if (str->len >= str->sz) return GN_STR_ERR_INTERN_LEN;
    if (num == 0) return GN_STR_ERR_NONE;
    if (num > str->len) return GN_STR_ERR_INVAL_RANGE;

    str->len -= num;
    memmove (str->dat, str->dat + num, str->len);
    str->dat[str->len] = '\0';

    return GN_STR_ERR_NONE;
}