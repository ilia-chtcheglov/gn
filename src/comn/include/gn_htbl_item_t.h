#ifndef GN_HTBL_ITEM_T_H
#define GN_HTBL_ITEM_T_H

#include <gn_str_t.h>

typedef struct gn_htbl_item_t gn_htbl_item_t;

struct gn_htbl_item_t
{
    char *           key;
    gn_str_len_t     key_len;
    char *           val;
    gn_str_len_t     val_len;
    gn_htbl_item_t * prev;
    gn_htbl_item_t * next;
};

#endif // GN_HTBL_ITEM_T_H