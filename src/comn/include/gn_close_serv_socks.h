#ifndef GN_CLOSE_SERV_SOCKS_H
#define GN_CLOSE_SERV_SOCKS_H

#include <stdlib.h>
#include <unistd.h>

#include <gn_serv_sock_list_t.h>

void
gn_close_serv_socks (gn_serv_sock_list_t * const serv_sock_list);

int
gn_close (int * const fd);

gn_serv_sock_t *
gn_serv_sock_list_pop (gn_serv_sock_list_t * const serv_sock_list);

#endif // GN_CLOSE_SERV_SOCKS_H