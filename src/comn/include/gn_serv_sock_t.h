#ifndef GN_SERV_SOCK_T_H
#define GN_SERV_SOCK_T_H

typedef struct gn_serv_sock_t gn_serv_sock_t;

struct gn_serv_sock_t
{
    int              fd;
    char *           addr;
    uint16_t         port;
    gn_serv_sock_t * prev;
    gn_serv_sock_t * next;
};

#endif // GN_SERV_SOCK_T_H