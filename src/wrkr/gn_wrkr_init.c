#include <gn_wrkr_init.h>

__attribute__((nonnull))
__attribute__((warn_unused_result))
bool
gn_wrkr_init (int ipc_sock,
              gn_serv_sock_list_t * const serv_sock_list,
              const char * const ipc_addr_str)
{
    signal (SIGINT, SIG_IGN);
    signal (SIGPIPE, SIG_IGN);

    // Connect to the master process.
    if (gn_ipc_conn (ipc_sock, ipc_addr_str) != EXIT_SUCCESS) return EXIT_FAILURE;

    // Create an epoll instance for server sockets.
    int repoll_create1 = epoll_create1 (EPOLL_CLOEXEC);
    if (repoll_create1 < 0)
    {
        fprintf (stderr, "Failed to create epoll instance. %s.\n", strerror (errno));
        return EXIT_FAILURE;
    }

    int this_ret = EXIT_SUCCESS;
    // Receive server sockets data.
    if (gn_recv_serv_socks (ipc_sock, repoll_create1, serv_sock_list) != 1)
    {
        fprintf (stderr, "Error occured while receiving server sockets data.\n");
        this_ret = EXIT_FAILURE;
        goto labl_gn_recv_serv_socks_failed;
    }

    // List of structures, one for each connection management thread.
    gn_conn_mgmt_thrd_data_list_t conn_mgmt_thrd_data_list = { 0 };

    // Start connection management threads.
    gn_start_conn_mgmt_thrds (1, &conn_mgmt_thrd_data_list);

    // List of structures, one for each connection acceptance thread.
    gn_conn_acpt_thrd_data_list_t conn_acpt_thrd_data_list = { 0 };

    // Start connection acceptance threads.
    gn_start_conn_acpt_thrds (1, &conn_acpt_thrd_data_list, repoll_create1, &conn_mgmt_thrd_data_list);

    gn_wrkr_main (ipc_sock);

    gn_stop_conn_acpt_thrds (&conn_acpt_thrd_data_list);
    gn_stop_conn_mgmt_thrds (&conn_mgmt_thrd_data_list);

    labl_gn_recv_serv_socks_failed:
    // Close the epoll instance created for server sockets.
    gn_close (&repoll_create1);

    return this_ret;
}