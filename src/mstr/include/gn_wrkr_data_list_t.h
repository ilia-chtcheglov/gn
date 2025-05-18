#ifndef GN_WRKR_DATA_LIST_T_H
#define GN_WRKR_DATA_LIST_T_H

#include <stdint.h>

#include <gn_wrkr_data_t.h>

typedef struct
{
    gn_wrkr_data_t * head;
    gn_wrkr_data_t * tail;
    uint8_t          len;
} gn_wrkr_data_list_t;

#endif // GN_WRKR_DATA_LIST_T_H