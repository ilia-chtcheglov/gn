#include <gn_start_wrkrs.h>

void
gn_start_wrkrs (gn_wrkr_data_list_t * const wrkr_data_list, const uint8_t num_workers, const char * const path,
                int ipc_sock, const char * const ipc_addr_str, gn_serv_sock_list_t * const serv_sock_list)
{
    for (uint8_t i = 0; i < num_workers; i++)
    {
        if (wrkr_data_list->len == UINT8_MAX)
        {
            fprintf (stderr, "Worker data list is full.\n");
            continue;
        }

        gn_wrkr_data_t * wrkr_data = (gn_wrkr_data_t *)malloc (sizeof (gn_wrkr_data_t));
        if (wrkr_data == NULL)
        {
            fprintf (stderr, "Failed to allocate structure for worker process data.\n");
            continue;
        }

        wrkr_data->ipc_sock = -1;
        wrkr_data->pid = -1;

        if (gn_start_wrkr (wrkr_data, path, ipc_sock, ipc_addr_str, serv_sock_list) != EXIT_SUCCESS)
        {
            free (wrkr_data);
            wrkr_data = NULL;
            continue;
        }

        printf ("Worker PID: %i, IPC socket: %i.\n", wrkr_data->pid, wrkr_data->ipc_sock);
        /*
         * No need to check return value. This function returns an error
         * if the list is full. The error check was done above.
         */
        (void)! gn_wrkr_data_list_push_back (wrkr_data_list, wrkr_data);
        wrkr_data = NULL;
    }
}