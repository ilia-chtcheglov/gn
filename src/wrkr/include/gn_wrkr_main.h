#ifndef GN_WRKR_MAIN_H
#define GN_WRKR_MAIN_H

#include <errno.h>
#include <poll.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <unistd.h>

#include <gn_serv_sock_list_t.h>

void
gn_wrkr_main (int ipc_sock, gn_serv_sock_list_t * const serv_sock_list, const char * const ipc_addr_str);

__attribute__((warn_unused_result))
int
gn_ipc_conn (const int ipc_sock, const char * const ipc_addr_str);

__attribute__((warn_unused_result))
int
gn_recv_serv_socks (const int ipc_sock, const int repoll_create1, gn_serv_sock_list_t * const serv_sock_list);

#endif // GN_WRKR_MAIN_H