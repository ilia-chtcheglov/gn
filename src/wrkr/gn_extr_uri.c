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
        printf ("Request URI (%u) \"%s\".\n", conn->uri.len, conn->uri.dat);
        // Move the rest of the data to the beginning of the receive buffer.
        size_t i = 0;
        size_t j = conn->uri.len + 1;
        while (j < conn->recv_buf.len)
        {
            conn->recv_buf.dat[i] = conn->recv_buf.dat[j];
            i++;
            j++;
        }
        conn->recv_buf.len -= conn->uri.len + 1;
        conn->recv_buf.dat[conn->recv_buf.len] = '\0';
        // printf ("Remaining (%u) \"%s\"\n", conn->recv_buf.len, conn->recv_buf.dat); // TODO: Remove.

        conn->prev_step = GN_CONN_STEP_INVALID;
        conn->step = GN_CONN_STEP_EXTR_PROT; // TODO: Go to next step.
    }
    else
    {
        if (conn->uri.len == conn->uri.sz - 1)
        {
            fprintf (stderr, "Request URI too long.\n");
            conn->status = 414;
            conn->step = GN_CONN_STEP_WRIT_HDRS;
        }
        else conn->step = GN_CONN_STEP_RECV_DATA;
    }
}