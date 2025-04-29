#include <gn_wrkr_main.h>

void
gn_wrkr_main (int ipc_sock, gn_serv_sock_list_t * const serv_sock_list, const char * const ipc_addr_str)
{
    printf ("[%i] Worker started. Received IPC address \"%s\".\n", getpid (), ipc_addr_str);

    // Connect to the master process.
    if (gn_ipc_conn (ipc_sock, ipc_addr_str) != EXIT_SUCCESS) return;

    // Receive server sockets data.
    int rgn_recv_serv_sock = 0;
    while (rgn_recv_serv_sock == 0)
    {
        rgn_recv_serv_sock = gn_recv_serv_sock (ipc_sock, serv_sock_list);
    }

    if (rgn_recv_serv_sock != 1)
    {
        fprintf (stderr, "Error occured while receiving server socket data.\n");
        return;
    }

    // Test code.
    const gn_serv_sock_t * serv_sock = serv_sock_list->head;
    for (uint16_t i = 0; i < serv_sock_list->len; serv_sock = serv_sock->next, i++)
    {
        printf ("Received server socket .fd: %i, .addr: [%s], .port: %i.\n",
                serv_sock->fd, serv_sock->addr, serv_sock->port);
    }

    // Main loop.
    while (true)
    {
        sleep (1);
    }
}