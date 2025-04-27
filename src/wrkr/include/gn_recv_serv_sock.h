#ifndef GN_RECV_SERV_SOCK_H
#define GN_RECV_SERV_SOCK_H

#include <arpa/inet.h>
#include <errno.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <gn_serv_sock_list_t.h>

__attribute__((warn_unused_result))
int
gn_recv_serv_sock (const int ipc_sock, gn_serv_sock_list_t * const serv_sock_list);

int
gn_serv_sock_list_push_back (gn_serv_sock_list_t * const list, gn_serv_sock_t * const sock);

#endif // GN_RECV_SERV_SOCK_H