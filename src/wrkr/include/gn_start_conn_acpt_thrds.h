#ifndef GN_START_CONN_ACPT_THRDS_H
#define GN_START_CONN_ACPT_THRDS_H

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <gn_conn_acpt_thrd_data_list_t.h>

void
gn_start_conn_acpt_thrds (const uint8_t num, gn_conn_acpt_thrd_data_list_t * const conn_acpt_thrd_data_list);

__attribute__((warn_unused_result))
int
gn_conn_acpt_thrd_data_list_push_back (gn_conn_acpt_thrd_data_list_t * const list,
                                       gn_conn_acpt_thrd_data_t * const data);

void *
gn_conn_acpt_thrd (void * const p);

#endif // GN_START_CONN_ACPT_THRDS_H