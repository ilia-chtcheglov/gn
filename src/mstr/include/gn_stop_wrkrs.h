#ifndef GN_STOP_WRKRS_H
#define GN_STOP_WRKRS_H

#include <stdlib.h>

#include <gn_wrkr_data_list_t.h>

void
gn_stop_wrkrs (gn_wrkr_data_list_t * const list);

int
gn_close (int * const fd);

__attribute__((warn_unused_result))
gn_wrkr_data_t *
gn_wrkr_data_list_pop (gn_wrkr_data_list_t * const list);

#endif // GN_STOP_WRKRS_H