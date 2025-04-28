#include <gn_wrkr_main.h>

void
gn_wrkr_main (int ipc_sock, gn_serv_sock_list_t * const serv_sock_list, const char * const ipc_addr_str)
{
    printf ("[%i] Worker started. Received IPC address \"%s\".\n", getpid (), ipc_addr_str);

    struct sockaddr_un sun;
    memset (&sun, 0, sizeof (sun));

    sun.sun_family = AF_UNIX;
    strcpy (sun.sun_path, ipc_addr_str);

    // Connect to the master process.
    const int rconnect = connect (ipc_sock, (struct sockaddr *)&sun, sizeof (sun));
    switch (rconnect)
    {
        case 0:
        {
            printf ("Connected to master process.\n");

            // Receive server sockets data.
            int rgn_recv_serv_sock = 0;
            while (rgn_recv_serv_sock == 0)
            {
                rgn_recv_serv_sock = gn_recv_serv_sock (ipc_sock, serv_sock_list);
            }

            if (rgn_recv_serv_sock == 1)
            {
                // Test code.
                const gn_serv_sock_t * serv_sock = serv_sock_list->head;
                for (uint16_t i = 0; i < serv_sock_list->len; serv_sock = serv_sock->next, i++)
                {
                    printf ("Received server socket .fd: %i, .addr: [%s], .port: %i.\n",
                            serv_sock->fd, serv_sock->addr, serv_sock->port);
                }

                // Main loop.
                while (true) sleep (1);
            }
            else
            {
                fprintf (stderr, "Error occured while receiving server socket data.\n");
            }

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