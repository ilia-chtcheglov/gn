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
        goto labl_close_sock;
    }

    // Get the source port.
    const uint16_t sport = ntohs (sin.sin_port);
    printf ("Accepted connection from [%s]:%i to [%s]:%i.\n", saddr, sport, serv_sock->addr, serv_sock->port);

    // Allocate structure to store connection data (socket, state, etc).
    gn_conn_t * conn = (gn_conn_t *)malloc (sizeof (gn_conn_t));
    if (conn == NULL)
    {
        fprintf (stderr, "Failed to allocate structure for connection.\n");
        goto labl_close_sock;
    }

    memset (conn, 0, sizeof (gn_conn_t));
    printf ("Allocated conn: %p\n", (void *)conn); // TODO: Remove.

    if (gn_htbl_init (&conn->req_hdrs, 16) != EXIT_SUCCESS) goto labl_free_conn;

    // Allocate buffer to store the source IP.
    conn->saddr = (char *)malloc (strlen (saddr) + 1);
    if (conn->saddr == NULL)
    {
        fprintf (stderr, "Failed to allocate buffer for source IP.\n");
        goto labl_free_htbl;
    }

    #define GN_RECV_BUF_LEN 1023
    if (gn_str_init (&conn->recv_buf, GN_RECV_BUF_LEN) != GN_STR_ERR_NONE)
    {
        fprintf (stderr, "Failed to allocate receive buffer.\n");
        goto labl_free_saddr;
    }

    #define GN_SEND_BUF_LEN 65535
    if (gn_str_init (&conn->send_buf, GN_SEND_BUF_LEN) != GN_STR_ERR_NONE)
    {
        fprintf (stderr, "Failed to allocate send buffer.\n");
        goto labl_free_recv_buf;
    }

    #define GN_MTHD_LEN 15
    if (gn_str_init (&conn->mthd, GN_MTHD_LEN) != GN_STR_ERR_NONE)
    {
        fprintf (stderr, "Failed to allocate request method buffer.\n");
        goto labl_free_send_buf;
    }

    #define GN_URI_LEN 65535
    if (gn_str_init (&conn->uri, GN_URI_LEN) != GN_STR_ERR_NONE)
    {
        fprintf (stderr, "Failed to allocate request URI buffer.\n");
        goto labl_free_mthd;
    }

    #define GN_PROT_LEN 15
    if (gn_str_init (&conn->prot, GN_PROT_LEN) != GN_STR_ERR_NONE)
    {
        fprintf (stderr, "Failed to allocate request protocol buffer.\n");
        goto labl_free_uri;
    }

    #define GN_HDRN_LEN 255
    if (gn_str_init (&conn->hdrn, GN_HDRN_LEN) != GN_STR_ERR_NONE)
    {
        fprintf (stderr, "Failed to allocate request header name buffer.\n");
        goto labl_free_prot;
    }

    #define GN_HDRV_LEN 4096
    if (gn_str_init (&conn->hdrv, GN_HDRV_LEN) != GN_STR_ERR_NONE)
    {
        fprintf (stderr, "Failed to allocate request header value buffer.\n");
        goto labl_free_hdrn;
    }

    conn->sock = raccept4;
    conn->sport = sport;
    conn->status = 0;
    conn->fd = -1;
    time (&conn->last_io); // TODO: Check return value.
    conn->step = GN_CONN_STEP_RECV_DATA;
    conn->prev_step = GN_CONN_STEP_EXTR_MTHD;

      // Store source IP in the connection structure.
    strcpy (conn->saddr, saddr);

    // Pass the structure to a connection management thread.
    gn_conn_mgmt_thrd_data_t * data = list->head;
    for (uint8_t i = 0; i < list->len; data = data->next, i++)
    {
        for (uint8_t j = 0; j < sizeof (data->new_conns) / sizeof (atomic_uintptr_t); j++)
        {
            atomic_uintptr_t expected = (uintptr_t)NULL;
            if (atomic_compare_exchange_strong_explicit (&data->new_conns[j], &expected, (uintptr_t)conn,
                                                         memory_order_relaxed, memory_order_relaxed))
            {
                atomic_flag_clear (&data->no_new_conn);
                return EXIT_SUCCESS;
            }
        }
    }

    fprintf (stderr, "Failed to pass connection to connection management thread.\n");

    gn_str_deinit (&conn->hdrv);

    labl_free_hdrn:
    gn_str_deinit (&conn->hdrn);

    labl_free_prot:
    gn_str_deinit (&conn->prot);

    labl_free_uri:
    gn_str_deinit (&conn->uri);

    labl_free_mthd:
    gn_str_deinit (&conn->mthd);

    labl_free_send_buf:
    gn_str_deinit (&conn->send_buf);

    labl_free_recv_buf:
    gn_str_deinit (&conn->recv_buf);

    labl_free_saddr:
    // Free source IP buffer.
    free (conn->saddr);

    labl_free_htbl:
    gn_htbl_free (&conn->req_hdrs);

    labl_free_conn:
    // Free connection structure.
    free (conn);
    conn = NULL;

    labl_close_sock:
    // Close client socket.
    gn_close (&raccept4);

    return EXIT_SUCCESS;
}