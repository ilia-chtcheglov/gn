#ifndef GN_CONN_MGMT_THRD_DATA_T_H
#define GN_CONN_MGMT_THRD_DATA_T_H

#include <pthread.h>
#include <stdatomic.h>

#include <gn_conn_t.h>

typedef struct gn_conn_mgmt_thrd_data_t gn_conn_mgmt_thrd_data_t;

struct gn_conn_mgmt_thrd_data_t
{
    pthread_t                  tid;
    atomic_uintptr_t           new_conns[127];
    gn_conn_mgmt_thrd_data_t * prev;
    gn_conn_mgmt_thrd_data_t * next;
};

#endif // GN_CONN_MGMT_THRD_DATA_T_H