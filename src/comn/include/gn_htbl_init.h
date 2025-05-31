#ifndef GN_HTBL_INIT_H
#define GN_HTBL_INIT_H

#include <gn_htbl_t.h>

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

__attribute__((nonnull))
__attribute__((warn_unused_result))
bool
gn_htbl_init (gn_htbl_t * const tbl,
              const gn_htbl_len_t size);

#endif // GN_HTBL_INIT_H