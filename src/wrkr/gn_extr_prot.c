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
        size_t i = 0;
        size_t j = (size_t)conn->prot.len + 1;
        while (j < conn->recv_buf.len)
        {
            conn->recv_buf.dat[i] = conn->recv_buf.dat[j];
            i++;
            j++;
        }
        conn->recv_buf.len -= (uint32_t)conn->prot.len + 1;
        conn->recv_buf.dat[conn->recv_buf.len] = '\0';
        // printf ("Remaining (%u) \"%s\"\n", conn->recv_buf.len, conn->recv_buf.dat); // TODO: Remove.

        // Must end with CRLF.
        if (conn->prot.len > 0 && conn->prot.dat[conn->prot.len - 1] == '\r')
        {
            conn->prot.dat[conn->prot.len - 1] = '\0';
            conn->prot.len--;
        }
        printf ("Request protocol (%u) \"%s\".\n", conn->prot.len, conn->prot.dat);

        conn->prev_step = GN_CONN_STEP_INVALID;
        conn->step = GN_CONN_STEP_EXTR_HDRN; // TODO: Go to next step.
    }
    else
    {
        if (conn->prot.len == conn->prot.sz - 1)
        {
            fprintf (stderr, "Request protocol too long.\n");
            conn->step = GN_CONN_STEP_CLOSE;
        }
        else conn->step = GN_CONN_STEP_RECV_DATA;
    }
}