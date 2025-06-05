#include <gn_htbl_dump.h>

__attribute__((nonnull))
void
gn_htbl_dump (gn_htbl_t * const tbl)
{
    const char * indent = "    ";
    for (gn_htbl_len_t i = 0; i < tbl->sz; i++)
    {
        printf ("#%u: ", i);
        if (i < 10) ;
        else if (i < 100) indent = "     ";
        else if (i < 1000) indent = "      ";
        else if (i < 10000) indent = "       ";

        if (tbl->itms[i] == NULL)
        {
            printf ("NULL\n");
            continue;
        }

        printf ("(%u) \"%s\" -> (%u) \"%s\"\n",
                tbl->itms[i]->key_len, tbl->itms[i]->key,
                tbl->itms[i]->val_len, tbl->itms[i]->val);

        gn_htbl_item_t * next_item = tbl->itms[i]->next;
        for ( ; next_item != NULL; next_item = next_item->next)
        {
            printf ("%s(%u) \"%s\" -> (%u) \"%s\"\n", indent,
                    next_item->key_len, next_item->key,
                    next_item->val_len, next_item->val);
        }
    }
    printf ("\n");
}