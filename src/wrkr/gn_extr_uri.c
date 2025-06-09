#include <gn_extr_uri.h>

__attribute__((nonnull))
void
gn_extr_uri (gn_conn_t * const conn)
{
    // Extract the request URI from conn->recv_buf and store it in conn->uri.
    gn_str_len_t recv_buf_i = 0;
    while (recv_buf_i < conn->recv_buf.len && conn->uri.len < conn->uri.sz - 1)
    {
        if (conn->recv_buf.dat[recv_buf_i] == ' ') break;

        conn->uri.dat[conn->uri.len++] = conn->recv_buf.dat[recv_buf_i++];
    }
    conn->uri.dat[conn->uri.len] = '\0';

    if (conn->recv_buf.dat[recv_buf_i] == ' ')
    {
        // Move the rest of the data to the beginning of the receive buffer.
        recv_buf_i = 0;
        gn_str_len_t i = conn->uri.len + 1;
        while (i < conn->recv_buf.len) conn->recv_buf.dat[recv_buf_i++] = conn->recv_buf.dat[i++];

        conn->recv_buf.len -= conn->uri.len + 1;
        conn->recv_buf.dat[conn->recv_buf.len] = '\0';

        conn->prev_step = GN_CONN_STEP_INVALID;
        if (conn->uri.len == 0)
        {
            // TODO: Maybe log error.
            conn->status = 400;
            conn->step = GN_CONN_STEP_WRIT_HDRS;
            return;
        }

        conn->step = GN_CONN_STEP_EXTR_PROT;
        return;
    }

    /*
     * From RFC 9112 3. Request Line:
     * A server that receives a request-target longer than any URI it
     * wishes to parse MUST respond with a 414 (URI Too Long) status code.
     */
    if (conn->uri.len == conn->uri.sz - 1)
    {
        // TODO: Maybe log error.
        conn->status = 414;
        conn->prev_step = GN_CONN_STEP_INVALID;
        conn->step = GN_CONN_STEP_WRIT_HDRS;
        return;
    }

    conn->prev_step = GN_CONN_STEP_EXTR_URI;
    conn->step = GN_CONN_STEP_RECV_DATA;
}