#ifndef GN_IPC_LSTN_H
#define GN_IPC_LSTN_H

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

__attribute__((warn_unused_result))
bool
gn_ipc_lstn (const int ipc_sock);

#endif // GN_IPC_LSTN_H