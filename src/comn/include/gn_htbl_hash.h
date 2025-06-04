#ifndef GN_HTBL_HASH_H
#define GN_HTBL_HASH_H

#include <gn_htbl_t.h>
#include <gn_str_t.h>

#include <stddef.h>

__attribute__((nonnull))
__attribute__((warn_unused_result))
gn_htbl_len_t
gn_htbl_hash (const gn_htbl_len_t size,
              const char * const key,
              const gn_str_len_t key_len);

#endif // GN_HTBL_HASH_H