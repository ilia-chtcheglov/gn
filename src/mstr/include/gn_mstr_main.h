#ifndef GN_MSTR_MAIN_H
#define GN_MSTR_MAIN_H

#include <arpa/inet.h>
#include <errno.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/un.h>
#include <time.h>
#include <unistd.h>

#include <gn_serv_sock_list_t.h>

void
gn_mstr_main (int ipc_sock, gn_serv_sock_list_t * const serv_sock_list);

__attribute__((warn_unused_result))
int
gn_open_serv_sock (gn_serv_sock_list_t * const list, const char * const addr, const uint16_t port);

void
gn_start_wrkrs (const uint8_t num_workers, const char * const path, int ipc_sock,
                const char * const ipc_addr_str, gn_serv_sock_list_t * const list);

#endif // GN_MSTR_MAIN_H