#ifndef GN_OPEN_SERV_SOCK_H
#define GN_OPEN_SERV_SOCK_H

#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <gn_serv_sock_list_t.h>

__attribute__((warn_unused_result))
int
gn_open_serv_sock (gn_serv_sock_list_t * const list, const char * const addr, const uint16_t port);

int
gn_close (int * const fd);

int
gn_serv_sock_list_push_back (gn_serv_sock_list_t * const list, gn_serv_sock_t * const sock);

#endif // GN_OPEN_SERV_SOCK_H