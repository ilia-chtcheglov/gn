#ifndef GN_WRKR_INIT_H
#define GN_WRKR_INIT_H

#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>

#include <gn_conn_acpt_thrd_data_list_t.h>
#include <gn_conn_mgmt_thrd_data_list_t.h>
#include <gn_serv_sock_list_t.h>

void
gn_wrkr_init (int ipc_sock, gn_serv_sock_list_t * const serv_sock_list, const char * const ipc_addr_str);

int
gn_close (int * const fd);

__attribute__((warn_unused_result))
int
gn_ipc_conn (const int ipc_sock, const char * const ipc_addr_str);

__attribute__((warn_unused_result))
int
gn_recv_serv_socks (const int ipc_sock, const int repoll_create1, gn_serv_sock_list_t * const serv_sock_list);

void
gn_start_conn_acpt_thrds (const uint8_t num,
                          gn_conn_acpt_thrd_data_list_t * const conn_acpt_thrd_data_list,
                          const int repoll_create1,
                          gn_conn_mgmt_thrd_data_list_t * const conn_mgmt_thrd_data_list);

void
gn_start_conn_mgmt_thrds (const uint8_t num,
                          gn_conn_mgmt_thrd_data_list_t * const conn_mgmt_thrd_data_list);

void
gn_stop_conn_acpt_thrds (gn_conn_acpt_thrd_data_list_t * const list);

void
gn_stop_conn_mgmt_thrds (gn_conn_mgmt_thrd_data_list_t * const list);

void
gn_wrkr_main (const int ipc_sock);

#endif // GN_WRKR_INIT_H