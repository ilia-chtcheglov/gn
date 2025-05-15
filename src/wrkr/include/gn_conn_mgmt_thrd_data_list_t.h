#ifndef GN_CONN_MGMT_THRD_DATA_LIST_T_H
#define GN_CONN_MGMT_THRD_DATA_LIST_T_H

#include <gn_conn_mgmt_thrd_data_t.h>

typedef struct
{
    gn_conn_mgmt_thrd_data_t * head;
    gn_conn_mgmt_thrd_data_t * tail;
    uint8_t                    len;
} gn_conn_mgmt_thrd_data_list_t;

#endif // GN_CONN_MGMT_THRD_DATA_LIST_T_H