#ifndef GN_EXTR_HDRV_H
#define GN_EXTR_HDRV_H

#include <gn_conn_t.h>

#include <stdbool.h>
#include <stdio.h>

__attribute__((nonnull))
void
gn_extr_hdrv (gn_conn_t * const conn);

__attribute__((nonnull))
__attribute__((warn_unused_result))
bool
gn_htbl_insr (gn_htbl_t * const tbl,
              const char * const key,
              uint32_t key_len,
              const char * const val,
              const uint32_t val_len);

#endif // GN_EXTR_HDRV_H