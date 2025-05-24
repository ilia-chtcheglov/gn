#include <gn_ipc_bind.h>

__attribute__((warn_unused_result))
bool
gn_ipc_bind (const int ipc_sock, struct sockaddr_un * const ipc_addr)
{
    const int rbind = bind (ipc_sock, (struct sockaddr *)ipc_addr, sizeof (struct sockaddr_un));
    switch (rbind)
    {
        case 0:
            return EXIT_SUCCESS;
        case -1:
        {
            fprintf (stderr, "Failed to bind IPC socket. %s.\n", strerror (errno));
            return EXIT_FAILURE;
        }
        default:
        {
            fprintf (stderr, "bind() returned undocumented value %i.\n", rbind);
            return EXIT_FAILURE;
        }
    }
}