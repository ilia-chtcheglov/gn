#ifndef GN_START_WRKRS_H
#define GN_START_WRKRS_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <gn_serv_sock_list_t.h>
#include <gn_wrkr_data_list_t.h>

void
gn_start_wrkrs (gn_wrkr_data_list_t * const wrkr_data_list, const int repoll_create1,
                const uint8_t num_workers, const char * const path, int ipc_sock,
                const char * const ipc_addr_str, gn_serv_sock_list_t * const serv_sock_list);

__attribute__((warn_unused_result))
bool
gn_start_wrkr (gn_wrkr_data_t * wrkr_data, const int repoll_create1, const char * const path, int ipc_sock,
               const char * const ipc_addr_str, gn_serv_sock_list_t * const serv_sock_list);

__attribute__((warn_unused_result))
bool
gn_wrkr_data_list_push_back (gn_wrkr_data_list_t * const list, gn_wrkr_data_t * const data);

#endif // GN_START_WRKRS_H