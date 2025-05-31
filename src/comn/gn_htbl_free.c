#include <gn_htbl_free.h>

__attribute__((nonnull))
void
gn_htbl_free (gn_htbl_t * const tbl)
{
    for (gn_htbl_len_t i = 0; i < tbl->sz; i++)
    {
        if (tbl->itms[i] == NULL) continue;

        gn_htbl_item_t * item = tbl->itms[i];
        while (item != NULL)
        {
            free (item->val);
            free (item->key);
            gn_htbl_item_t * next_item = item->next;
            free (item);
            item = next_item;
        }
    }

    free (tbl->itms);
    tbl->itms = NULL;
    tbl->len = 0;
    tbl->sz = 0;
}