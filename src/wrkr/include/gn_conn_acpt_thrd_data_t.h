#ifndef GN_CONN_ACPT_THRD_DATA_T_H
#define GN_CONN_ACPT_THRD_DATA_T_H

#include <pthread.h>

#include <gn_conn_mgmt_thrd_data_list_t.h>
#include <gn_conn_t.h>

typedef struct gn_conn_acpt_thrd_data_t gn_conn_acpt_thrd_data_t;

struct gn_conn_acpt_thrd_data_t
{
    pthread_t                       tid;
    int                             repoll_create1;
    gn_conn_mgmt_thrd_data_list_t * conn_mgmt_thrd_data_list;
    gn_conn_acpt_thrd_data_t *      prev;
    gn_conn_acpt_thrd_data_t *      next;
};

#endif // GN_CONN_ACPT_THRD_DATA_T_H