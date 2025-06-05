#include <gn_extr_hdrn.h>

__attribute__((nonnull))
void
gn_extr_hdrn (gn_conn_t * const conn)
{
    // Extract the header name from conn->recv_buf and store it in conn->hdrn.
    gn_str_len_t recv_buf_i = 0;
    for ( ;
         recv_buf_i < conn->recv_buf.len &&
         conn->hdrn.len < conn->hdrn.sz - 1 &&
         conn->recv_buf.dat[recv_buf_i] != ':' &&
         conn->recv_buf.dat[recv_buf_i] != '\n';
         recv_buf_i++, conn->hdrn.len++)
    {
        /*
         * From RFC9112 5. Field Syntax:
         * Each field line consists of a case-insensitive field name...
         */
        conn->hdrn.dat[conn->hdrn.len] = (char)tolower ((unsigned char)conn->recv_buf.dat[recv_buf_i]);
    }
    conn->hdrn.dat[conn->hdrn.len] = '\0';

    switch (conn->recv_buf.dat[recv_buf_i])
    {
        case ':':
        {
            printf ("Header name (%u) \"%s\".\n", conn->hdrn.len, conn->hdrn.dat);
            // Move the rest of the data to the beginning of the receive buffer.
            size_t i = 0;
            size_t j = (size_t)conn->hdrn.len + 1;
            while (j < conn->recv_buf.len)
            {
                conn->recv_buf.dat[i] = conn->recv_buf.dat[j];
                i++;
                j++;
            }
            conn->recv_buf.len -= (uint32_t)conn->hdrn.len + 1;
            conn->recv_buf.dat[conn->recv_buf.len] = '\0';
            printf ("Remaining (%u) \"%s\"\n", conn->recv_buf.len, conn->recv_buf.dat); // TODO: Remove.

            conn->step = GN_CONN_STEP_EXTR_HDRV; // TODO: Go to next step.
            break;
        }
        case '\n':
        {
            printf ("End of request headers.\n");
            conn->recv_buf.dat[0] = '\0';
            gn_htbl_dump (&conn->req_hdrs); // TODO: Remove.

            /*
             * From RFC9112 3.2 Request Target:
             * A server MUST response with a 400 (Bad Request) status code to
             * any HTTP/1.1 request message that lacks a Host header field...
             */
            if (gn_htbl_srch (&conn->req_hdrs, "host", 4) == NULL)
            {
                fprintf (stderr, "Missing Host header.\n");
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
                fprintf (stderr, "Request header name too long.\n");
                conn->step = GN_CONN_STEP_CLOSE;
            }
            else conn->step = GN_CONN_STEP_RECV_DATA;
        }
    }
}