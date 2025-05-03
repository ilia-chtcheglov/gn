#include <gn_conn_mgmt_thrd.h>

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
        printf ("len: %u\n", conn_list.len);
        if (data->conn != NULL)
        {
            // TODO: Push connection to @conn_list.
            if (gn_conn_list_push_back (&conn_list, data->conn) != 0)
            {
                close (data->conn->fd);
                free (data->conn->saddr);
                free (data->conn); // TODO: Remove.
            }
            data->conn = NULL;
        }

        if (conn_list.len == UINT32_MAX) main_loop = false;

        sleep (1);
    }

    return NULL;
}