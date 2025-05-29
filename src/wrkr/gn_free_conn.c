#include <gn_free_conn.h>

void
gn_free_conn (gn_conn_t ** const conn)
{
    if (conn == NULL) return;

    gn_close (&(*conn)->fd);
    free ((*conn)->uri);
    free ((*conn)->mthd);
    free ((*conn)->recv_buf);
    free ((*conn)->saddr);

    free (*conn);
    *conn = NULL;
}