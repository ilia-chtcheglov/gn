#include <arpa/inet.h>
#include <errno.h>
#include <poll.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/un.h>
#include <unistd.h>

#include <gn_serv_sock_t.h>

int
gn_recv_serv_sock (const int ipc_sock);

int
gn_recv_serv_sock (const int ipc_sock)
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
                    fprintf (stderr, "cmsg_level is %i instead of SOL_SOCKET.", cmsg_hdr->cmsg_level);
                    break;
                }
                if (cmsg_hdr->cmsg_type != SCM_RIGHTS)
                {
                    fprintf (stderr, "cmsg_type is %i instead of SCM_RIGHTS.", cmsg_hdr->cmsg_type);
                    break;
                }

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

    free (serv_sock);
    return EXIT_FAILURE;
}

void
gn_wrkr_main (int ipc_sock, const char * const ipc_addr_str);

void
gn_wrkr_main (int ipc_sock, const char * const ipc_addr_str)
{
    (void)ipc_sock; // TODO: Remove.
    printf ("[%i] Worker started. Received IPC address \"%s\".\n", getpid (), ipc_addr_str);

    struct sockaddr_un sun;
    memset (&sun, 0, sizeof (sun));

    sun.sun_family = AF_UNIX;
    strcpy (sun.sun_path, ipc_addr_str);

    const int rconnect = connect (ipc_sock, (struct sockaddr *)&sun, sizeof (sun));
    switch (rconnect)
    {
        case 0:
        {
            printf ("Connected to master process.\n");

            while (gn_recv_serv_sock (ipc_sock) == EXIT_SUCCESS) sleep (1);

            while (true) sleep (1);

            break;
        }
        case -1:
        {
            fprintf (stderr, "Failed to connect to master process. %s.\n", strerror (errno));
            return;
        }
        default:
        {
            fprintf (stderr, "connect() returned unexpected value %i.\n", rconnect);
            return;
        }
    }
}