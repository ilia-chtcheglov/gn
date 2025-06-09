#include <gn_extr_mthd.h>

__attribute__((nonnull))
void
gn_extr_mthd (gn_conn_t * const conn)
{
    // Extract the request method from conn->recv_buf and store it in conn->mthd.
    gn_str_len_t recv_buf_i = 0;
    while (recv_buf_i < conn->recv_buf.len && conn->mthd.len < conn->mthd.sz - 1)
    {
        if (conn->recv_buf.dat[recv_buf_i] == ' ') break;

        conn->mthd.dat[conn->mthd.len++] = conn->recv_buf.dat[recv_buf_i++];
    }
    conn->mthd.dat[conn->mthd.len] = '\0';

    if (conn->recv_buf.dat[recv_buf_i] == ' ')
    {
        // Move the rest of the data to the beginning of the receive buffer.
        recv_buf_i = 0;
        gn_str_len_t i = conn->mthd.len + 1;
        while (i < conn->recv_buf.len) conn->recv_buf.dat[recv_buf_i++] = conn->recv_buf.dat[i++];

        conn->recv_buf.len -= conn->mthd.len + 1;
        conn->recv_buf.dat[conn->recv_buf.len] = '\0';

        conn->prev_step = GN_CONN_STEP_INVALID;
        if (conn->mthd.len == 0)
        {
            // TODO: Maybe log error.
            conn->status = 400;
            conn->step = GN_CONN_STEP_WRIT_HDRS;
            return;
        }

        if (strcmp (conn->mthd.dat, "GET") == 0 ||
            strcmp (conn->mthd.dat, "POST") == 0 ||
            strcmp (conn->mthd.dat, "HEAD") == 0)
        {
            conn->step = GN_CONN_STEP_EXTR_URI;
            return;
        }

        // TODO: Maybe log error.
        conn->status = 501;
        conn->step = GN_CONN_STEP_WRIT_HDRS;
        return;
    }

    if (conn->mthd.len == conn->mthd.sz - 1)
    {
        // TODO: Maybe log error.
        conn->status = 400; // TODO: Return 400 Bad Request or 501 Not Implemented?
        conn->prev_step = GN_CONN_STEP_INVALID;
        conn->step = GN_CONN_STEP_WRIT_HDRS;
        return;
    }

    conn->prev_step = GN_CONN_STEP_EXTR_MTHD;
    conn->step = GN_CONN_STEP_RECV_DATA;
}