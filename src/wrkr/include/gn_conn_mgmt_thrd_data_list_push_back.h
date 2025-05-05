#ifndef GN_CONN_MGMT_THRD_DATA_LIST_PUSH_BACK_H
#define GN_CONN_MGMT_THRD_DATA_LIST_PUSH_BACK_H

#include <gn_conn_mgmt_thrd_data_list_t.h>

__attribute__((warn_unused_result))
int
gn_conn_mgmt_thrd_data_list_push_back (gn_conn_mgmt_thrd_data_list_t * const list,
                                       gn_conn_mgmt_thrd_data_t * const data);

#endif // GN_CONN_MGMT_THRD_DATA_LIST_PUSH_BACK_H