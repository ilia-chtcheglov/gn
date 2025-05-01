#ifndef GN_RECV_SERV_SOCKS_H
#define GN_RECV_SERV_SOCKS_H

#include <gn_serv_sock_list_t.h>

__attribute__((warn_unused_result))
int
gn_recv_serv_socks (const int ipc_sock, const int repoll_create1, gn_serv_sock_list_t * const serv_sock_list);

__attribute__((warn_unused_result))
int
gn_recv_serv_sock (const int ipc_sock, const int repoll_create1, gn_serv_sock_list_t * const serv_sock_list);

#endif // GN_RECV_SERV_SOCKS_H