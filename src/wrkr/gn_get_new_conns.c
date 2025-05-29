#include <gn_get_new_conns.h>

__attribute__((nonnull))
void
gn_get_new_conns (gn_conn_mgmt_thrd_data_t * const data,
                  gn_conn_list_t * const conn_list,
                  const bool stop)
{
    /*
     * Get new connections from new_conns and add them to the connections list.
     * new_conns is an array of atomic_uintptr_t variables.
     * no_new_conn is an atomic flag variable which is false when a new connection
     * was placed in new_conns.
     */
    if (!atomic_flag_test_and_set (&data->no_new_conn))
    {
        for (int8_t i = sizeof (data->new_conns) / sizeof (atomic_uintptr_t) - 1; i > -1 ; i--)
        {
            // If there's no new connection at this index, try next index.
            if (atomic_load_explicit (&data->new_conns[i], memory_order_relaxed) == (uintptr_t)NULL) continue;

            // Turn the atomic_uintptr_t value into a pointer to a connection structure.
            gn_conn_t * new_conn = (gn_conn_t *)data->new_conns[i];
            // Close the connection if the thread has to stop or the connections list is full.
            if (stop || gn_conn_list_push_back (conn_list, new_conn) != 0)
            {
                gn_free_conn (&new_conn);
            }

            // Make the array index available for a new connection.
            atomic_store_explicit (&data->new_conns[i], (uintptr_t)NULL, memory_order_relaxed);
        }
    }
}