#ifndef GN_HTBL_SRCH_H
#define GN_HTBL_SRCH_H

#include <gn_htbl_t.h>
#include <gn_str_t.h>

#include <stddef.h>
#include <stdio.h>
#include <string.h>

__attribute__((nonnull))
__attribute__((warn_unused_result))
gn_htbl_item_t *
gn_htbl_srch (gn_htbl_t * const tbl,
              const char * const key,
              gn_str_len_t key_len);

__attribute__((nonnull))
__attribute__((warn_unused_result))
gn_htbl_len_t
gn_htbl_hash (const gn_htbl_len_t size,
              const char * const key,
              const uint32_t key_len);

#endif // GN_HTBL_SRCH_H