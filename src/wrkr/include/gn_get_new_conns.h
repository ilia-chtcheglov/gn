#ifndef GN_GET_NEW_CONNS_H
#define GN_GET_NEW_CONNS_H

#include <gn_conn_mgmt_thrd_data_t.h>
#include <gn_conn_list_t.h>

#include <stdbool.h>

__attribute__((nonnull))
void
gn_get_new_conns (gn_conn_mgmt_thrd_data_t * const data,
                  gn_conn_list_t * const conn_list,
                  const bool stop);

int
gn_conn_list_push_back (gn_conn_list_t * const list,
                        gn_conn_t * const conn);

void
gn_free_conn (gn_conn_t ** const conn);

#endif // GN_GET_NEW_CONNS_H