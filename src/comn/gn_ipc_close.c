#include <gn_ipc_close.h>

__attribute__((nonnull))
void
gn_ipc_close (int * const ipc_sock)
{
    gn_close (ipc_sock);
}