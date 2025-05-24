#include <gn_ipc_prep.h>

__attribute__((malloc))
__attribute__((warn_unused_result))
char *
gn_ipc_prep (const int ipc_sock)
{
    // Get current time to generate a path for the IPC socket.
    struct timespec ts;
    memset (&ts, 0, sizeof (ts));

    const int rclock_gettime = clock_gettime (CLOCK_REALTIME, &ts);
    switch (rclock_gettime)
    {
        case 0:
            break;
        case -1:
        {
            fprintf (stderr, "Failed to get current time. %s.\n", strerror (errno));
            return NULL;
        }
        default:
        {
            fprintf (stderr, "clock_gettime() returned undocumented value %i.\n", rclock_gettime);
            return NULL;
        }
    }

    /*
     * Generate an address for the IPC socket. The address is made of:
     * - Current process ID in hexadecimal.
     * - Time in seconds in hexadecimal.
     * - Nanoseconds in hexadecimal.
     * - The first byte will be replaced with '\0' to make the address an abstract one.
     */
    struct sockaddr_un sun;
    memset (&sun, 0, sizeof (sun));

    sun.sun_family = AF_UNIX;
    // sun.sun_path is sun_path[108].

    int rsnprintf = snprintf (sun.sun_path, sizeof (sun.sun_path), "Z%X%lX%lX",
                              (unsigned int)getpid (), (long unsigned int)ts.tv_sec, (long unsigned int)ts.tv_nsec);
    printf ("rsnprintf: %i, sun_path: \"%s\".\n", rsnprintf, sun.sun_path);

    /*
     * Setting the first byte of .sun_path to '\0' will turn the socket address inside .sun_path
     * into an abstract socket address. No files will be created on disk.
     */
    sun.sun_path[0] = '\0';

    if (gn_ipc_bind (ipc_sock, &sun) != EXIT_SUCCESS) return NULL;

    // Set sun_path[0] back to some byte because some gn_* functions can't work if @sun_path starts with '\0'.
    sun.sun_path[0] = 'Z';

    if (gn_ipc_lstn (ipc_sock) != EXIT_SUCCESS) return NULL;

    char * ipc_addr_str = (char *)malloc (strlen (sun.sun_path) + 1);
    if (ipc_addr_str == NULL)
    {
        fprintf (stderr, "Failed to allocate buffer for IPC address string.\n");
        return NULL;
    }

    strcpy (ipc_addr_str, sun.sun_path);
    return ipc_addr_str;
}