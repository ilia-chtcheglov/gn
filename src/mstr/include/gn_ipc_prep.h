#ifndef GN_IPC_PREP_H
#define GN_IPC_PREP_H

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <time.h>
#include <unistd.h>

__attribute__((malloc))
__attribute__((warn_unused_result))
char *
gn_ipc_prep (const int ipc_sock);

__attribute__((warn_unused_result))
bool
gn_ipc_bind (const int ipc_sock, struct sockaddr_un * const ipc_addr);

__attribute__((warn_unused_result))
bool
gn_ipc_lstn (const int ipc_sock);

#endif // GN_IPC_PREP_H