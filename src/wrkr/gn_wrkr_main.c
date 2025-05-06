#include <gn_wrkr_main.h>

void
gn_wrkr_main (int ipc_sock, gn_serv_sock_list_t * const serv_sock_list, const char * const ipc_addr_str)
{
    // Connect to the master process.
    if (gn_ipc_conn (ipc_sock, ipc_addr_str) != EXIT_SUCCESS) return;

    // Create an epoll instance for server sockets.
    int repoll_create1 = epoll_create1 (EPOLL_CLOEXEC);
    if (repoll_create1 < 0)
    {
        fprintf (stderr, "Failed to create epoll instance. %s.\n", strerror (errno));
        return;
    }

    // Receive server sockets data.
    if (gn_recv_serv_socks (ipc_sock, repoll_create1, serv_sock_list) != 1)
    {
        fprintf (stderr, "Error occured while receiving server sockets data.\n");
        return;
    }

    // List of structures, one for each connection management thread.
    gn_conn_mgmt_thrd_data_list_t conn_mgmt_thrd_data_list;
    memset (&conn_mgmt_thrd_data_list, 0, sizeof (conn_mgmt_thrd_data_list));

    // Start connection management threads.
    gn_start_conn_mgmt_thrds (1, &conn_mgmt_thrd_data_list);

    // List of structures, one for each connection acceptance thread.
    gn_conn_acpt_thrd_data_list_t conn_acpt_thrd_data_list;
    memset (&conn_acpt_thrd_data_list, 0, sizeof (conn_acpt_thrd_data_list));

    // Start connection acceptance threads.
    gn_start_conn_acpt_thrds (1, &conn_acpt_thrd_data_list);

    // Main loop.
    bool main_loop = true;
    while (main_loop)
    {
        if (gn_acpt_conns (&repoll_create1, &conn_mgmt_thrd_data_list) != EXIT_SUCCESS) main_loop = false;
    }

    // TODO: Stop connection acceptance/management threads.
    // TODO: Free connection acceptance/management thread data structures.

    // Close the epoll instance created for server sockets.
    close (repoll_create1);
}