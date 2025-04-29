#ifndef GN_WRKR_MAIN_H
#define GN_WRKR_MAIN_H

#include <poll.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <gn_serv_sock_list_t.h>

void
gn_wrkr_main (int ipc_sock, gn_serv_sock_list_t * const serv_sock_list, const char * const ipc_addr_str);

__attribute__((warn_unused_result))
int
gn_ipc_conn (const int ipc_sock, const char * const ipc_addr_str);

__attribute__((warn_unused_result))
int
gn_recv_serv_sock (const int ipc_sock, gn_serv_sock_list_t * const serv_sock_list);

#endif // GN_WRKR_MAIN_H