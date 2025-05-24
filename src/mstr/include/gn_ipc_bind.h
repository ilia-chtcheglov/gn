#ifndef GN_IPC_BIND_H
#define GN_IPC_BIND_H

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>

__attribute__((warn_unused_result))
bool
gn_ipc_bind (const int ipc_sock, struct sockaddr_un * const ipc_addr);

#endif // GN_IPC_BIND_H