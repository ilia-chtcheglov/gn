#ifndef GN_WRKR_DATA_LIST_PUSH_BACK_H
#define GN_WRKR_DATA_LIST_PUSH_BACK_H

#include <stdbool.h>

#include <gn_wrkr_data_list_t.h>

__attribute__((warn_unused_result))
bool
gn_wrkr_data_list_push_back (gn_wrkr_data_list_t * const list, gn_wrkr_data_t * const data);

#endif // GN_WRKR_DATA_LIST_PUSH_BACK_H