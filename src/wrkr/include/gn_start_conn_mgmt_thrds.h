#ifndef GN_START_CONN_MGMT_THRDS_H
#define GN_START_CONN_MGMT_THRDS_H

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <gn_conn_mgmt_thrd_data_list_t.h>

#include <gn_conn_mgmt_thrd_state_e.h>

void
gn_start_conn_mgmt_thrds (const uint8_t num, gn_conn_mgmt_thrd_data_list_t * const conn_mgmt_thrd_data_list);

__attribute__((warn_unused_result))
int
gn_conn_mgmt_thrd_data_list_push_back (gn_conn_mgmt_thrd_data_list_t * const list,
                                       gn_conn_mgmt_thrd_data_t * const data);

void *
gn_conn_mgmt_thrd (void * const p);

#endif // GN_START_CONN_MGMT_THRDS_H