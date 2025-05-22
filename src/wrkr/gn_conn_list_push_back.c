#include <gn_conn_list_push_back.h>

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