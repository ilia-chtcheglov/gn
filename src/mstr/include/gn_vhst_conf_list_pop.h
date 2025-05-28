#ifndef GN_VHST_CONF_LIST_POP_H
#define GN_VHST_CONF_LIST_POP_H

#include <gn_vhst_conf_list_t.h>

#include <stddef.h>

__attribute__((nonnull))
__attribute__((warn_unused_result))
gn_vhst_conf_t *
gn_vhst_conf_list_pop (gn_vhst_conf_list_t * const list);

#endif // GN_VHST_CONF_LIST_POP_H