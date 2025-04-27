#include <arpa/inet.h>
#include <errno.h>
#include <poll.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/un.h>
#include <unistd.h>

#include <gn_serv_sock_list_t.h>

__attribute__((warn_unused_result))
int
gn_recv_serv_sock (const int ipc_sock, gn_serv_sock_list_t * const serv_sock_list);

void
gn_wrkr_main (int ipc_sock, gn_serv_sock_list_t * const serv_sock_list, const char * const ipc_addr_str);

void
gn_wrkr_main (int ipc_sock, gn_serv_sock_list_t * const serv_sock_list, const char * const ipc_addr_str)
{
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

            while (gn_recv_serv_sock (ipc_sock, serv_sock_list) == EXIT_SUCCESS) sleep (1);

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