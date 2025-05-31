#include <gn_conn_list_push_back.h>

__attribute__((nonnull))
__attribute__((warn_unused_result))
bool
gn_conn_list_push_back (gn_conn_list_t * const list, gn_conn_t * const conn)
{
    switch (list->len)
    {
        case GN_CONN_LIST_LEN_MAX:
            return EXIT_FAILURE;
        case 0:
        {
            list->head = list->tail = conn->prev = conn->next = conn;
            break;
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
    return EXIT_SUCCESS;
}