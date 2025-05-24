#ifndef GN_VHST_CONF_LIST_T_H
#define GN_VHST_CONF_LIST_T_H

#include <stdint.h>

#include <gn_vhst_conf_t.h>

typedef struct
{
    gn_vhst_conf_t * head;
    gn_vhst_conf_t * tail;
    uint32_t         len;
} gn_vhst_conf_list_t;

#endif // GN_VHST_CONF_LIST_T_H