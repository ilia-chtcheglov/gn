#include <gn_vhst_conf_list_pop.h>

__attribute__((nonnull))
__attribute__((warn_unused_result))
gn_vhst_conf_t *
gn_vhst_conf_list_pop (gn_vhst_conf_list_t * const list)
{
    switch (list->len)
    {
        case 0:
        {
            return NULL;
        }
        case 1:
        {
            gn_vhst_conf_t * const ret = list->head;
            list->head = list->tail = NULL;
            list->len--;
            return ret;
        }
        default:
        {
            gn_vhst_conf_t * const ret = list->head;
            list->head = list->head->next;

            list->head->prev = list->tail;
            list->tail->next = list->head;

            list->len--;
            return ret;
        }
    }
}