#ifndef GN_IPC_CLOSE_H
#define GN_IPC_CLOSE_H

__attribute__((nonnull))
void
gn_ipc_close (int * const ipc_sock);

int
gn_close (int * const fd);

#endif // GN_IPC_CLOSE_H