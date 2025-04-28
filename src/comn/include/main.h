#ifndef MAIN_H
#define MAIN_H

#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/un.h>
#include <unistd.h>

#include <gn_serv_sock_list_t.h>

#define GN_MIN_CMDL_ARGS 1 // Minimum number of command line arguments.
#define GN_MAX_CMDL_ARGS 8 // Maximum number of command line arguments.

void
gn_mstr_main (int ipc_sock,
              gn_serv_sock_list_t * const serv_sock_list);

void
gn_wrkr_main (int ipc_sock,
              gn_serv_sock_list_t * const serv_sock_list,
              const char * const ipc_addr_str);

void
gn_close_serv_socks (gn_serv_sock_list_t * const serv_sock_list);

#endif // MAIN_H