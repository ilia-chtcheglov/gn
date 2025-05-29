#include <gn_conn_mgmt_thrd.h>

#include <arpa/inet.h> // TODO: Remove.

void
gn_recv_data (gn_conn_t * const conn);

void
gn_recv_data (gn_conn_t * const conn)
{
    const ssize_t rrecv = recv (conn->fd, &conn->recv_buf[conn->recv_buf_len],
                                conn->recv_buf_sz - conn->recv_buf_len - 1, 0);
    if (rrecv > 0)
    {
        conn->recv_buf_len += (uint32_t)rrecv;
        conn->recv_buf[conn->recv_buf_len] = '\0';
        printf ("Received (%u) \"%s\"\n", conn->recv_buf_len, conn->recv_buf);
    }
}

void
gn_process_conn (gn_conn_t * const conn);

void
gn_process_conn (gn_conn_t * const conn)
{
    switch (conn->step)
    {
        case GN_CONN_STEP_RECV_DATA:
        {
            gn_recv_data (conn);
            break;
        }
        default:
        {
            fprintf (stderr, "Invalid connection step %u.\n", conn->step);
        }
    }
}

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
        gn_conn_t * next_conn = NULL;
        for (uint32_t i = 0; i < conn_list.len; i++)
        {
            gn_process_conn (conn);

            // If the thread has to stop we close and remove the connection from the list.
            if (!stop) next_conn = conn->next;
            else
            {
                gn_conn_list_remove (&conn_list, conn);
                if (conn_list.len > 0) next_conn = conn->next;
                else next_conn = NULL;

                gn_free_conn (&conn);
            }

            conn = next_conn;
        }

        gn_get_new_conns (data, &conn_list, stop);

        /*
         * If the local stop variable isn't true we check the atomic stop variable.
         * If the atomic one is true we set the local one to true and signal that the thread is stopping.
         * If the thread doesn't have to stop we wait 1 second to avoid 100% CPU usage.
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