#ifndef GN_WRKR_DATA_T_H
#define GN_WRKR_DATA_T_H

#include <unistd.h>

typedef struct gn_wrkr_data_t gn_wrkr_data_t;

struct gn_wrkr_data_t
{
    int              ipc_sock;
    pid_t            pid;
    gn_wrkr_data_t * prev;
    gn_wrkr_data_t * next;
};

#endif // GN_WRKR_DATA_T_H