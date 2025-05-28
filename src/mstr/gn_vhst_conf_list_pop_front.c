#include <gn_vhst_conf_list_pop_front.h>

__attribute__((nonnull))
__attribute__((warn_unused_result))
gn_vhst_conf_t *
gn_vhst_conf_list_pop_front (gn_vhst_conf_list_t * const list)
{
    gn_vhst_conf_t * const this_ret = list->head;

    switch (list->len)
    {
        case 0:
            return NULL;
        case 1:
        {
            list->head = list->tail = NULL;
            break;
        }
        default:
        {
            list->head = list->head->next;

            list->head->prev = list->tail;
            list->tail->next = list->head;
        }
    }

    list->len--;
    return this_ret;
}