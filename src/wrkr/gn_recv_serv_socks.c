#include <gn_recv_serv_socks.h>

__attribute__((warn_unused_result))
int
gn_recv_serv_socks (const int ipc_sock, const int repoll_create1, gn_serv_sock_list_t * const serv_sock_list)
{
    int rgn_recv_serv_sock = 0;
    while (rgn_recv_serv_sock == 0)
    {
        rgn_recv_serv_sock = gn_recv_serv_sock (ipc_sock, repoll_create1, serv_sock_list);
    }

    return rgn_recv_serv_sock;
}