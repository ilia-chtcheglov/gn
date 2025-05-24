#ifndef GN_MSTR_INIT_H
#define GN_MSTR_INIT_H

#include <arpa/inet.h>
#include <errno.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>

#include <gn_mstr_conf_t.h>
#include <gn_serv_sock_list_t.h>
#include <gn_vhst_conf_list_t.h>
#include <gn_wrkr_data_list_t.h>

void
gn_mstr_init (int ipc_sock, gn_serv_sock_list_t * const serv_sock_list);

int
gn_close (int * const fd);

__attribute__((malloc))
__attribute__((warn_unused_result))
char *
gn_ipc_prep (const int ipc_sock);

void
gn_load_mstr_conf (gn_mstr_conf_t * const mstr_conf);

void
gn_load_vhsts_conf (gn_vhst_conf_list_t * const vhst_conf_list);

void
gn_mstr_main (gn_wrkr_data_list_t * const wrkr_data_list, int * const repoll_create1, const char * const self_path,
              int ipc_sock, const char * const ipc_addr_str, gn_serv_sock_list_t * const serv_sock_list);

__attribute__((warn_unused_result))
int
gn_open_serv_sock (gn_serv_sock_list_t * const list, const char * const addr, const uint16_t port);

__attribute__((warn_unused_result))
char *
gn_self_path (void);

__attribute__((warn_unused_result))
bool
gn_set_sig_hndlr (const int signum, const __sighandler_t hndlr);

void
gn_sigint_handler (const int signum);

void
gn_start_wrkrs (gn_wrkr_data_list_t * const wrkr_data_list, const int repoll_create1,
                const uint8_t num_workers, const char * const path, int ipc_sock,
                const char * const ipc_addr_str, gn_serv_sock_list_t * const serv_sock_list);

void
gn_stop_wrkrs (gn_wrkr_data_list_t * const list);

#endif // GN_MSTR_INIT_H