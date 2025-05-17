#include <gn_stop_conn_mgmt_thrds.h>

void
gn_stop_conn_mgmt_thrds (gn_conn_mgmt_thrd_data_list_t * const list)
{
    while (list->len > 0)
    {
        gn_conn_mgmt_thrd_data_t * data = list->head;
        for (uint8_t i = 0; i < list->len; i++)
        {
            gn_conn_mgmt_thrd_state_e state =
                (gn_conn_mgmt_thrd_state_e)atomic_load_explicit (&data->state, memory_order_relaxed);
            switch (state)
            {
                case GN_CONN_MGMT_THRD_STATE_STARTING:
                case GN_CONN_MGMT_THRD_STATE_RUNNING:
                {
                    printf ("Stopping connection management thread %lu.\n", data->tid);
                    atomic_store_explicit (&data->stop, true, memory_order_relaxed);
                    data = data->next;
                    break;
                }
                case GN_CONN_MGMT_THRD_STATE_STOPPING:
                {
                    data = data->next;
                    break;
                }
                case GN_CONN_MGMT_THRD_STATE_STOPPED:
                {
                    gn_conn_mgmt_thrd_data_list_remove (list, data);
                    gn_conn_mgmt_thrd_data_t * next_data = NULL;
                    if (list->len > 0) next_data = data->next;

                    free (data);
                    if (next_data != NULL) data = next_data;
                    else data = NULL;

                    i--;
                    break;
                }
                default:
                {
                    fprintf (stderr, "Invalid connection management thread state %u.\n", state);
                    atomic_store_explicit (&data->stop, true, memory_order_relaxed);
                    data = data->next;
                }
            }
        }

        sleep (1);
    }
}