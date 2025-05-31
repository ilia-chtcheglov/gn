#ifndef GN_ACPT_CONN_H
#define GN_ACPT_CONN_H

#include <arpa/inet.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <gn_conn_t.h>
#include <gn_conn_mgmt_thrd_data_list_t.h>
#include <gn_serv_sock_t.h>

__attribute__((warn_unused_result))
int
gn_acpt_conn (const gn_serv_sock_t * const serv_sock, gn_conn_mgmt_thrd_data_list_t * const list);

int
gn_close (int * const fd);

__attribute__((nonnull))
void
gn_htbl_free (gn_htbl_t * const tbl);

__attribute__((nonnull))
__attribute__((warn_unused_result))
bool
gn_htbl_init (gn_htbl_t * const tbl,
              const gn_htbl_len_t size);

#endif // GN_ACPT_CONN_H