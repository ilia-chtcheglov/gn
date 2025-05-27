#ifndef GN_IPC_OPEN_H
#define GN_IPC_OPEN_H

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>

__attribute__((warn_unused_result))
int
gn_ipc_open (void);

#endif // GN_IPC_OPEN_H