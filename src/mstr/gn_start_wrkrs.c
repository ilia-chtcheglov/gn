#include <gn_start_wrkrs.h>

void
gn_start_wrkrs (const uint8_t num_workers, const char * const path, int ipc_sock,
                const char * const ipc_addr_str, gn_serv_sock_list_t * const list)
{
    for (uint8_t i = 0; i < num_workers; i++)
    {
        gn_start_wrkr (path, ipc_sock, ipc_addr_str, list);
    }
}