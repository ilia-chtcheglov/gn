#ifndef GN_SERV_SOCK_LIST_T_H
#define GN_SERV_SOCK_LIST_T_H

#include <gn_serv_sock_t.h>

typedef struct
{
    gn_serv_sock_t * head;
    gn_serv_sock_t * tail;
    uint16_t         len;
} gn_serv_sock_list_t;

#endif // GN_SERV_SOCK_LIST_T_H