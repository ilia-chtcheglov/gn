#ifndef GN_SET_SIG_HNDLR_H
#define GN_SET_SIG_HNDLR_H

#include <errno.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

__attribute__((warn_unused_result))
bool
gn_set_sig_hndlr (const int signum, const __sighandler_t hndlr);

#endif // GN_SET_SIG_HNDLR_H