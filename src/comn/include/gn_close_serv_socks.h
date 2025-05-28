#ifndef GN_CLOSE_SERV_SOCKS_H
#define GN_CLOSE_SERV_SOCKS_H

#include <gn_serv_sock_list_t.h>

#include <stdlib.h>

__attribute__((nonnull))
void
gn_close_serv_socks (gn_serv_sock_list_t * const list);

int
gn_close (int * const fd);

__attribute__((nonnull))
__attribute__((warn_unused_result))
gn_serv_sock_t *
gn_serv_sock_list_pop_front (gn_serv_sock_list_t * const list);

#endif // GN_CLOSE_SERV_SOCKS_H