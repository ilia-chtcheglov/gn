#ifndef GN_MSTR_MAIN_H
#define GN_MSTR_MAIN_H

#include <arpa/inet.h>
#include <errno.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <sys/un.h>
#include <time.h>
#include <unistd.h>

#include <gn_mstr_conf_t.h>
#include <gn_serv_sock_list_t.h>
#include <gn_wrkr_data_list_t.h>

void
gn_mstr_main (int ipc_sock, gn_serv_sock_list_t * const serv_sock_list);

int
gn_close (int * const fd);

void
gn_load_mstr_conf (gn_mstr_conf_t * const mstr_conf);

__attribute__((warn_unused_result))
int
gn_open_serv_sock (gn_serv_sock_list_t * const list, const char * const addr, const uint16_t port);

__attribute__((warn_unused_result))
bool
gn_start_wrkr (gn_wrkr_data_t * wrkr_data, const int repoll_create1, const char * const path, int ipc_sock,
               const char * const ipc_addr_str, gn_serv_sock_list_t * const serv_sock_list);

void
gn_start_wrkrs (gn_wrkr_data_list_t * const wrkr_data_list, const int repoll_create1,
                const uint8_t num_workers, const char * const path, int ipc_sock,
                const char * const ipc_addr_str, gn_serv_sock_list_t * const serv_sock_list);

void
gn_stop_wrkrs (gn_wrkr_data_list_t * const list);

void
gn_wrkr_data_list_remove (gn_wrkr_data_list_t * const list,
                          const gn_wrkr_data_t * const data);

#endif // GN_MSTR_MAIN_H