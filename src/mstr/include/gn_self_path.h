#ifndef GN_SELF_PATH_H
#define GN_SELF_PATH_H

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

__attribute__((warn_unused_result))
char *
gn_self_path (void);

#endif // GN_SELF_PATH_H