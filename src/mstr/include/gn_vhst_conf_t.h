#ifndef GN_VHST_CONF_T_H
#define GN_VHST_CONF_T_H

typedef struct gn_vhst_conf_t gn_vhst_conf_t;

struct gn_vhst_conf_t
{
    gn_vhst_conf_t * prev;
    gn_vhst_conf_t * next;
};

#endif // GN_VHST_CONF_T_H