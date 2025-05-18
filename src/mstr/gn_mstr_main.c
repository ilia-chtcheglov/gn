#include <gn_mstr_main.h>

void
gn_mstr_main (gn_wrkr_data_list_t * const wrkr_data_list, int * const repoll_create1, const char * const self_path,
              int ipc_sock, const char * const ipc_addr_str, gn_serv_sock_list_t * const serv_sock_list)
{
    // Main loop.
    bool main_loop = true;
    while (main_loop)
    {
        if (sigint_rcvd) break;
        
        struct epoll_event epoll_evts[UINT8_MAX];
        memset (epoll_evts, 0, sizeof (epoll_evts));

        const int repoll_wait = epoll_wait (*repoll_create1, epoll_evts,
                                            sizeof (epoll_evts) / sizeof (struct epoll_event), 1000);
        switch (repoll_wait)
        {
            case 0:
            {
                // Timeout.
                break;
            }
            case -1:
            {
                switch (errno)
                {
                    case EINTR:
                        break;
                    case EBADF:
                    case EINVAL:
                    {
                        *repoll_create1 = -1;
                        __attribute__((fallthrough));
                    }
                    case EFAULT:
                    {
                        fprintf (stderr, "epoll_wait() failed. %s.\n", strerror (errno));
                        main_loop = false;
                        break;
                    }
                    default:
                    {
                        fprintf (stderr, "epoll_wait() returned undocumented errno code %i.\n", errno);
                        main_loop = false;
                        break;
                    }
                }
                break;
            }
            default:
            {
                if (repoll_wait < -1)
                {
                    fprintf (stderr, "epoll_wait() returned unexpected value %i.\n", repoll_wait);
                    main_loop = false;
                    break;
                }

                for (uint8_t i = 0; i < repoll_wait; i++)
                {
                    gn_wrkr_data_t * wrkr_data = (gn_wrkr_data_t *)epoll_evts[i].data.ptr;
                    // Detect loss of IPC connection with the worker process.
                    if (epoll_evts[i].events & EPOLLHUP)
                    {
                        printf ("Worker %i disconnected.\n", wrkr_data->pid);

                        // Remove the worker IPC socket from the epoll instance.
                        const int repoll_ctl = epoll_ctl (*repoll_create1, EPOLL_CTL_DEL, wrkr_data->ipc_sock, NULL);
                        switch (repoll_ctl)
                        {
                            case 0:
                            {
                                // Close the worker IPC socket.
                                gn_close (&wrkr_data->ipc_sock);
                                wrkr_data->pid = -1;
                                // Don't change wrkr_data->prev/next.

                                // Start a new worker process.
                                if (gn_start_wrkr (wrkr_data, *repoll_create1, self_path,
                                                   ipc_sock, ipc_addr_str, serv_sock_list) != EXIT_SUCCESS)
                                {
                                    // Remove the worker data structure from the list.
                                    gn_wrkr_data_list_remove (wrkr_data_list, wrkr_data);
                                    free (wrkr_data);
                                    wrkr_data = NULL;
                                }
                                break;
                            }
                            case -1:
                            {
                                fprintf (stderr, "Failed to remove worker IPC socket from epoll instance. %s.\n",
                                         strerror (errno));
                                // TODO: Check errno.
                                break;
                            }
                            default:
                            {
                                fprintf (stderr, "epoll_ctl() returned unexpected value %i.\n", repoll_ctl);
                            }
                        }
                    }
                }
            }
        }
    }
}