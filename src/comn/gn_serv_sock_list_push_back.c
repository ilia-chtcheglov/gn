#include <gn_serv_sock_list_push_back.h>

int
gn_serv_sock_list_push_back (gn_serv_sock_list_t * const list, gn_serv_sock_t * const sock)
{
    switch (list->len)
    {
        case 0:
        {
            list->head = list->tail = sock->prev = sock->next = sock;
            break;
        }
        case UINT16_MAX:
        {
            return 1;
        }
        default:
        {
            list->tail->next = sock;
            sock->prev = list->tail;
            list->head->prev = sock;
            sock->next = list->head;
            list->tail = sock;
        }
    }

    list->len++;
    return 0;
}