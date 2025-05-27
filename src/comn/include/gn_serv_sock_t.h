#ifndef GN_SERV_SOCK_T_H
#define GN_SERV_SOCK_T_H

#include <stdint.h>

typedef struct gn_serv_sock_t gn_serv_sock_t;

struct gn_serv_sock_t
{
    gn_serv_sock_t * prev;
    gn_serv_sock_t * next;
    char *           addr;
    int              fd;
    uint16_t         port;
    uint16_t         _pad;
};

#endif // GN_SERV_SOCK_T_H