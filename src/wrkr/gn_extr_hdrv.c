
#include <gn_extr_hdrv.h>

__attribute__((nonnull))
void
gn_extr_hdrv (gn_conn_t * const conn)
{
    size_t recv_buf_i = 0;
    for ( ;
         recv_buf_i < conn->recv_buf_len &&
         conn->hdrv_len < conn->hdrv_sz - 1 &&
         conn->recv_buf[recv_buf_i] != '\n';
         recv_buf_i++, conn->hdrv_len++)
    {
        conn->hdrv[conn->hdrv_len] = conn->recv_buf[recv_buf_i];
    }

    conn->hdrv[conn->hdrv_len] = '\0';
    if (conn->recv_buf[recv_buf_i] == '\n')
    {
        // Move the rest of the data to the beginning of the receive buffer.
        size_t i = 0;
        size_t j = (size_t)conn->hdrv_len + 1;
        while (j < conn->recv_buf_len)
        {
            conn->recv_buf[i] = conn->recv_buf[j];
            i++;
            j++;
        }
        conn->recv_buf_len -= (uint32_t)conn->hdrv_len + 1;
        conn->recv_buf[conn->recv_buf_len] = '\0';
        printf ("Remaining (%u) \"%s\"\n", conn->recv_buf_len, conn->recv_buf); // TODO: Remove.

        // Must end with CRLF.
        if (conn->hdrv_len > 0 && conn->hdrv[conn->hdrv_len - 1] == '\r')
        {
            conn->hdrv[conn->hdrv_len - 1] = '\0';
            conn->hdrv_len--;
        }
        printf ("Header value (%u) \"%s\".\n", conn->hdrv_len, conn->hdrv);

        if (gn_htbl_insr (&conn->req_hdrs, conn->hdrn, conn->hdrn_len, conn->hdrv, conn->hdrv_len))
        {
            fprintf (stderr, "Failed to add request header to hash table.\n");
            conn->step = GN_CONN_STEP_CLOSE;
            return;
        }

        conn->hdrn_len = 0;
        conn->hdrv_len = 0;

        conn->prev_step = GN_CONN_STEP_INVALID;
        conn->step = GN_CONN_STEP_EXTR_HDRN; // TODO: Go to next step.
    }
    else
    {
        if (conn->hdrn_len == conn->hdrn_sz - 1)
        {
            fprintf (stderr, "Request header value too long.\n");
            conn->step = GN_CONN_STEP_CLOSE;
        }
        else conn->step = GN_CONN_STEP_RECV_DATA;
    }
}