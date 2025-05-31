#ifndef GN_CONN_MGMT_THRD_H
#define GN_CONN_MGMT_THRD_H

#include <gn_conn_list_t.h>
#include <gn_conn_mgmt_thrd_data_t.h>

#include <gn_conn_mgmt_thrd_state_e.h>

#include <stdbool.h>
#include <stdio.h>

void *
gn_conn_mgmt_thrd (void * const p);

void
gn_conn_list_remove (gn_conn_list_t * const list,
                     gn_conn_t * const conn);

void
gn_free_conn (gn_conn_t ** const conn);

__attribute__((nonnull))
void
gn_get_new_conns (gn_conn_mgmt_thrd_data_t * const data,
                  gn_conn_list_t * const conn_list,
                  const bool stop);

__attribute__((nonnull))
__attribute__((warn_unused_result))
bool
gn_process_conn (gn_conn_t * const conn);

void
gn_sleep_ms (const uint32_t ms);

#endif // GN_CONN_MGMT_THRD_H