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

void *
gn_conn_mgmt_thrd (void * const p)
{
    gn_conn_mgmt_thrd_data_t * const data = (gn_conn_mgmt_thrd_data_t *)p;

    gn_conn_list_t conn_list;
    memset (&conn_list, 0, sizeof (conn_list));

    bool main_loop = true;
    while (main_loop)
    {
        gn_conn_t * conn = conn_list.head;
        for (uint32_t i = 0; i < conn_list.len; conn = conn->next, i++)
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
        }

        for (int8_t i = sizeof (data->new_conns) / sizeof (atomic_uintptr_t) - 1; i > -1 ; i--)
        {
            if (data->new_conns[i] == (uintptr_t)NULL) continue;

            gn_conn_t * const new_conn = (gn_conn_t *)data->new_conns[i];
            if (gn_conn_list_push_back (&conn_list, new_conn) != 0)
            {
                close (new_conn->fd);
                free (new_conn->saddr);
                free (new_conn);
            }
            data->new_conns[i] = (uintptr_t)NULL;
        }

        sleep (1);
        // TODO: Remove.
        if (data->tid == 0) return NULL;
        if (conn_list.len == UINT32_MAX) main_loop = false;
    }

    return NULL;
}