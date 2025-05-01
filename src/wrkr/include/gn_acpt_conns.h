#ifndef GN_ACPT_CONNS_H
#define GN_ACPT_CONNS_H

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>

#include <gn_serv_sock_t.h>

__attribute__((warn_unused_result))
int
gn_acpt_conns (int * const repoll_create1);

__attribute__((warn_unused_result))
int
gn_acpt_conn (const gn_serv_sock_t * const serv_sock);

#endif // GN_ACPT_CONNS_H