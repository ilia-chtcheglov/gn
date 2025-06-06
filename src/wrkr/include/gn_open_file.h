#ifndef GN_OPEN_FILE_H
#define GN_OPEN_FILE_H

#include <gn_conn_t.h>

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

__attribute__((nonnull))
void
gn_open_file (gn_conn_t * const conn);

#endif // GN_OPEN_FILE_H