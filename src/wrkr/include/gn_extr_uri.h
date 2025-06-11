#ifndef GN_EXTR_URI_H
#define GN_EXTR_URI_H

#include <gn_conn_t.h>

#include <stdio.h>

__attribute__((nonnull))
void
gn_extr_uri (gn_conn_t * const conn);

__attribute__((nonnull))
__attribute__((warn_unused_result))
gn_str_err_e
gn_str_lshift (gn_str_t * const str,
               const gn_str_len_t num);

#endif // GN_EXTR_URI_H