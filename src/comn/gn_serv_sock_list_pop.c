#include <gn_serv_sock_list_pop.h>

__attribute__((warn_unused_result))
gn_serv_sock_t *
gn_serv_sock_list_pop (gn_serv_sock_list_t * const list)
{
    switch (list->len)
    {
        case 0:
        {
            return NULL;
        }
        case 1:
        {
            gn_serv_sock_t * const ret = list->head;
            list->head = list->tail = NULL;
            list->len--;
            return ret;
        }
        default:
        {
            gn_serv_sock_t * const ret = list->head;
            list->head = list->head->next;

            list->head->prev = list->tail;
            list->tail->next = list->head;

            list->len--;
            return ret;
        }
    }
}