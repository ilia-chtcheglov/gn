#include <gn_htbl_hash.h>

__attribute__((nonnull))
__attribute__((pure))
__attribute__((warn_unused_result))
gn_htbl_len_t
gn_htbl_hash (const gn_htbl_len_t size,
              const char * const key,
              const gn_str_len_t key_len)
{
    size_t hash = 0;
    for (gn_str_len_t i = 0; i < key_len; i++)
    {
        hash = hash * 31 + (unsigned char)key[i];
    }

    return (gn_htbl_len_t)hash % size;
}