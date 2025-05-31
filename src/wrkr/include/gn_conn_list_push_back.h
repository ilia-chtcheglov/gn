#ifndef GN_CONN_LIST_PUSH_BACK_H
#define GN_CONN_LIST_PUSH_BACK_H

#include <gn_conn_list_t.h>

#include <stdbool.h>
#include <stdlib.h>

__attribute__((nonnull))
__attribute__((warn_unused_result))
bool
gn_conn_list_push_back (gn_conn_list_t * const list, gn_conn_t * const conn);

#endif // GN_CONN_LIST_PUSH_BACK_H