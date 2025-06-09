#include <gn_extr_hdrv.h>

__attribute__((nonnull))
void
gn_extr_hdrv (gn_conn_t * const conn)
{
    // Extract the header value from conn->recv_buf and store it in conn->hdrv.
    gn_str_len_t recv_buf_i = 0;
    while (recv_buf_i < conn->recv_buf.len && conn->hdrv.len < conn->hdrv.sz - 1)
    {
        if (conn->recv_buf.dat[recv_buf_i] == '\n') break;

        conn->hdrv.dat[conn->hdrv.len++] = conn->recv_buf.dat[recv_buf_i++];
    }

    conn->hdrv.dat[conn->hdrv.len] = '\0';
    if (conn->recv_buf.dat[recv_buf_i] == '\n')
    {
        // Move the rest of the data to the beginning of the receive buffer.
        gn_str_len_t i = 0;
        gn_str_len_t j = conn->hdrv.len + 1;
        while (j < conn->recv_buf.len)
        {
            conn->recv_buf.dat[i] = conn->recv_buf.dat[j];
            i++;
            j++;
        }
        conn->recv_buf.len -= conn->hdrv.len + 1;
        conn->recv_buf.dat[conn->recv_buf.len] = '\0';
        // printf ("Remaining (%u) \"%s\"\n", conn->recv_buf.len, conn->recv_buf.dat); // TODO: Remove.

        // Must end with CRLF.
        if (conn->hdrv.len > 0 && conn->hdrv.dat[conn->hdrv.len - 1] == '\r')
        {
            conn->hdrv.dat[conn->hdrv.len - 1] = '\0';
            conn->hdrv.len--;
        }

        // Remove white spaces before the header value.
        bool loop = true;
        for (i = 0; i < conn->hdrv.len && loop; )
        {
            switch (conn->hdrv.dat[i])
            {
                case ' ':
                case '\t':
                {
                    i++;
                    break;
                }
                default:
                    loop = false;
            }
        }

        gn_str_len_t s = i;
        // Move the data to the beginning of the header value buffer.
        i = 0;
        j = s;
        while (j < conn->hdrv.len)
        {
            conn->hdrv.dat[i] = conn->hdrv.dat[j];
            i++;
            j++;
        }
        conn->hdrv.len -= s;
        conn->hdrv.dat[conn->hdrv.len] = '\0';

        printf ("Header value (%u) \"%s\".\n", conn->hdrv.len, conn->hdrv.dat);

        if (strcmp (conn->hdrn.dat, "host") == 0)
        {
            /*
             * From RFC9112 3.2 Request Target:
             * A server MUST response with a 400 (Bad Request) status code to
             * any HTTP/1.1 request message that ... contains more than one
             * Host header field line...
             */
            if (gn_htbl_srch (&conn->req_hdrs, conn->hdrn.dat, conn->hdrn.len) != NULL)
            {
                fprintf (stderr, "Duplicate Host header detected.\n");
                conn->status = 400;
                conn->step = GN_CONN_STEP_WRIT_HDRS;
                return;
            }
        }
        else if (strcmp (conn->hdrn.dat, "content-length") == 0)
        {
            /*
             * From RFC9112 6.2 Content-Length:
             * A sender MUST NOT send a Content-Length header field in
             * any message that contains a Transfer-Encoding header field.
             */
            if (gn_htbl_srch (&conn->req_hdrs, "transfer-encoding", strlen ("transfer-encoding")) != NULL)
            {
                fprintf (stderr, "Content-Length present with Transfer-Encoding.\n");
                conn->status = 400;
                conn->step = GN_CONN_STEP_WRIT_HDRS;
                return;
            }
        }
        else if (strcmp (conn->hdrn.dat, "transfer-encoding") == 0)
        {
            /*
             * From RFC9112 6.2 Content-Length:
             * A sender MUST NOT send a Content-Length header field in
             * any message that contains a Transfer-Encoding header field.
             */
            if (gn_htbl_srch (&conn->req_hdrs, "content-length", strlen ("content-length")) != NULL)
            {
                fprintf (stderr, "Transfer-Encoding present with Content-Length.\n");
                conn->status = 400;
                conn->step = GN_CONN_STEP_WRIT_HDRS;
                return;
            }
        }

        if (gn_htbl_insr (&conn->req_hdrs, conn->hdrn.dat, conn->hdrn.len, conn->hdrv.dat, conn->hdrv.len))
        {
            fprintf (stderr, "Failed to add request header to hash table.\n");
            conn->status = 500;
            conn->step = GN_CONN_STEP_WRIT_HDRS;
            return;
        }

        conn->hdrn.len = 0;
        conn->hdrv.len = 0;

        conn->prev_step = GN_CONN_STEP_INVALID;
        conn->step = GN_CONN_STEP_EXTR_HDRN;
    }
    else
    {
        if (conn->hdrn.len == conn->hdrn.sz - 1)
        {
            fprintf (stderr, "Request header value too long.\n");
            conn->step = GN_CONN_STEP_CLOSE;
        }
        else conn->step = GN_CONN_STEP_RECV_DATA;
    }
}