#ifndef GN_SERV_SOCK_LIST_T_H
#define GN_SERV_SOCK_LIST_T_H

#include <gn_serv_sock_t.h>

typedef uint16_t gn_serv_sock_list_len_t;
#define GN_SERV_SOCK_LIST_LEN_MAX UINT16_MAX

typedef struct
{
    gn_serv_sock_t *        head;
    gn_serv_sock_t *        tail;
    gn_serv_sock_list_len_t len;
} gn_serv_sock_list_t;

#endif // GN_SERV_SOCK_LIST_T_H