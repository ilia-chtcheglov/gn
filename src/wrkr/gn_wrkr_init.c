#include <gn_wrkr_init.h>

void
gn_wrkr_init (int ipc_sock, gn_serv_sock_list_t * const serv_sock_list, const char * const ipc_addr_str)
{
    signal (SIGINT, SIG_IGN);

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
        goto labl_gn_recv_serv_socks_failed;
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
    gn_start_conn_acpt_thrds (1, &conn_acpt_thrd_data_list, repoll_create1, &conn_mgmt_thrd_data_list);

    struct pollfd pfd = {
        .fd = ipc_sock,
        .events = POLLIN,
        .revents = 0
    };
    // Main loop.
    bool main_loop = true;
    while (main_loop)
    {
        const int rpoll = poll (&pfd, 1, 1000);
        switch (rpoll)
        {
            case 1:
            {
                if (pfd.revents & POLLHUP)
                {
                    printf ("IPC connection closed.\n");
                    main_loop = false;
                }
                break;
            }
            case 0:
            {
                break;
            }
            case -1:
            {
                switch (errno)
                {
                    case EINTR:
                    case ENOMEM:
                        break;
                    default:
                        fprintf (stderr, "Failed to poll IPC socket. %s.\n", strerror (errno));
                }
                break;
            }
            default:
            {
                
            }
        }
    }

    printf ("Stopping connection acceptance threads.\n");
    gn_stop_conn_acpt_thrds (&conn_acpt_thrd_data_list);
    printf ("Stopping connection management threads.\n");
    gn_stop_conn_mgmt_thrds (&conn_mgmt_thrd_data_list);

    labl_gn_recv_serv_socks_failed:
    // Close the epoll instance created for server sockets.
    gn_close (&repoll_create1);
}