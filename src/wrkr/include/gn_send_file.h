#ifndef GN_SEND_FILE_H
#define GN_SEND_FILE_H

#include <gn_conn_t.h>

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

__attribute__((nonnull))
void
gn_send_file (gn_conn_t * const conn);

__attribute__((nonnull))
void
gn_send_data (gn_conn_t * const conn);

#endif // GN_SEND_FILE_H