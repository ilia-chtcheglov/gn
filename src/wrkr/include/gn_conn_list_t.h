#ifndef GN_CONN_LIST_T_H
#define GN_CONN_LIST_T_H

#include <gn_conn_t.h>

typedef struct gn_conn_list_t gn_conn_list_t;

struct gn_conn_list_t
{
    gn_conn_t * head;
    gn_conn_t * tail;
    uint32_t    len;
};

#endif // GN_CONN_LIST_T_H