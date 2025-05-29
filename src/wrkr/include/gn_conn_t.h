#ifndef GN_CONN_T_H
#define GN_CONN_T_H

#include <stdint.h>

#include <gn_conn_step_e.h>

typedef struct gn_conn_t gn_conn_t;

struct gn_conn_t
{
    int            fd;
    gn_conn_step_e step;
    gn_conn_step_e prev_step;
    char *         saddr;
    uint16_t       sport;
    char *         recv_buf;
    uint32_t       recv_buf_len;
    uint32_t       recv_buf_sz;
    char *         mthd;
    uint32_t       mthd_len;
    uint32_t       mthd_sz;
    gn_conn_t *    prev;
    gn_conn_t *    next;
};

#endif // GN_CONN_T_H