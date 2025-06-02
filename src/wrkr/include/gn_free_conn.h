#ifndef GN_FREE_CONN_H
#define GN_FREE_CONN_H

#include <gn_conn_t.h>

#include <stdlib.h>

void
gn_free_conn (gn_conn_t ** const conn);

int
gn_close (int * const fd);

__attribute__((nonnull))
void
gn_htbl_free (gn_htbl_t * const tbl);

void
gn_str_deinit (gn_str_t * const str);

#endif // GN_FREE_CONN_H