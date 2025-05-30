#include <gn_start_wrkr.h>

__attribute__((warn_unused_result))
bool
gn_start_wrkr (gn_wrkr_data_t * wrkr_data, const int repoll_create1, const char * const path, int ipc_sock,
               const char * const ipc_addr_str, gn_serv_sock_list_t * const serv_sock_list)
{
    printf ("Starting worker process.\n");
    // Fork the master process to start a worker process.
    pid_t rfork = fork ();
    switch (rfork)
    {
        case 0: // Child.
        {
            // Start a worker process.
            char ** argv = (char **)malloc (4 * sizeof (char *));
            if (argv != NULL)
            {
                // TODO: If malloc() for, for example, argv[0] fails other mallocs mustn't be called.
                argv[0] = (char *)malloc (strlen (path) + 1);
                if (argv[0] != NULL) strcpy (argv[0], path);
                argv[1] = (char *)malloc (strlen ("--ipc") + 1);
                if (argv[1] != NULL) strcpy (argv[1], "--ipc");
                argv[2] = (char *)malloc (strlen (ipc_addr_str) + 1);
                if (argv[2] != NULL) strcpy (argv[2], ipc_addr_str);
                argv[3] = NULL;

                execv (path, argv);
                fprintf (stderr, "Failed to start worker process. %s.\n", strerror (errno));
            }

            exit (1);
        }
        case -1:
        {
            fprintf (stderr, "Failed to fork master process. %s.\n", strerror (errno));
            break;
        }
        default: // Parent.
        {
            const int raccept4 = gn_ipc_acpt (ipc_sock);
            if (raccept4 < 0) break;

            wrkr_data->ipc_sock = raccept4;
            wrkr_data->pid = rfork;

            // Add worker IPC socket to epoll instance.
            struct epoll_event epoll_evt;
            memset (&epoll_evt, 0, sizeof (epoll_evt));
            epoll_evt.events = EPOLLIN | EPOLLRDHUP;
            epoll_evt.data.ptr = wrkr_data;

            const int repoll_ctl = epoll_ctl (repoll_create1, EPOLL_CTL_ADD, raccept4, &epoll_evt);
            if (repoll_ctl == 0)
            {
                // Send configuration, server sockets, etc. to the worker process.
                gn_serv_sock_t * serv_sock = serv_sock_list->head;
                for (size_t i = 0; i < serv_sock_list->len; serv_sock = serv_sock->next, i++)
                {
                    gn_send_serv_sock (raccept4, serv_sock);
                }

                gn_serv_sock_t final_serv_sock = {
                    .fd = 0,
                    .addr = NULL,
                    .port = 0
                };
                gn_send_serv_sock (raccept4, &final_serv_sock);

                // TODO: Remove IPC socket from epoll instance on error and return EXIT_FAILURE.
                return EXIT_SUCCESS;
            }
            else
            {
                fprintf (stderr, "Failed to add worker IPC socket to epoll instance. %s.\n", strerror (errno));
            }
        }
    }

    return EXIT_FAILURE;
}