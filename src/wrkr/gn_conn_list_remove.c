#include <gn_conn_list_remove.h>

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