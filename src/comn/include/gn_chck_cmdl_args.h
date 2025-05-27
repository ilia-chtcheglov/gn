#ifndef GN_CHCK_CMDL_ARGS_H
#define GN_CHCK_CMDL_ARGS_H

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define GN_CMDL_ARGS_MIN 1 // Minimum number of command line arguments.
#define GN_CMDL_ARGS_MAX 3 // Maximum number of command line arguments.

__attribute__((nonnull))
__attribute__((warn_unused_result))
bool
gn_chck_cmdl_args (const int argc,
                   const char * const * const argv);

#endif