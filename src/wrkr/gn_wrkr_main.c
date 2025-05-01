#include <gn_wrkr_main.h>

#include <arpa/inet.h>

void
gn_wrkr_main (int ipc_sock, gn_serv_sock_list_t * const serv_sock_list, const char * const ipc_addr_str)
{
    printf ("[%i] Worker started. Received IPC address \"%s\".\n", getpid (), ipc_addr_str);

    // Connect to the master process.
    if (gn_ipc_conn (ipc_sock, ipc_addr_str) != EXIT_SUCCESS) return;

    int repoll_create1 = epoll_create1 (EPOLL_CLOEXEC);
    if (repoll_create1 < 0)
    {
        fprintf (stderr, "Failed to create epoll instance. %s.\n", strerror (errno));
        return;
    }

    // Receive server sockets data.
    int rgn_recv_serv_sock = 0;
    while (rgn_recv_serv_sock == 0)
    {
        rgn_recv_serv_sock = gn_recv_serv_sock (ipc_sock, repoll_create1, serv_sock_list);
    }

    if (rgn_recv_serv_sock != 1)
    {
        fprintf (stderr, "Error occured while receiving server socket data.\n");
        return;
    }

    // Test code.
    const gn_serv_sock_t * ssock = serv_sock_list->head;
    for (uint16_t i = 0; i < serv_sock_list->len; ssock = ssock->next, i++)
    {
        printf ("Received server socket .fd: %i, .addr: [%s], .port: %i.\n",
                ssock->fd, ssock->addr, ssock->port);
    }

    // Main loop.
    bool main_loop = true;
    while (main_loop)
    {
        struct epoll_event epoll_evts[64];
        memset (epoll_evts, 0, sizeof (epoll_evts));

        const int repoll_wait = epoll_wait (repoll_create1, epoll_evts,
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
                        repoll_create1 = -1;
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

                for (unsigned int i = 0; i < (unsigned int)repoll_wait; i++)
                {
                    const struct gn_serv_sock_t * const serv_sock = (gn_serv_sock_t *)epoll_evts[i].data.ptr;
                    struct sockaddr_in sin;
                    memset (&sin, 0, sizeof (sin));
                    socklen_t sin_sz = sizeof (sin);

                    int raccept4 = accept4 (serv_sock->fd, &sin, &sin_sz, SOCK_CLOEXEC | SOCK_NONBLOCK);
                    if (raccept4 > -1)
                    {
                        char saddr[INET_ADDRSTRLEN];
                        memset (saddr, 0, sizeof (saddr));

                        const char * const rinet_ntop = inet_ntop (AF_INET, &sin.sin_addr, saddr, sizeof (saddr));
                        if (rinet_ntop == NULL)
                        {
                            fprintf (stderr, "inet_ntop() failed. %s.\n", strerror (errno));
                        }

                        const uint16_t sport = ntohs (sin.sin_port);
                        printf ("Accepted connection from [%s]:%i to [%s]:%i.\n",
                                saddr, sport, serv_sock->addr, serv_sock->port);
                        close (raccept4);
                    }
                    else
                    {
                        switch (errno)
                        {
                            case EAGAIN:
                            case ECONNABORTED:
                            case EINTR:
                                break;
                            case EMFILE:
                            case ENFILE:
                            case ENOBUFS:
                            case ENOMEM:
                            case EPERM:
                            {
                                fprintf (stderr, "Failed to accept connection. %s.\n", strerror (errno));
                                break;
                            }
                            case EBADF:
                            case EFAULT:
                            case EINVAL:
                            case ENOTSOCK:
                            case EOPNOTSUPP:
                            case EPROTO:
                            {
                                fprintf (stderr, "Failed to accept connection. %s.\n", strerror (errno));
                                main_loop = false;
                                break;
                            }
                            default:
                            {
                                fprintf (stderr, "Failed to accept connection. %s.\n", strerror (errno));
                                main_loop = false;
                            }
                        }
                    }
                }
            }
        }
    }

    close (repoll_create1);
}