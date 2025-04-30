#ifndef GN_IPC_ACPT_H
#define GN_IPC_ACPT_H

#include <arpa/inet.h>
#include <errno.h>
#include <poll.h>
#include <stdio.h>
#include <string.h>

__attribute__((warn_unused_result))
int
gn_ipc_acpt (const int ipc_sock);

#endif // GN_IPC_ACPT_H