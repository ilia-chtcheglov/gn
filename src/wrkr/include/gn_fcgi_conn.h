#ifndef GN_FCGI_CONN_H
#define GN_FCGI_CONN_H

#include <gn_conn_t.h>

#include <arpa/inet.h>
#include <errno.h>
#include <poll.h>
#include <stdio.h>
#include <string.h>

__attribute__((nonnull))
void
gn_fcgi_conn (gn_conn_t * const conn);

int
gn_close (int * const fd);

#endif // GN_FCGI_CONN_H