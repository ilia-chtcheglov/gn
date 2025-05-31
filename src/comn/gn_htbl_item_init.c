#include <gn_htbl_item_init.h>

__attribute__((nonnull))
void
gn_htbl_item_init (gn_htbl_item_t * const item)
{
    item->key = NULL;
    item->key_len = 0;
    item->val = NULL;
    item->val_len = 0;
    item->prev = NULL;
    item->next = NULL;
}