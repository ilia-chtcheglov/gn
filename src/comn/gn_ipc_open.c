#include <gn_ipc_open.h>

__attribute__((warn_unused_result))
int
gn_ipc_open (void)
{
    const int ipc_sock = socket (AF_UNIX, SOCK_STREAM | SOCK_CLOEXEC | SOCK_NONBLOCK, 0);
    if (ipc_sock < 0) fprintf (stderr, "Failed to create IPC socket. %s.\n", strerror (errno));

    return ipc_sock;
}