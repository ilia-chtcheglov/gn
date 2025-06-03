#include <gn_close_conn.h>

void
gn_close_conn (gn_conn_t * const conn)
{
    gn_close (&conn->fd);
    gn_close (&conn->sock);
    gn_htbl_free (&conn->req_hdrs);
    gn_str_deinit (&conn->hdrv);
    gn_str_deinit (&conn->hdrn);
    gn_str_deinit (&conn->prot);
    gn_str_deinit (&conn->uri);
    gn_str_deinit (&conn->mthd);
    gn_str_deinit (&conn->send_buf);
    gn_str_deinit (&conn->recv_buf);
    free (conn->saddr);
    conn->saddr = NULL;
}