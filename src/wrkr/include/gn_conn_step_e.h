#ifndef GN_CONN_STEP_E_H
#define GN_CONN_STEP_E_H

typedef enum
{
    GN_CONN_STEP_INVALID,
    GN_CONN_STEP_EXTR_MTHD,
    GN_CONN_STEP_EXTR_URI,
    GN_CONN_STEP_RECV_DATA,
    GN_CONN_STEP_CLOSE
} gn_conn_step_e;

#endif // GN_CONN_STEP_E_H