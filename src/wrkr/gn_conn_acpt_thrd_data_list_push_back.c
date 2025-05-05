#include <gn_conn_acpt_thrd_data_list_push_back.h>

__attribute__((warn_unused_result))
int
gn_conn_acpt_thrd_data_list_push_back (gn_conn_acpt_thrd_data_list_t * const list,
                                       gn_conn_acpt_thrd_data_t * const data)
{
    switch (list->len)
    {
        case 0:
        {
            list->head = list->tail = data->prev = data->next = data;
            break;
        }
        case UINT8_MAX:
        {
            return 1;
        }
        default:
        {
            list->tail->next = data;
            data->prev = list->tail;
            list->head->prev = data;
            data->next = list->head;
            list->tail = data;
        }
    }

    list->len++;
    return 0;
}