#ifndef GN_MSTR_MAIN_H
#define GN_MSTR_MAIN_H

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>

#include <gn_serv_sock_list_t.h>
#include <gn_wrkr_data_list_t.h>

extern bool sigint_rcvd;

void
gn_mstr_main (gn_wrkr_data_list_t * const wrkr_data_list, int * const repoll_create1, const char * const self_path,
              int ipc_sock, const char * const ipc_addr_str, gn_serv_sock_list_t * const serv_sock_list);

int
gn_close (int * const fd);

__attribute__((warn_unused_result))
bool
gn_start_wrkr (gn_wrkr_data_t * wrkr_data, const int repoll_create1, const char * const path, int ipc_sock,
               const char * const ipc_addr_str, gn_serv_sock_list_t * const serv_sock_list);

void
gn_wrkr_data_list_remove (gn_wrkr_data_list_t * const list,
                          const gn_wrkr_data_t * const data);

#endif // GN_MSTR_MAIN_H