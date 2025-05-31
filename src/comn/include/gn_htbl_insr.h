#ifndef GN_HTBL_INSR_H
#define GN_HTBL_INSR_H

#include <gn_htbl_t.h>

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

__attribute__((nonnull))
__attribute__((warn_unused_result))
bool
gn_htbl_insr (gn_htbl_t * const tbl,
              const char * const key,
              uint32_t key_len,
              const char * const val,
              const uint32_t val_len);

__attribute__((nonnull))
__attribute__((warn_unused_result))
gn_htbl_len_t
gn_htbl_hash (const gn_htbl_len_t size,
              const char * const key,
              const uint32_t key_len);

__attribute__((nonnull))
void
gn_htbl_item_init (gn_htbl_item_t * const item);

#endif // GN_HTBL_INSR_H