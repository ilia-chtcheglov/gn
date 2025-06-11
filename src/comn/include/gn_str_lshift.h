#ifndef GN_STR_LSHIFT_H
#define GN_STR_LSHIFT_H

#include <gn_str_t.h>

#include <string.h>

__attribute__((nonnull))
__attribute__((warn_unused_result))
gn_str_err_e
gn_str_lshift (gn_str_t * const str,
               const gn_str_len_t num);

#endif // GN_STR_LSHIFT_H