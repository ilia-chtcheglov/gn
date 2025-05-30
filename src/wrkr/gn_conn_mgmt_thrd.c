#include <gn_conn_mgmt_thrd.h>

void
gn_close_conn (gn_conn_t * const conn);

__attribute__((nonnull))
__attribute__((warn_unused_result))
bool
gn_process_conn (gn_conn_t * const conn);

void *
gn_conn_mgmt_thrd (void * const p)
{
    gn_conn_mgmt_thrd_data_t * const data = (gn_conn_mgmt_thrd_data_t *)p;
    atomic_store_explicit (&data->state, GN_CONN_MGMT_THRD_STATE_RUNNING, memory_order_release);

    // Connections list.
    gn_conn_list_t conn_list = { 0 };

    bool stop = false;
    bool main_loop = true;
    while (main_loop)
    {
        // Process connections one by one.
        gn_conn_t * conn = conn_list.head;
        for (uint32_t i = 0; i < conn_list.len; i++)
        {
            printf ("Processing connection %p\n", (void *)conn);
            if (stop) conn->step = GN_CONN_STEP_CLOSE;
            if (!gn_process_conn (conn))
            {
                conn = conn->next;
                continue;
            }

            gn_conn_list_remove (&conn_list, conn);
            gn_conn_t * next_conn;
            if (conn_list.len > 0) next_conn = conn->next;
            else next_conn = NULL;

            printf ("Freeing connection %p\n", (void *)conn);
            gn_free_conn (&conn);
            conn = next_conn;
        }

        gn_get_new_conns (data, &conn_list, stop);

        /*
         * If the local stop variable isn't true we check the atomic stop variable.
         * If the atomic one is true we set the local one to true and signal that the thread is stopping.
         * If the thread doesn't have to stop it waits 100 milliseconds to avoid 100% CPU usage.
         */
        if (!stop)
        {
            if (atomic_load_explicit (&data->stop, memory_order_relaxed))
            {
                printf ("Connection management thread %lu stopping.\n", data->tid);
                atomic_store_explicit (&data->state, GN_CONN_MGMT_THRD_STATE_STOPPING, memory_order_release);
                stop = true;
            }
            else gn_sleep_ms (100);
        }
        else if (conn_list.len == 0)
        {
            // If the thread has to stop and the connection list is empty we leave the main loop.
            main_loop = false;
            break;
        }
    }

    // Signal that the thread is stopped.
    atomic_store_explicit (&data->state, GN_CONN_MGMT_THRD_STATE_STOPPED, memory_order_release);
    return NULL;
}