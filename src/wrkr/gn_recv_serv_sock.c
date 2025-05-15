#include <gn_recv_serv_sock.h>

__attribute__((warn_unused_result))
int
gn_recv_serv_sock (const int ipc_sock, const int repoll_create1, gn_serv_sock_list_t * const serv_sock_list)
{
    // Allocate structure to store socket, IP address and port of server socket.
    gn_serv_sock_t * const serv_sock = (gn_serv_sock_t *)malloc (sizeof (gn_serv_sock_t));
    if (serv_sock == NULL)
    {
        fprintf (stderr, "Failed to allocate structure to store server socket data.\n");
        return 2; // Error, don't retry.
    }

    // Set default values in the new server socket.
    serv_sock->fd = -1;
    serv_sock->addr = NULL;
    serv_sock->port = 0;

    // Exit code of this function.
    int this_ret = 2; // Error, don't retry.

    // Wait 3 seconds to receive server socket data from the master process.
    struct pollfd pfd = {
        .fd = ipc_sock,
        .events = POLLIN,
        .revents = 0
    };

    const int rpoll = poll (&pfd, 1, 3000);
    switch (rpoll)
    {
        case 1:
        {
            break;
        }
        case 0:
        {
            fprintf (stderr, "Timeout waiting for server socket data.\n");
            goto labl_exit;
        }
        case -1:
        {
            fprintf (stderr, "Failed to wait for server socket data. %s.\n", strerror (errno));
            goto labl_exit;
        }
        default:
        {
            fprintf (stderr, "poll() returned unexpected value %i.\n", rpoll);
            goto labl_exit;
        }
    }

    // TODO: Check pfd.revents.
    // TODO: Check what the code does if @ipc_sock is disconnected from the master process.

    /*
     * If we're here it means there was no errors or timeout when calling poll().
     * Now we prepare different structures to receive server socket data.
     */
    char buf[1024];
    memset (buf, 0, sizeof (buf));

    struct iovec io_vec;
    memset (&io_vec, 0, sizeof (io_vec));
    io_vec.iov_base = buf;
    io_vec.iov_len = sizeof (buf);

    struct msghdr msg_hdr;
    memset (&msg_hdr, 0, sizeof (msg_hdr));
    msg_hdr.msg_iov = &io_vec;
    msg_hdr.msg_iovlen = 1;

    char control[CMSG_SPACE (sizeof (int))];
    msg_hdr.msg_control = control;
    msg_hdr.msg_controllen = sizeof (control);

    // Receive the data (server socket and string).
    const ssize_t rrecvmsg = recvmsg (ipc_sock, &msg_hdr, MSG_NOSIGNAL);
    if (rrecvmsg == -1)
    {
        fprintf (stderr, "Failed to receive server socket data. %s.\n", strerror (errno));
        goto labl_exit;
    }

    printf ("Received %lu bytes \"%s\".\n", (size_t)rrecvmsg, buf); // TODO: Remove.

    // Check structure before using it.
    const struct cmsghdr * const cmsg_hdr = CMSG_FIRSTHDR (&msg_hdr);
    if (cmsg_hdr == NULL)
    {
        fprintf (stderr, "cmsg_hdr is NULL.\n");
        goto labl_exit;
    }
    if (cmsg_hdr->cmsg_level != SOL_SOCKET)
    {
        fprintf (stderr, "cmsg_level is %i instead of SOL_SOCKET.\n", cmsg_hdr->cmsg_level);
        goto labl_exit;
    }
    if (cmsg_hdr->cmsg_type != SCM_RIGHTS)
    {
        fprintf (stderr, "cmsg_type is %i instead of SCM_RIGHTS.\n", cmsg_hdr->cmsg_type);
        goto labl_exit;
    }

    // TODO: Do a deep dive in CMSG_ macros and cmsg/msg structures to see what else we can check.

    // Extract server socket.
    memcpy (&serv_sock->fd, CMSG_DATA (cmsg_hdr), sizeof (int));
    printf ("Received FD %i for \"%s\"\n", serv_sock->fd, buf); // TODO: Remove.

    // Find IP address delimiter.
    size_t buf_i = 0; // Length of IP address string.
    for (; buf[buf_i] != '@' && buf[buf_i] != '\0'; buf_i++) ;
    if (buf[buf_i] != '@')
    {
        fprintf (stderr, "IP address delimiter not found.\n");
        goto labl_exit;
    }

    // Allocate buffer to store the extracted IP address.
    serv_sock->addr = (char *)malloc (buf_i + 1);
    if (serv_sock->addr == NULL)
    {
        fprintf (stderr, "Failed to allocate buffer for server IP address.");
        goto labl_exit;
    }

    // Copy IP address from @buf.
    memcpy (serv_sock->addr, buf, buf_i);
    serv_sock->addr[buf_i] = '\0';

    /*
     * If we received a NULL address it means the master process
     * has no more server sockets to send.
     */
    if (strcmp (serv_sock->addr, "(null)") == 0)
    {
        this_ret = 1; // No errors, don't retry.
        goto labl_exit;
    }

    printf ("Extracted address (%lu) \"%s\"\n", buf_i, serv_sock->addr);

    // Skip the address/port delimiter.
    buf_i++;

    // Extract the server port.
    char port_buf[6];
    memset (port_buf, 0, sizeof (port_buf));

    uint8_t port_buf_i = 0;
    for (; port_buf_i < 5 && buf[buf_i] != '\n' && buf[buf_i] != '\0'; buf_i++, port_buf_i++)
    {
        port_buf[port_buf_i] = buf[buf_i];
    }
    printf ("Extracted port (%lu) \"%s\"\n", strlen (port_buf), port_buf);

    // Check port and convert it from string to number.
    const int32_t port = atoi (port_buf);
    if (port < 1 || port > 65535)
    {
        fprintf (stderr, "Received invalid port number %i.\n", port);
        goto labl_exit;
    }

    serv_sock->port = (uint16_t)port;
    printf ("Converted port: %i.\n", port);

    struct epoll_event epoll_evt;
    memset (&epoll_evt, 0, sizeof (epoll_evt));
    epoll_evt.events = EPOLLIN | EPOLLRDHUP;
    epoll_evt.data.ptr = serv_sock;

    const int repoll_ctl = epoll_ctl (repoll_create1, EPOLL_CTL_ADD, serv_sock->fd, &epoll_evt);
    if (repoll_ctl == 0)
    {
        // Add server socket to the linked list.
        gn_serv_sock_list_push_back (serv_sock_list, serv_sock);
        return 0; // No errors, retry.
    }
    else
    {
        fprintf (stderr, "Failed to add server socket to epoll instance. %s.\n", strerror (errno));
    }

    labl_exit:
    gn_close (&serv_sock->fd);
    free (serv_sock->addr);
    free (serv_sock);

    return this_ret;
}