#include <gn_recv_serv_sock.h>

__attribute__((warn_unused_result))
int
gn_recv_serv_sock (const int ipc_sock, gn_serv_sock_list_t * const serv_sock_list)
{
    printf ("In gn_recv_serv_sock()\n");
    // Allocate gn_serv_sock_t structure to store socket, IP address and port of server socket.
    gn_serv_sock_t * const serv_sock = (gn_serv_sock_t *)malloc (sizeof (gn_serv_sock_t));
    if (serv_sock == NULL)
    {
        fprintf (stderr, "Failed to allocate structure to store server socket data.\n");
        return EXIT_FAILURE;
    }
    memset (serv_sock, 0, sizeof (gn_serv_sock_t));

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

            const ssize_t rrecvmsg = recvmsg (ipc_sock, &msg_hdr, MSG_NOSIGNAL);
            if (rrecvmsg == -1)
            {
                fprintf (stderr, "Failed to receive server socket data. %s.\n", strerror (errno));
            }
            else
            {
                printf ("Received %lu bytes \"%s\".\n", (size_t)rrecvmsg, buf);

                // Check structure before using it.
                const struct cmsghdr * const cmsg_hdr = CMSG_FIRSTHDR (&msg_hdr);
                if (cmsg_hdr == NULL)
                {
                    fprintf (stderr, "cmsg_hdr is NULL.\n");
                    break;
                }
                if (cmsg_hdr->cmsg_level != SOL_SOCKET)
                {
                    fprintf (stderr, "cmsg_level is %i instead of SOL_SOCKET.\n", cmsg_hdr->cmsg_level);
                    break;
                }
                if (cmsg_hdr->cmsg_type != SCM_RIGHTS)
                {
                    fprintf (stderr, "cmsg_type is %i instead of SCM_RIGHTS.\n", cmsg_hdr->cmsg_type);
                    break;
                }

                // Extract server socket.
                memcpy (&serv_sock->fd, CMSG_DATA (cmsg_hdr), sizeof (int));
                printf ("Received FD %i for \"%s\"\n", serv_sock->fd, buf);

                // Find IP address delimiter.
                size_t buf_i = 0; // Length of IP address string.
                for (; buf[buf_i] != '@' && buf[buf_i] != '\0'; buf_i++) ;
                if (buf[buf_i] != '@')
                {
                    fprintf (stderr, "IP address delimiter not found.\n");
                    break;
                }

                // Allocate buffer to store the extracted IP address.
                serv_sock->addr = (char *)malloc (buf_i + 1);
                if (serv_sock->addr == NULL)
                {
                    fprintf (stderr, "Failed to allocate buffer for server IP address.");
                    break;
                }

                // Copy IP address from @buf.
                memcpy (serv_sock->addr, buf, buf_i);
                serv_sock->addr[buf_i] = '\0';
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

                const int32_t port = atoi (port_buf);
                if (port < 1 || port > 65535)
                {
                    fprintf (stderr, "Received invalid port number %i.\n", port);
                    break;
                }

                serv_sock->port = (uint16_t)port;
                printf ("Converted port: %i.\n", port);

                gn_serv_sock_list_push_back (serv_sock_list, serv_sock);

                return EXIT_SUCCESS;
            }
            break;
        }
        case 0:
        {
            fprintf (stderr, "Timeout waiting for server socket data.\n");
            break;
        }
        case -1:
        {
            fprintf (stderr, "Failed to wait for server socket data. %s.\n", strerror (errno));
            break;
        }
        default:
        {
            fprintf (stderr, "poll() returned unexpected value %i.\n", rpoll);
        }
    }

    close (serv_sock->fd);
    free (serv_sock->addr);
    free (serv_sock);

    return EXIT_FAILURE;
}