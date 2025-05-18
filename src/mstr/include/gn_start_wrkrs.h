#ifndef GN_START_WRKRS_H
#define GN_START_WRKRS_H

#include <stdio.h>
#include <stdlib.h>

#include <gn_serv_sock_list_t.h>
#include <gn_wrkr_data_list_t.h>

void
gn_start_wrkrs (gn_wrkr_data_list_t * const wrkr_data_list, const uint8_t num_workers, const char * const path,
                int ipc_sock, const char * const ipc_addr_str, gn_serv_sock_list_t * const serv_sock_list);

void
gn_start_wrkr (gn_wrkr_data_t * wrkr_data, const char * const path, int ipc_sock,
               const char * const ipc_addr_str, gn_serv_sock_list_t * const serv_sock_list);

#endif // GN_START_WRKRS_H