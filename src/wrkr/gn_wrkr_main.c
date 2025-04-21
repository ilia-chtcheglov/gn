#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <sys/un.h>
#include <unistd.h>

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