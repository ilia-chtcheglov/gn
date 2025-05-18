#include <gn_acpt_conn.h>

#include <stdatomic.h>

__attribute__((warn_unused_result))
int
gn_acpt_conn (const gn_serv_sock_t * const serv_sock, gn_conn_mgmt_thrd_data_list_t * const list)
{
    struct sockaddr_in sin;
    memset (&sin, 0, sizeof (sin));
    socklen_t sin_sz = sizeof (sin);

    // Accept connection.
    int raccept4 = accept4 (serv_sock->fd, &sin, &sin_sz, SOCK_CLOEXEC | SOCK_NONBLOCK);
    if (raccept4 < 0)
    {
        // Handle accept4() error.
        switch (errno)
        {
            case EAGAIN:
            case ECONNABORTED:
            case EINTR:
                return EXIT_SUCCESS;
            case EMFILE:
            case ENFILE:
            case ENOBUFS:
            case ENOMEM:
            case EPERM:
            {
                fprintf (stderr, "Failed to accept connection. %s.\n", strerror (errno));
                return EXIT_SUCCESS;
            }
            case EBADF:
            case EFAULT:
            case EINVAL:
            case ENOTSOCK:
            case EOPNOTSUPP:
            case EPROTO:
            {
                fprintf (stderr, "Failed to accept connection. %s.\n", strerror (errno));
                return EXIT_FAILURE;
            }
            default:
            {
                fprintf (stderr, "Failed to accept connection. %s.\n", strerror (errno));
                return EXIT_FAILURE;
            }
        }
    }

    // Contains the source IP.
    char saddr[INET_ADDRSTRLEN];
    memset (saddr, 0, sizeof (saddr));

    // Get the source IP and store it in @saddr.
    const char * const rinet_ntop = inet_ntop (AF_INET, &sin.sin_addr, saddr, sizeof (saddr));
    if (rinet_ntop == NULL)
    {
        fprintf (stderr, "inet_ntop() failed. %s.\n", strerror (errno));
        return EXIT_SUCCESS;
    }

    // Get the source port.
    const uint16_t sport = ntohs (sin.sin_port);
    printf ("Accepted connection from [%s]:%i to [%s]:%i.\n", saddr, sport, serv_sock->addr, serv_sock->port);

    // Allocate structure to store connection data (socket, state, etc).
    gn_conn_t * conn = (gn_conn_t *)malloc (sizeof (gn_conn_t));
    if (conn == NULL)
    {
        fprintf (stderr, "Failed to allocate structure for connection.\n");
        return EXIT_SUCCESS;
    }

    memset (conn, 0, sizeof (gn_conn_t));
    printf ("Allocated conn: %p\n", (void *)conn); // TODO: Remove.

    // Allocate buffer to store the source IP.
    conn->saddr = (char *)malloc (strlen (saddr) + 1);
    if (conn->saddr != NULL)
    {
        // Store data in the connection structure.
        strcpy (conn->saddr, saddr);
        conn->fd = raccept4;
        conn->sport = sport;

        // Pass the structure to a connection management thread.
        const gn_conn_mgmt_thrd_data_t * data = list->head;
        for (uint8_t i = 0; i < list->len; data = data->next, i++)
        {
            for (uint8_t j = 0; j < sizeof (data->new_conns) / sizeof (atomic_uintptr_t); j++)
            {
                atomic_uintptr_t expected = (uintptr_t)NULL;
                if (atomic_compare_exchange_strong_explicit (&data->new_conns[j], &expected, (uintptr_t)conn,
                                                             memory_order_relaxed, memory_order_relaxed))
                {
                    conn = NULL;
                    raccept4 = -1;
                }
            }
        }

        if (conn != NULL)
        {
            fprintf (stderr, "Failed to pass connection to connection management thread.\n");
            free (conn->saddr);
        }
    }
    else
    {
        fprintf (stderr, "Failed to allocate buffer for source IP.\n");
    }
    free (conn);

    // Close client socket.
    gn_close (&raccept4);

    return EXIT_SUCCESS;
}