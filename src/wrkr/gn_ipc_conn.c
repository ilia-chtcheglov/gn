#include <gn_ipc_conn.h>

__attribute__((warn_unused_result))
int
gn_ipc_conn (const int ipc_sock, const char * const ipc_addr_str)
{
    struct sockaddr_un sun;
    memset (&sun, 0, sizeof (sun));

    sun.sun_family = AF_UNIX;
    strcpy (sun.sun_path, ipc_addr_str);
    /*
     * Setting the first byte of .sun_path to '\0' will turn the socket address inside .sun_path
     * into an abstract socket address. No files will be created on disk.
     */
    sun.sun_path[0] = '\0';

    // Connect to the master process.
    const int rconnect = connect (ipc_sock, (struct sockaddr *)&sun, sizeof (sun));
    switch (rconnect)
    {
        case 0:
        {
            printf ("Connected to master process.\n"); // TODO: Remove.
            return EXIT_SUCCESS;
        }
        case -1:
        {
            fprintf (stderr, "Failed to connect to master process. %s.\n", strerror (errno));
            return EXIT_FAILURE;
        }
        default:
        {
            fprintf (stderr, "connect() returned unexpected value %i.\n", rconnect);
            return EXIT_FAILURE;
        }
    }
}