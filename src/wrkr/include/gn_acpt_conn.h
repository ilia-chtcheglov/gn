#ifndef GN_ACPT_CONN_H
#define GN_ACPT_CONN_H

#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <gn_serv_sock_t.h>

__attribute__((warn_unused_result))
int
gn_acpt_conn (const gn_serv_sock_t * const serv_sock);

#endif // GN_ACPT_CONN_H