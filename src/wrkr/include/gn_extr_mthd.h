#ifndef GN_EXTR_MTHD_H
#define GN_EXTR_MTHD_H

#include <gn_conn_t.h>
#include <gn_htbl_t.h>

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

__attribute__((nonnull))
void
gn_extr_mthd (gn_conn_t * const conn);

__attribute__((nonnull))
__attribute__((warn_unused_result))
bool
gn_htbl_insr (gn_htbl_t * const tbl,
              const char * const key,
              gn_str_len_t key_len,
              const char * const val,
              const gn_str_len_t val_len);

__attribute__((nonnull))
__attribute__((warn_unused_result))
gn_str_err_e
gn_str_lshift (gn_str_t * const str,
               const gn_str_len_t num);

#endif // GN_EXTR_MTHD_H