#ifndef GN_START_WRKRS_H
#define GN_START_WRKRS_H

#include <gn_serv_sock_list_t.h>

void
gn_start_wrkrs (const uint8_t num_workers, const char * const path, int ipc_sock,
                const char * const ipc_addr_str, gn_serv_sock_list_t * const list);

void
gn_start_wrkr (const char * const path, int ipc_sock,
                const char * const ipc_addr_str, gn_serv_sock_list_t * const list);

#endif // GN_START_WRKRS_H