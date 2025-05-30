#ifndef GN_RECV_DATA_H
#define GN_RECV_DATA_H

#include <gn_conn_t.h>

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>

__attribute__((nonnull))
void
gn_recv_data (gn_conn_t * const conn);

#endif // GN_RECV_DATA_H