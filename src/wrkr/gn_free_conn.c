#include <gn_free_conn.h>

void
gn_free_conn (gn_conn_t ** const conn)
{
    if (conn == NULL) return;

    gn_close (&(*conn)->fd);
    free ((*conn)->hdrv);
    free ((*conn)->hdrn);
    free ((*conn)->prot);
    free ((*conn)->uri);
    free ((*conn)->mthd);
    free ((*conn)->recv_buf);
    free ((*conn)->saddr);
    gn_htbl_free (&(*conn)->req_hdrs);

    free (*conn);
    *conn = NULL;
}