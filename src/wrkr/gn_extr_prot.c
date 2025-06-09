#include <gn_extr_prot.h>

__attribute__((nonnull))
void
gn_extr_prot (gn_conn_t * const conn)
{
    // Extract the request protocol from conn->recv_buf and store it in conn->prot.
    gn_str_len_t recv_buf_i = 0;
    while (recv_buf_i < conn->recv_buf.len && conn->prot.len < conn->prot.sz - 1)
    {
        if (conn->recv_buf.dat[recv_buf_i] == '\n') break;

        conn->prot.dat[conn->prot.len++] = conn->recv_buf.dat[recv_buf_i++];
    }
    conn->prot.dat[conn->prot.len] = '\0';

    if (conn->recv_buf.dat[recv_buf_i] == '\n')
    {
        // Move the rest of the data to the beginning of the receive buffer.
        recv_buf_i = 0;
        gn_str_len_t i = conn->prot.len + 1;
        while (i < conn->recv_buf.len) conn->recv_buf.dat[recv_buf_i++] = conn->recv_buf.dat[i++];

        conn->recv_buf.len -= conn->prot.len + 1;
        conn->recv_buf.dat[conn->recv_buf.len] = '\0';

        conn->prev_step = GN_CONN_STEP_INVALID;
        /*
         * From RFC 9112 2.2 Message Parsing:
         * Although the line terminator for the start-line and fields is
         * the sequence CRLF, a recipient MAY recognize a single LF as
         * a line terminator and ignore any preceding CR.
         */
        // Remove CR from the end of the request protocol buffer.
        if (conn->prot.len > 0 && conn->prot.dat[conn->prot.len - 1] == '\r')
        {
            conn->prot.len--;
            conn->prot.dat[conn->prot.len] = '\0';
        }

        if (conn->prot.len == 0)
        {
            // TODO: Maybe log error.
            conn->status = 400;
            conn->step = GN_CONN_STEP_WRIT_HDRS;
            return;
        }

        conn->step = GN_CONN_STEP_EXTR_HDRN;
        return;
    }

    if (conn->prot.len == conn->prot.sz - 1)
    {
        // TODO: Maybe log error.
        conn->status = 400;
        conn->prev_step = GN_CONN_STEP_INVALID;
        conn->step = GN_CONN_STEP_WRIT_HDRS;
        return;
    }

    conn->prev_step = GN_CONN_STEP_EXTR_PROT;
    conn->step = GN_CONN_STEP_RECV_DATA;
}