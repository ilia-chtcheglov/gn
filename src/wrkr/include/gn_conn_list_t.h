#ifndef GN_CONN_LIST_T_H
#define GN_CONN_LIST_T_H

#include <gn_conn_t.h>

typedef uint32_t gn_conn_list_len_t;
#define GN_CONN_LIST_LEN_MAX UINT32_MAX

typedef struct
{
    gn_conn_t *        head;
    gn_conn_t *        tail;
    gn_conn_list_len_t len;
} gn_conn_list_t;

#endif // GN_CONN_LIST_T_H