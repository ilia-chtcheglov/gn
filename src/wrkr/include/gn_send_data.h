#ifndef GN_SEND_DATA_H
#define GN_SEND_DATA_H

#include <gn_conn_t.h>

#include <errno.h>
#include <stdio.h>
#include <sys/socket.h>

__attribute__((nonnull))
void
gn_send_data (gn_conn_t * const conn);

int
gn_close (int * const fd);

__attribute__((nonnull))
__attribute__((warn_unused_result))
gn_str_err_e
gn_str_lshift (gn_str_t * const str,
               const gn_str_len_t num);

#endif // GN_SEND_DATA_H