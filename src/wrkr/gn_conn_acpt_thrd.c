#include <gn_conn_acpt_thrd.h>

#include <unistd.h>

void *
gn_conn_acpt_thrd (void * const p)
{
    gn_conn_acpt_thrd_data_t * const data = (gn_conn_acpt_thrd_data_t *)p;
    (void)data; // TODO: Remove.


    bool main_loop = true;
    while (main_loop)
    {
        sleep (1);
        // TODO: Remove.
        main_loop = false;
    }

    return NULL;
}