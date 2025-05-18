#include <gn_stop_wrkrs.h>

void
gn_stop_wrkrs (gn_wrkr_data_list_t * const list)
{
    while (list->len > 0)
    {
        gn_wrkr_data_t * data = gn_wrkr_data_list_pop (list);
        if (data == NULL) break;

        gn_close (&data->ipc_sock);
        free (data);
        data = NULL;
    }
}