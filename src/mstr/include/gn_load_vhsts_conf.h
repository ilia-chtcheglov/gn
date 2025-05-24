#ifndef GN_LOAD_VHSTS_CONF_H
#define GN_LOAD_VHSTS_CONF_H

#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

#include <gn_vhst_conf_list_t.h>

#include <json-c/json.h>

#define GN_VHSTS_CONF_DIR_PATH "/etc/gn/vhosts/enabled/"

void
gn_load_vhsts_conf (gn_vhst_conf_list_t * const vhst_conf_list);

#endif // GN_LOAD_VHSTS_CONF_H