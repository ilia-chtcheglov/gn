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
    atomic_store_explicit (&data->state, GN_CONN_MGMT_THRD_STATE_RUNNING, memory_order_relaxed);

    gn_conn_list_t conn_list;
    memset (&conn_list, 0, sizeof (conn_list));

    bool stop = false;
    bool main_loop = true;
    while (main_loop)
    {
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

        for (int8_t i = sizeof (data->new_conns) / sizeof (atomic_uintptr_t) - 1; i > -1 ; i--)
        {
            if (data->new_conns[i] == (uintptr_t)NULL) continue;

            gn_conn_t * const new_conn = (gn_conn_t *)data->new_conns[i];
            if (stop || gn_conn_list_push_back (&conn_list, new_conn) != 0)
            {
                gn_close (&new_conn->fd);
                free (new_conn->saddr);
                free (new_conn);
            }
            data->new_conns[i] = (uintptr_t)NULL;
        }

        if (!stop)
        {
            if (atomic_load_explicit (&data->stop, memory_order_relaxed))
            {
                printf ("Connection management thread %lu stopping.\n", data->tid);
                atomic_store_explicit (&data->state, GN_CONN_MGMT_THRD_STATE_STOPPING, memory_order_relaxed);
                stop = true;
            }
            else sleep (1);
        }
        else if (conn_list.len == 0) break;
    }

    atomic_store_explicit (&data->state, GN_CONN_MGMT_THRD_STATE_STOPPED, memory_order_relaxed);
    return NULL;
}