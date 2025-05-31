#ifndef GN_CLOSE_CONN_H
#define GN_CLOSE_CONN_H

#include <gn_conn_t.h>

#include <stdlib.h>

void
gn_close_conn (gn_conn_t * const conn);

int
gn_close (int * const fd);

__attribute__((nonnull))
void
gn_htbl_free (gn_htbl_t * const tbl);

#endif // GN_CLOSE_CONN_H