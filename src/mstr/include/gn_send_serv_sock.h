#ifndef GN_SEND_SERV_SOCK_H
#define GN_SEND_SERV_SOCK_H

#include <arpa/inet.h>
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include <gn_serv_sock_t.h>

void
gn_send_serv_sock (const int ipc_sock, const gn_serv_sock_t * const serv_sock);

#endif // GN_SEND_SERV_SOCK_H