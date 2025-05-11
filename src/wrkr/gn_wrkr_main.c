#include <gn_wrkr_main.h>

#include <gn_conn_acpt_thrd_state_e.h>
#include <gn_conn_mgmt_thrd_state_e.h>

void
gn_conn_acpt_thrd_data_list_remove (gn_conn_acpt_thrd_data_list_t * const list,
                                    gn_conn_acpt_thrd_data_t * const data);

void
gn_conn_acpt_thrd_data_list_remove (gn_conn_acpt_thrd_data_list_t * const list,
                                    gn_conn_acpt_thrd_data_t * const data)
{
    switch (list->len)
    {
        case 0:
            return;
        case 1:
        {
            list->head = list->tail = NULL;
            break;
        }
        default:
        {
            data->prev->next = data->next;
            data->next->prev = data->prev;
            if (data == list->head) list->head = data->next;
            else if (data == list->tail) list->tail = data->prev;
        }
    }

    list->len--;
    return;
}

void
gn_stop_conn_acpt_thrds (gn_conn_acpt_thrd_data_list_t * const conn_acpt_thrd_data_list);

void
gn_stop_conn_acpt_thrds (gn_conn_acpt_thrd_data_list_t * const conn_acpt_thrd_data_list)
{
    while (conn_acpt_thrd_data_list->len > 0)
    {
        gn_conn_acpt_thrd_data_t * conn_acpt_thrd_data = conn_acpt_thrd_data_list->head;
        for (uint8_t i = 0; i < conn_acpt_thrd_data_list->len; i++)
        {
            gn_conn_acpt_thrd_state_e state =
                (gn_conn_acpt_thrd_state_e)atomic_load_explicit (&conn_acpt_thrd_data->state, memory_order_relaxed);
            switch (state)
            {
                case GN_CONN_ACPT_THRD_STATE_STARTING:
                case GN_CONN_ACPT_THRD_STATE_STOPPING:
                    break;
                case GN_CONN_ACPT_THRD_STATE_RUNNING:
                {
                    atomic_store_explicit (&conn_acpt_thrd_data->stop, true, memory_order_relaxed);
                    break;
                }
                case GN_CONN_ACPT_THRD_STATE_STOPPED:
                {
                    gn_conn_acpt_thrd_data_list_remove (conn_acpt_thrd_data_list, conn_acpt_thrd_data);
                    gn_conn_acpt_thrd_data_t * next_conn_acpt_thrd_data = NULL;
                    if (conn_acpt_thrd_data_list->len > 0) next_conn_acpt_thrd_data = conn_acpt_thrd_data->next;

                    free (conn_acpt_thrd_data);
                    if (next_conn_acpt_thrd_data != NULL) conn_acpt_thrd_data = next_conn_acpt_thrd_data;
                    else conn_acpt_thrd_data = NULL;

                    i--;
                    break;
                }
                default:
                {
                    fprintf (stderr, "Invalid connection acceptance thread state %u.\n", state);
                }
            }
        }
    }
}

void
gn_conn_mgmt_thrd_data_list_remove (gn_conn_mgmt_thrd_data_list_t * const list,
                                    gn_conn_mgmt_thrd_data_t * const data);

void
gn_conn_mgmt_thrd_data_list_remove (gn_conn_mgmt_thrd_data_list_t * const list,
                                    gn_conn_mgmt_thrd_data_t * const data)
{
    switch (list->len)
    {
        case 0:
            return;
        case 1:
        {
            list->head = list->tail = NULL;
            break;
        }
        default:
        {
            data->prev->next = data->next;
            data->next->prev = data->prev;
            if (data == list->head) list->head = data->next;
            else if (data == list->tail) list->tail = data->prev;
        }
    }

    list->len--;
    return;
}

void
gn_stop_conn_mgmt_thrds (gn_conn_mgmt_thrd_data_list_t * const conn_mgmt_thrd_data_list);

void
gn_stop_conn_mgmt_thrds (gn_conn_mgmt_thrd_data_list_t * const conn_mgmt_thrd_data_list)
{
    while (conn_mgmt_thrd_data_list->len > 0)
    {
        gn_conn_mgmt_thrd_data_t * conn_mgmt_thrd_data = conn_mgmt_thrd_data_list->head;
        for (uint8_t i = 0; i < conn_mgmt_thrd_data_list->len; i++)
        {
            gn_conn_mgmt_thrd_state_e state =
                (gn_conn_mgmt_thrd_state_e)atomic_load_explicit (&conn_mgmt_thrd_data->state, memory_order_relaxed);
            switch (state)
            {
                case GN_CONN_MGMT_THRD_STATE_STARTING:
                case GN_CONN_MGMT_THRD_STATE_STOPPING:
                    break;
                case GN_CONN_MGMT_THRD_STATE_RUNNING:
                {
                    atomic_store_explicit (&conn_mgmt_thrd_data->stop, true, memory_order_relaxed);
                    break;
                }
                case GN_CONN_MGMT_THRD_STATE_STOPPED:
                {
                    gn_conn_mgmt_thrd_data_list_remove (conn_mgmt_thrd_data_list, conn_mgmt_thrd_data);
                    gn_conn_mgmt_thrd_data_t * next_conn_mgmt_thrd_data = NULL;
                    if (conn_mgmt_thrd_data_list->len > 0) next_conn_mgmt_thrd_data = conn_mgmt_thrd_data->next;

                    free (conn_mgmt_thrd_data);
                    if (next_conn_mgmt_thrd_data != NULL) conn_mgmt_thrd_data = next_conn_mgmt_thrd_data;
                    else conn_mgmt_thrd_data = NULL;

                    i--;
                    break;
                }
                default:
                {
                    fprintf (stderr, "Invalid connection management thread state %u.\n", state);
                }
            }
        }
    }
}

void
gn_wrkr_main (int ipc_sock, gn_serv_sock_list_t * const serv_sock_list, const char * const ipc_addr_str)
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

    // Close the epoll instance created for server sockets.
    close (repoll_create1);
}