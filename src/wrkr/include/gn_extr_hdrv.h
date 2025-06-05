#ifndef GN_EXTR_HDRV_H
#define GN_EXTR_HDRV_H

#include <gn_conn_t.h>

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

__attribute__((nonnull))
void
gn_extr_hdrv (gn_conn_t * const conn);

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
gn_htbl_item_t *
gn_htbl_srch (gn_htbl_t * const tbl,
              const char * const key,
              gn_str_len_t key_len);

#endif // GN_EXTR_HDRV_H