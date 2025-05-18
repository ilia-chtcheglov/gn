#ifndef GN_MSTR_CONF_T_H
#define GN_MSTR_CONF_T_H

#include <stdint.h>

typedef struct
{
    uint8_t workers;
    uint8_t connection_acceptance_threads;
    uint8_t connection_management_threads;
} gn_mstr_conf_t;

#endif // GN_MSTR_CONF_T_H