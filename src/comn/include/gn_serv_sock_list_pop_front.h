#ifndef GN_SERV_SOCK_LIST_POP_FRONT_H
#define GN_SERV_SOCK_LIST_POP_FRONT_H

#include <gn_serv_sock_list_t.h>

#include <stddef.h>

__attribute__((nonnull))
__attribute__((warn_unused_result))
gn_serv_sock_t *
gn_serv_sock_list_pop_front (gn_serv_sock_list_t * const list);

#endif // GN_SERV_SOCK_LIST_POP_FRONT_H