#ifndef MAIN_H
#define MAIN_H

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#include <gn_serv_sock_list_t.h>

__attribute__((warn_unused_result))
bool
gn_chck_cmdl_args (const int argc,
                   const char * const * const argv);

int
gn_close (int * const fd);

void
gn_mstr_init (int ipc_sock,
              gn_serv_sock_list_t * const serv_sock_list);

__attribute__((nonnull))
__attribute__((warn_unused_result))
bool
gn_prse_cmdl_args (const int argc,
                   const char * const * const argv,
                   const char ** const ipc_addr_str);

void
gn_wrkr_init (int ipc_sock,
              gn_serv_sock_list_t * const serv_sock_list,
              const char * const ipc_addr_str);

void
gn_close_serv_socks (gn_serv_sock_list_t * const serv_sock_list);

#endif // MAIN_H