#ifndef GN_WRKR_MAIN_H
#define GN_WRKR_MAIN_H

#include <errno.h>
#include <poll.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

void
gn_wrkr_main (const int ipc_sock);

#endif // GN_WRKR_MAIN_H