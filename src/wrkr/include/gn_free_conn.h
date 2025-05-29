#ifndef GN_FREE_CONN_H
#define GN_FREE_CONN_H

#include <gn_conn_t.h>

#include <stdlib.h>

void
gn_free_conn (gn_conn_t ** const conn);

int
gn_close (int * const fd);

#endif // GN_FREE_CONN_H