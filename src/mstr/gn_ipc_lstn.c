#include <gn_ipc_lstn.h>

__attribute__((warn_unused_result))
bool
gn_ipc_lstn (const int ipc_sock)
{
    const int rlisten = listen (ipc_sock, 8);
    switch (rlisten)
    {
        case 0:
            return EXIT_SUCCESS;
        case -1:
        {
            fprintf (stderr, "Failed to listen on IPC socket. %s.\n", strerror (errno));
            return EXIT_FAILURE;
        }
        default:
        {
            fprintf (stderr, "listen() returned undocumented value %i.\n", rlisten);
            return EXIT_FAILURE;
        }
    }
}