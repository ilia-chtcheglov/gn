#ifndef GN_HTBL_T_H
#define GN_HTBL_T_H

#include <gn_htbl_item_t.h>

typedef uint32_t gn_htbl_len_t;
#define GN_HTBL_LEN_MAX UINT32_MAX

typedef struct
{
    gn_htbl_item_t ** itms;
    gn_htbl_len_t     len;
    gn_htbl_len_t     sz;
} gn_htbl_t;

#endif // GN_HTBL_T_H