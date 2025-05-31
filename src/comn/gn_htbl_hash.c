#include <gn_htbl_hash.h>

__attribute__((nonnull))
__attribute__((warn_unused_result))
gn_htbl_len_t
gn_htbl_hash (const gn_htbl_len_t size,
              const char * const key,
              const uint32_t key_len)
{
    gn_htbl_len_t hash = 0;
    for (uint32_t i = 0; i < key_len; i++)
    {
        hash += (gn_htbl_len_t)((double)key[i] * pow (10, (double)i));
    }

    return hash % size;
}