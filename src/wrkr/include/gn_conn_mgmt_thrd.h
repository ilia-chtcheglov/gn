#ifndef GN_CONN_MGMT_THRD_H
#define GN_CONN_MGMT_THRD_H

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <gn_conn_list_t.h>
#include <gn_conn_mgmt_thrd_data_t.h>

#include <gn_conn_mgmt_thrd_state_e.h>

void *
gn_conn_mgmt_thrd (void * const p);

int
gn_close (int * const fd);

#endif // GN_CONN_MGMT_THRD_H