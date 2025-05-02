#include <gn_wrkr_main.h>

__attribute__((warn_unused_result))
int
gn_conn_mgmt_thrd_data_list_push_back (gn_conn_mgmt_thrd_data_list_t * const list,
                                       gn_conn_mgmt_thrd_data_t * const data);

__attribute__((warn_unused_result))
int
gn_conn_mgmt_thrd_data_list_push_back (gn_conn_mgmt_thrd_data_list_t * const list,
                                       gn_conn_mgmt_thrd_data_t * const data)
{
    switch (list->len)
    {
        case 0:
        {
            list->head = list->tail = data->prev = data->next = data;
            break;
        }
        case UINT8_MAX:
        {
            return 1;
        }
        default:
        {
            list->tail->next = data;
            data->prev = list->tail;
            list->head->prev = data;
            data->next = list->head;
            list->tail = data;
        }
    }

    list->len++;
    return 0;
}

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
    for (uint8_t i = 0; i < 2; i++)
    {
        gn_conn_mgmt_thrd_data_t * data = (gn_conn_mgmt_thrd_data_t *)malloc (sizeof (gn_conn_mgmt_thrd_data_t));
        if (data != NULL) memset (data, 0, sizeof (gn_conn_mgmt_thrd_data_t));
        else
        {
            fprintf (stderr, "Failed to allocate structure for connection management thread data.\n");
            continue;
        }


        const int rpthread_create = pthread_create (&data->tid, NULL, gn_conn_mgmt_thrd, data);
        switch (rpthread_create)
        {
            case 0:
            {
                // TODO: Don't ignore returned value.
                (void)! gn_conn_mgmt_thrd_data_list_push_back (&conn_mgmt_thrd_data_list, data);
                break;
            }
            case EAGAIN:
            case EINVAL:
            case EPERM:
            {
                fprintf (stderr, "Failed to create connection management thread. %s.\n", strerror (rpthread_create));
                // TODO: Stop process on EINVAL or EPERM.
                break;
            }
            default:
            {
                fprintf (stderr, "pthread_create() returned unexpected value %i.\n", rpthread_create);
                // TODO: Stop process.
            }
        }
    }

    // Main loop.
    bool main_loop = true;
    while (main_loop)
    {
        if (gn_acpt_conns (&repoll_create1) != EXIT_SUCCESS) main_loop = false;
    }

    // TODO: Stop connection management threads.
    // TODO: Free connection management thread data structures.

    // Close the epoll instance created for server sockets.
    close (repoll_create1);
}