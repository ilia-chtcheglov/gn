#ifndef GN_PROCESS_CONN_H
#define GN_PROCESS_CONN_H

#include <gn_conn_t.h>

#include <stdbool.h>
#include <stdio.h>

__attribute__((nonnull))
__attribute__((warn_unused_result))
bool
gn_process_conn (gn_conn_t * const conn);

void
gn_close_conn (gn_conn_t * const conn);

__attribute__((nonnull))
void
gn_extr_hdrn (gn_conn_t * const conn);

__attribute__((nonnull))
void
gn_extr_hdrv (gn_conn_t * const conn);

__attribute__((nonnull))
void
gn_extr_mthd (gn_conn_t * const conn);

__attribute__((nonnull))
void
gn_extr_prot (gn_conn_t * const conn);

__attribute__((nonnull))
void
gn_extr_uri (gn_conn_t * const conn);

__attribute__((nonnull))
void
gn_recv_data (gn_conn_t * const conn);

#endif // GN_PROCESS_CONN_H