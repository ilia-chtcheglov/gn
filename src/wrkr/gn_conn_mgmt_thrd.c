#include <gn_conn_mgmt_thrd.h>

#include <arpa/inet.h>
#include <unistd.h>

int
gn_conn_list_push_back (gn_conn_list_t * const list, gn_conn_t * const conn);

int
gn_conn_list_push_back (gn_conn_list_t * const list, gn_conn_t * const conn)
{
    switch (list->len)
    {
        case 0:
        {
            list->head = list->tail = conn->prev = conn->next = conn;
            break;
        }
        case UINT32_MAX:
        {
            return 1;
        }
        default:
        {
            list->tail->next = conn;
            conn->prev = list->tail;
            list->head->prev = conn;
            conn->next = list->head;
            list->tail = conn;
        }
    }

    list->len++;
    return 0;
}

void
gn_conn_list_remove (gn_conn_list_t * const list,
                     gn_conn_t * const conn);

void
gn_conn_list_remove (gn_conn_list_t * const list,
                     gn_conn_t * const conn)
{
    switch (list->len)
    {
        case 0:
            return;
        case 1:
        {
            list->head = list->tail = NULL;
            break;
        }
        default:
        {
            conn->prev->next = conn->next;
            conn->next->prev = conn->prev;
            if (conn == list->head) list->head = conn->next;
            else if (conn == list->tail) list->tail = conn->prev;
        }
    }

    list->len--;
    return;
}

void *
gn_conn_mgmt_thrd (void * const p)
{
    gn_conn_mgmt_thrd_data_t * const data = (gn_conn_mgmt_thrd_data_t *)p;
    atomic_store_explicit (&data->state, GN_CONN_MGMT_THRD_STATE_RUNNING, memory_order_release);

    // Connections list.
    gn_conn_list_t conn_list;
    memset (&conn_list, 0, sizeof (conn_list));

    bool stop = false;
    bool main_loop = true;
    while (main_loop)
    {
        // Process connections one by one.
        gn_conn_t * conn = conn_list.head;
        gn_conn_t * next_conn = NULL;
        for (uint32_t i = 0; i < conn_list.len; i++)
        {
            // Test code.
            char buf[1024];
            const ssize_t rrecv = recv (conn->fd, buf, sizeof (buf) - 1, 0);
            if (rrecv > 0)
            {
                const size_t buf_len = (size_t)rrecv;
                buf[buf_len] = '\0';
                printf ("Received (%lu) \"%s\"\n", buf_len, buf);
            }

            // If the thread has to stop we close and remove the connection from the list.
            if (stop)
            {
                gn_conn_list_remove (&conn_list, conn);
                if (conn_list.len > 0) next_conn = conn->next;
                else next_conn = NULL;

                gn_close (&conn->fd);
                free (conn->saddr);
                free (conn);
                conn = NULL;
            }
            else
            {
                next_conn = conn->next;
            }

            conn = next_conn;
        }

        /*
         * Get new connections from new_conns and add them to the connections list.
         * new_conns is an array of atomic_uintptr_t variables.
         * no_new_conn is an atomic flag variable which is false when a new connection
         * was placed in new_conns.
         */
        if (!atomic_flag_test_and_set (&data->no_new_conn))
        {
            for (int8_t i = sizeof (data->new_conns) / sizeof (atomic_uintptr_t) - 1; i > -1 ; i--)
            {
                // If there's no new connection at this index, try next index.
                if (atomic_load_explicit (&data->new_conns[i], memory_order_relaxed) == (uintptr_t)NULL) continue;

                // Turn the atomic_uintptr_t value into a pointer to a connection structure.
                gn_conn_t * const new_conn = (gn_conn_t *)data->new_conns[i];
                // Close the connection if the thread has to stop or the connections list is full.
                if (stop || gn_conn_list_push_back (&conn_list, new_conn) != 0)
                {
                    gn_close (&new_conn->fd);
                    free (new_conn->saddr);
                    free (new_conn);
                }

                // Make the array index available for a new connection.
                atomic_store_explicit (&data->new_conns[i], (uintptr_t)NULL, memory_order_relaxed);
            }
        }

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
            else sleep (1);
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