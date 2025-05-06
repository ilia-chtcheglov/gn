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


        const int rpthread_create = pthread_create (&data->tid, NULL, gn_conn_mgmt_thrd, data);
        switch (rpthread_create)
        {
            case 0:
            {
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