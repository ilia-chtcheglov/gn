#ifndef GN_EXTR_HDRN_H
#define GN_EXTR_HDRN_H

#include <gn_conn_t.h>

#include <ctype.h>
#include <stdio.h>

__attribute__((nonnull))
void
gn_extr_hdrn (gn_conn_t * const conn);

__attribute__((nonnull))
void
gn_htbl_dump (gn_htbl_t * const tbl);

__attribute__((nonnull))
__attribute__((warn_unused_result))
gn_htbl_item_t *
gn_htbl_srch (gn_htbl_t * const tbl,
              const char * const key,
              gn_str_len_t key_len);

__attribute__((nonnull))
__attribute__((warn_unused_result))
gn_str_err_e
gn_str_lshift (gn_str_t * const str,
               const gn_str_len_t num);

#endif // GN_EXTR_HDRN_H