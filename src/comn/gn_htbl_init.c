#include <gn_htbl_init.h>

__attribute__((nonnull))
__attribute__((warn_unused_result))
bool
gn_htbl_init (gn_htbl_t * const tbl, const gn_htbl_len_t size)
{
    tbl->len = 0;
    tbl->itms = (gn_htbl_item_t **)malloc (size * sizeof (gn_htbl_item_t *));
    if (tbl->itms == NULL)
    {
        tbl->sz = 0;
        return EXIT_FAILURE;
    }

    memset (tbl->itms, 0, size * sizeof (gn_htbl_item_t *));
    tbl->sz = size;
    return EXIT_SUCCESS;
}