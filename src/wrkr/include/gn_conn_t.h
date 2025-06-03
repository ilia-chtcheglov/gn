#ifndef GN_CONN_T_H
#define GN_CONN_T_H

#include <gn_htbl_t.h>
#include <gn_str_t.h>

#include <gn_conn_step_e.h>

#include <stdint.h>
#include <time.h>

typedef struct gn_conn_t gn_conn_t;

struct gn_conn_t
{
    int            sock;
    int            fd;
    uint16_t       status;
    gn_conn_step_e step;
    gn_conn_step_e prev_step;
    char *         saddr;
    uint16_t       sport;
    gn_htbl_t      req_hdrs;
    gn_str_t       recv_buf;
    gn_str_t       send_buf;
    gn_str_t       mthd;
    gn_str_t       uri;
    gn_str_t       prot;
    gn_str_t       hdrn;
    gn_str_t       hdrv;
    time_t         last_io;
    gn_conn_t *    prev;
    gn_conn_t *    next;
};

#endif // GN_CONN_T_H