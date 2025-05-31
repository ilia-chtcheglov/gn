#include <gn_close_conn.h>

void
gn_close_conn (gn_conn_t * const conn)
{
    gn_close (&conn->fd);
    free (conn->hdrv);
    conn->hdrv = NULL;
    free (conn->hdrn);
    conn->hdrn = NULL;
    free (conn->prot);
    conn->prot = NULL;
    free (conn->uri);
    conn->uri = NULL;
    free (conn->mthd);
    conn->mthd = NULL;
    free (conn->recv_buf);
    conn->recv_buf = NULL;
    free (conn->saddr);
    conn->saddr = NULL;
    gn_htbl_free (&conn->req_hdrs);
}