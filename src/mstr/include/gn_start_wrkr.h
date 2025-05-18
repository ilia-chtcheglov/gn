#ifndef GN_START_WRKR_H
#define GN_START_WRKR_H

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <unistd.h>

#include <gn_serv_sock_list_t.h>
#include <gn_wrkr_data_t.h>

__attribute__((warn_unused_result))
bool
gn_start_wrkr (gn_wrkr_data_t * wrkr_data, const int repoll_create1, const char * const path, int ipc_sock,
               const char * const ipc_addr_str, gn_serv_sock_list_t * const list);

__attribute__((warn_unused_result))
int
gn_ipc_acpt (const int ipc_sock);

void
gn_send_serv_sock (const int ipc_sock, const gn_serv_sock_t * const serv_sock);

#endif // GN_START_WRKR_H