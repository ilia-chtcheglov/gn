#include <gn_htbl_insr.h>

__attribute__((nonnull))
__attribute__((warn_unused_result))
bool
gn_htbl_insr (gn_htbl_t * const tbl,
              const char * const key,
              gn_str_len_t key_len,
              const char * const val,
              const gn_str_len_t val_len)
{
    // Return error if the hash table is full.
    if (tbl->len >= tbl->sz)
    {
        fprintf (stderr, "Hash table is full.\n");
        return true;
    }
    // If the given key length is 0, calculate the length with strlen().
    if (key_len == 0) key_len = (gn_str_len_t)strlen (key); // TODO: Check length before cast.

    const gn_htbl_len_t index = gn_htbl_hash (tbl->sz, key, key_len);
    printf ("Index: %u, Key: \"%s\"\n", index, key);

    if (tbl->itms[index] == NULL)
    {
        gn_htbl_item_t * const new_item = (gn_htbl_item_t *)malloc (sizeof (gn_htbl_item_t));
        if (new_item == NULL)
        {
            fprintf (stderr, "Failed to allocate hash table item.\n");
            return true;
        }

        gn_htbl_item_init (new_item);
        new_item->key = (char *)malloc (key_len + 1);
        if (new_item->key != NULL)
        {
            new_item->val = (char *)malloc (val_len + 1);
            if (new_item->val != NULL)
            {
                strncpy (new_item->key, key, key_len);
                new_item->key[key_len] = '\0';
                new_item->key_len = key_len;

                strncpy (new_item->val, val, val_len);
                new_item->val[val_len] = '\0';
                new_item->val_len = val_len;

                new_item->prev = new_item;
                tbl->itms[index] = new_item;
                tbl->len++;
                return false;
            }
            free (new_item->key);
        }
        free (new_item);
        return true;
    }
    else
    {
        printf ("Collision!\n");
        printf ("Searching existing key...\n");
        gn_htbl_item_t * next_item = tbl->itms[index];
        for ( ; next_item != NULL; next_item = next_item->next)
        {
            printf ("\t.key (%u) \"%s\", .val (%u) \"%s\"\n",
                    next_item->key_len, next_item->key,
                    next_item->val_len, next_item->val);
            if (next_item->key_len == key_len && strncmp (next_item->key, key, next_item->key_len) == 0)
            {
                printf ("FOUND\n");
                char * const new_val = (char *)malloc (val_len + 1);
                if (new_val != NULL)
                {
                    strncpy (new_val, val, val_len);
                    new_val[val_len] = '\0';
                    free (next_item->val);
                    next_item->val = new_val;
                    next_item->val_len = val_len;
                    return false;
                }
                else
                {
                    fprintf (stderr, "Failed to allocate new value.\n");
                    return true;
                }
            }
        }

        gn_htbl_item_t * const new_item = (gn_htbl_item_t *)malloc (sizeof (gn_htbl_item_t));
        if (new_item == NULL)
        {
            fprintf (stderr, "Failed to allocate hash table item.\n");
            return true;
        }

        gn_htbl_item_init (new_item);
        new_item->key = (char *)malloc (key_len + 1);
        if (new_item->key != NULL)
        {
            new_item->val = (char *)malloc (val_len + 1);
            if (new_item->val != NULL)
            {
                strncpy (new_item->key, key, key_len);
                new_item->key[key_len] = '\0';
                new_item->key_len = key_len;

                strncpy (new_item->val, val, val_len);
                new_item->val[val_len] = '\0';
                new_item->val_len = val_len;

                gn_htbl_item_t * const item = tbl->itms[index];

                item->prev->next = new_item;
                new_item->prev = item->prev;
                item->prev = new_item;
                return false;
            }
            free (new_item->key);
        }
        free (new_item);
        return true;
    }

    return true;
}