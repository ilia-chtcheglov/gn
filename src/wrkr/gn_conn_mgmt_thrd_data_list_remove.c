#include <gn_conn_mgmt_thrd_data_list_remove.h>

void
gn_conn_mgmt_thrd_data_list_remove (gn_conn_mgmt_thrd_data_list_t * const list,
                                    gn_conn_mgmt_thrd_data_t * const data)
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
            data->prev->next = data->next;
            data->next->prev = data->prev;
            if (data == list->head) list->head = data->next;
            else if (data == list->tail) list->tail = data->prev;
        }
    }

    list->len--;
    return;
}