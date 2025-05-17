#include <gn_conn_acpt_thrd.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

__attribute__((warn_unused_result))
int
gn_acpt_conns (int * const repoll_create1, gn_conn_mgmt_thrd_data_list_t * const list);

void *
gn_conn_acpt_thrd (void * const p)
{
    gn_conn_acpt_thrd_data_t * const data = (gn_conn_acpt_thrd_data_t *)p;
    atomic_store_explicit (&data->state, GN_CONN_ACPT_THRD_STATE_RUNNING, memory_order_relaxed);

    gn_conn_mgmt_thrd_data_list_t * const conn_mgmt_thrd_data_list = data->conn_mgmt_thrd_data_list;
    int repoll_create1 = data->repoll_create1;

    bool main_loop = true;
    while (main_loop)
    {
        if (atomic_load_explicit (&data->stop, memory_order_relaxed))
        {
            printf ("Connection acceptance thread %lu stopping.\n", data->tid);
            atomic_store_explicit (&data->state, GN_CONN_ACPT_THRD_STATE_STOPPING, memory_order_relaxed);
            main_loop = false;
            break;
        }

        if (gn_acpt_conns (&repoll_create1, conn_mgmt_thrd_data_list) != EXIT_SUCCESS) main_loop = false;
    }

    atomic_store_explicit (&data->state, GN_CONN_ACPT_THRD_STATE_STOPPED, memory_order_relaxed);
    return NULL;
}