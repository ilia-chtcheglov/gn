#include <gn_start_conn_mgmt_thrds.h>

void
gn_start_conn_mgmt_thrds (const uint8_t num, gn_conn_mgmt_thrd_data_list_t * const conn_mgmt_thrd_data_list)
{
    for (uint8_t i = 0; i < num; i++)
    {
        gn_conn_mgmt_thrd_data_t * data = (gn_conn_mgmt_thrd_data_t *)malloc (sizeof (gn_conn_mgmt_thrd_data_t));
        if (data != NULL) memset (data, 0, sizeof (gn_conn_mgmt_thrd_data_t));
        else
        {
            fprintf (stderr, "Failed to allocate structure for connection management thread data.\n");
            continue;
        }

        atomic_store_explicit (&data->stop, false, memory_order_relaxed);
        atomic_store_explicit (&data->state, GN_CONN_MGMT_THRD_STATE_STARTING, memory_order_relaxed);
        atomic_flag_clear (&data->no_new_conn); // Initialize (set to false).
        atomic_flag_test_and_set (&data->no_new_conn); // Set to true.

        const int rpthread_create = pthread_create (&data->tid, NULL, gn_conn_mgmt_thrd, data);
        switch (rpthread_create)
        {
            case 0:
            {
                const int rdetach = pthread_detach (data->tid);
                if (rdetach != 0)
                {
                    fprintf (stderr, "Failed to detach connection management thread. %s.\n", strerror (rdetach));
                    break;
                }

                // TODO: Don't ignore returned value.
                (void)! gn_conn_mgmt_thrd_data_list_push_back (conn_mgmt_thrd_data_list, data);
                break;
            }
            case EAGAIN:
            case EINVAL:
            case EPERM:
            {
                fprintf (stderr, "Failed to create connection management thread. %s.\n", strerror (rpthread_create));
                // TODO: Stop process on EINVAL or EPERM.
                break;
            }
            default:
            {
                fprintf (stderr, "pthread_create() returned unexpected value %i.\n", rpthread_create);
                // TODO: Stop process.
            }
        }
    }
}