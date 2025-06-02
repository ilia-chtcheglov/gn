#ifndef GN_STR_INIT_H
#define GN_STR_INIT_H

#include <gn_str_t.h>

#include <stdlib.h>

__attribute__((nonnull))
__attribute__((warn_unused_result))
gn_str_err_e
gn_str_init (gn_str_t * const str,
             const gn_str_len_t len);

#endif // GN_STR_INIT_H