#ifndef GN_STR_DEL_H
#define GN_STR_DEL_H

#include <gn_str_t.h>

#include <stdlib.h>

void
gn_str_del (gn_str_t ** const str);

void
gn_str_deinit (gn_str_t * const str);

#endif // GN_STR_DEL_H