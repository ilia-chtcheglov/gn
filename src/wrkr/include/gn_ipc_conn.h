#ifndef GN_IPC_CONN_H
#define GN_IPC_CONN_H

#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/un.h>

__attribute__((warn_unused_result))
int
gn_ipc_conn (const int ipc_sock, const char * const ipc_addr_str);

#endif // GN_IPC_CONN_H