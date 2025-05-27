#include <gn_close_serv_socks.h>

__attribute__((nonnull))
void
gn_close_serv_socks (gn_serv_sock_list_t * const serv_sock_list)
{
    while (serv_sock_list->len > 0)
    {
        gn_serv_sock_t * const serv_sock = gn_serv_sock_list_pop (serv_sock_list);
        gn_close (&serv_sock->fd);
        free (serv_sock->addr);
        free (serv_sock);
    }
}