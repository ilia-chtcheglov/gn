#ifndef GN_PRSE_CMDL_ARGS_H
#define GN_PRSE_CMDL_ARGS_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/un.h>

__attribute__((nonnull))
__attribute__((warn_unused_result))
bool
gn_prse_cmdl_args (const int argc,
                   const char * const * const argv,
                   const char ** const ipc_addr_str);

#endif // GN_PRSE_CMDL_ARGS_H