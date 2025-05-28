#ifndef MAIN_H
#define MAIN_H

#include <gn_serv_sock_list_t.h>

#include <stdbool.h>
#include <stdlib.h>

__attribute__((nonnull))
__attribute__((warn_unused_result))
bool
gn_chck_cmdl_args (const int argc,
                   const char * const * const argv);

__attribute__((nonnull))
void
gn_close_serv_socks (gn_serv_sock_list_t * const serv_sock_list);

__attribute__((nonnull))
void
gn_ipc_close (int * const ipc_sock);

__attribute__((warn_unused_result))
int
gn_ipc_open (void);

__attribute__((nonnull))
__attribute__((warn_unused_result))
bool
gn_mstr_init (int ipc_sock,
              gn_serv_sock_list_t * const serv_sock_list);

__attribute__((nonnull))
__attribute__((warn_unused_result))
bool
gn_prse_cmdl_args (const int argc,
                   const char * const * const argv,
                   const char ** const ipc_addr_str);

__attribute__((nonnull))
void
gn_wrkr_init (int ipc_sock,
              gn_serv_sock_list_t * const serv_sock_list,
              const char * const ipc_addr_str);

#endif // MAIN_H