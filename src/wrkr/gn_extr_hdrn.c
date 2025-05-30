#include <gn_extr_hdrn.h>

__attribute__((nonnull))
void
gn_extr_hdrn (gn_conn_t * const conn)
{
    size_t recv_buf_i = 0;
    for ( ;
         recv_buf_i < conn->recv_buf_len &&
         conn->hdrn_len < conn->hdrn_sz - 1 &&
         conn->recv_buf[recv_buf_i] != ':' &&
         conn->recv_buf[recv_buf_i] != '\n';
         recv_buf_i++, conn->hdrn_len++)
    {
        conn->hdrn[conn->hdrn_len] = conn->recv_buf[recv_buf_i];
    }
    conn->hdrn[conn->hdrn_len] = '\0';

    switch (conn->recv_buf[recv_buf_i])
    {
        case ':':
        {
            printf ("Header name (%u) \"%s\".\n", conn->hdrn_len, conn->hdrn);
            // Move the rest of the data to the beginning of the receive buffer.
            size_t i = 0;
            size_t j = (size_t)conn->hdrn_len + 1;
            while (j < conn->recv_buf_len)
            {
                conn->recv_buf[i] = conn->recv_buf[j];
                i++;
                j++;
            }
            conn->recv_buf_len -= (uint32_t)conn->hdrn_len + 1;
            conn->recv_buf[conn->recv_buf_len] = '\0';
            printf ("Remaining (%u) \"%s\"\n", conn->recv_buf_len, conn->recv_buf); // TODO: Remove.

            conn->step = GN_CONN_STEP_EXTR_HDRV; // TODO: Go to next step.
            break;
        }
        case '\n':
        {
            printf ("End of request headers.\n");
            conn->recv_buf[0] = '\0';
            conn->step = GN_CONN_STEP_RECV_DATA;
            break;
        }
        default:
        {
            if (conn->hdrn_len == conn->hdrn_sz - 1)
            {
                fprintf (stderr, "Request header name too long.\n");
                conn->step = GN_CONN_STEP_CLOSE;
            }
            else conn->step = GN_CONN_STEP_RECV_DATA;
        }
    }
}