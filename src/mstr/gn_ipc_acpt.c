#include <gn_ipc_acpt.h>

__attribute__((warn_unused_result))
int
gn_ipc_acpt (const int ipc_sock)
{
    // Wait for a connection from the worker process we just started.
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
            // Accept the connection from the worker process.
            const int raccept4 = accept4 (ipc_sock, NULL, NULL, SOCK_CLOEXEC | SOCK_NONBLOCK);
            if (raccept4 > -1) return raccept4;

            if (raccept4 == -1)
            {
                fprintf (stderr, "Failed to accept IPC connection. %s.\n", strerror (errno));
            }
            else
            {
                fprintf (stderr, "accept4() returned unexpected value %i.\n", raccept4);
            }

            break;
        }
        case 0:
        {
            fprintf (stderr, "Timeout while waiting to accept IPC connection.\n");
            break;
        }
        case -1:
        {
            fprintf (stderr, "Error while waiting to accept IPC connection. %s.\n", strerror (errno));
            break;
        }
        default:
        {
            fprintf (stderr, "poll() returned unexpected value %i.\n", rpoll);
        }
    }

    return -1;
}