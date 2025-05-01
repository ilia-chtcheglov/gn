#include <gn_conn_mgmt_thrd.h>

void *
gn_conn_mgmt_thrd (void * const p)
{
    (void)p; // TODO: Remove.
    printf ("Thread started.\n");

    bool main_loop = true;
    while (main_loop)
    {
        main_loop = false;
    }

    return NULL;
}