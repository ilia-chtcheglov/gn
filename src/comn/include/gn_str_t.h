#ifndef GN_STR_T_H
#define GN_STR_T_H

#include <gn_str_err_e.h>

#include <stdint.h>

typedef uint32_t gn_str_len_t;
#define GN_STR_SZ_MAX UINT32_MAX
#define GN_STR_LEN_MAX GN_STR_SZ_MAX - 1

typedef struct
{
    char *       dat;
    gn_str_len_t len;
    gn_str_len_t sz;
} gn_str_t;

#endif // GN_STR_T_H