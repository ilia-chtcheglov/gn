#include <gn_extr_hdrn.h>

__attribute__((nonnull))
void
gn_extr_hdrn (gn_conn_t * const conn)
{
    // Extract the header name from conn->recv_buf and store it in conn->hdrn.
    gn_str_len_t recv_buf_i = 0;
    while (recv_buf_i < conn->recv_buf.len && conn->hdrn.len < conn->hdrn.sz - 1)
    {
        if (conn->recv_buf.dat[recv_buf_i] == ':') break;
        else if (conn->recv_buf.dat[recv_buf_i] == '\n') break;

        /*
         * From RFC 9112 5. Field Syntax:
         * Each field line consists of a case-insensitive field name...
         */
        conn->hdrn.dat[conn->hdrn.len++] = (char)tolower ((unsigned char)conn->recv_buf.dat[recv_buf_i++]);
    }
    conn->hdrn.dat[conn->hdrn.len] = '\0';

    switch (conn->recv_buf.dat[recv_buf_i])
    {
        case ':':
        {
            // Move the rest of the data to the beginning of the receive buffer.
            (void)! gn_str_lshift (&conn->recv_buf, conn->hdrn.len + 1);

            conn->prev_step = GN_CONN_STEP_INVALID;
            if (conn->hdrn.len == 0)
            {
                // TODO: Maybe log error.
                conn->status = 400;
                conn->step = GN_CONN_STEP_WRIT_HDRS;
                return;
            }

            conn->step = GN_CONN_STEP_EXTR_HDRV;
            break;
        }
        case '\n':
        {
            // Move the rest of the data to the beginning of the receive buffer.
            (void)! gn_str_lshift (&conn->recv_buf, conn->hdrn.len + 1);

            // Remove CR from the end of the header name buffer.
            if (conn->hdrn.len > 0 && conn->hdrn.dat[conn->hdrn.len - 1] == '\r')
            {
                conn->hdrn.len--;
                conn->hdrn.dat[conn->hdrn.len] = '\0';
            }

            if (conn->hdrn.len > 0)
            {
                // We have a header name but it ends with \n.
                // TODO: Maybe log error.
                conn->status = 400;
                conn->step = GN_CONN_STEP_WRIT_HDRS;
                return;
            }

            gn_htbl_dump (&conn->req_hdrs); // TODO: Remove.

            /*
             * From RFC 9112 3.2 Request Target:
             * A server MUST respond with a 400 (Bad Request) status code to
             * any HTTP/1.1 request message that lacks a Host header field...
             */
            conn->prev_step = GN_CONN_STEP_INVALID;
            if (gn_htbl_srch (&conn->req_hdrs, "host", 0) == NULL)
            {
                conn->status = 400;
                conn->step = GN_CONN_STEP_WRIT_HDRS;
                return;
            }

            conn->step = GN_CONN_STEP_OPEN_FILE;
            break;
        }
        default:
        {
            if (conn->hdrn.len == conn->hdrn.sz - 1)
            {
                // TODO: Maybe log error.
                conn->status = 400;
                conn->prev_step = GN_CONN_STEP_INVALID;
                conn->step = GN_CONN_STEP_WRIT_HDRS;
                return;
            }

            conn->prev_step = GN_CONN_STEP_EXTR_HDRN;
            conn->step = GN_CONN_STEP_RECV_DATA;
        }
    }
}