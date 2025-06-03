#include <gn_htbl_srch.h>

__attribute__((nonnull))
__attribute__((warn_unused_result))
gn_htbl_item_t *
gn_htbl_srch (gn_htbl_t * const tbl,
              const char * const key,
              gn_str_len_t key_len)
{
    // Return error if the length of the table is greater than its size.
    if (tbl->len > tbl->sz) return NULL;
    // Calculate the length of the key if key_len is 0.
    if (key_len == 0) key_len = (gn_str_len_t)strlen (key);
    // Return error if calculated key length is 0, because the hash table doesn't support empty keys.
    if (key_len == 0) return NULL;

    // Compute the key hash and get a table index.
    const gn_htbl_len_t index = gn_htbl_hash (tbl->sz, key, key_len);
    printf ("Searching key at index: %u. Key: \"%s\"\n", index, key); // TODO: Remove.

    if (tbl->itms[index] == NULL)
    {
        // There's no hash table item/key at computed index.
        fprintf (stderr, "Key \"%s\" not found at index %u.\n", key, index);
    }
    else
    {
        // There's a hash table item/key at computed index.
        // Is the first item the one we have to find ?
        gn_htbl_item_t * next_item = tbl->itms[index];
        if (next_item->key_len == key_len && strncmp (next_item->key, key, next_item->key_len) == 0)
        {
            // Yes, the key we have to find is the first at this index.
            printf ("FOUND '%s'\n", next_item->val); // TODO: Remove.
            return next_item;
        }
        else
        {
            // No, the key we have to find is not the first at this index.
            // Find the key in the list. Start at the second item.
            next_item = next_item->next;
            for ( ; next_item != NULL; next_item = next_item->next)
            {
                printf ("\t.key (%u) \"%s\", .val (%u) \"%s\"\n",
                        next_item->key_len, next_item->key,
                        next_item->val_len, next_item->val);
                if (next_item->key_len == key_len && strncmp (next_item->key, key, next_item->key_len) == 0)
                {
                    printf ("FOUND '%s'\n", next_item->val); // TODO: Remove.
                    // We found the key. Now return it.
                    return next_item;
                }
            }
            // We didn't find the key.
        }
    }

    return NULL;
}