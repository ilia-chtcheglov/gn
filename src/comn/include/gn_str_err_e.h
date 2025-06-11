#ifndef GN_STR_ERR_E_H
#define GN_STR_ERR_E_H

typedef enum
{
    GN_STR_ERR_NONE,
    GN_STR_ERR_INTERN_DAT,
    GN_STR_ERR_INTERN_LEN,
    GN_STR_ERR_INTERN_SZ,
    GN_STR_ERR_ALLOC,
    GN_STR_ERR_LEN,
    GN_STR_ERR_INVAL_RANGE
} gn_str_err_e;

#endif // GN_STR_ERR_E_H