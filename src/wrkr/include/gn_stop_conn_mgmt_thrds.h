#ifndef GN_STOP_CONN_MGMT_THRDS_H
#define GN_STOP_CONN_MGMT_THRDS_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <gn_conn_mgmt_thrd_data_list_t.h>

#include <gn_conn_mgmt_thrd_state_e.h>

void
gn_stop_conn_mgmt_thrds (gn_conn_mgmt_thrd_data_list_t * const list);

void
gn_conn_mgmt_thrd_data_list_remove (gn_conn_mgmt_thrd_data_list_t * const list,
                                    gn_conn_mgmt_thrd_data_t * const data);

#endif // GN_STOP_CONN_MGMT_THRDS_H