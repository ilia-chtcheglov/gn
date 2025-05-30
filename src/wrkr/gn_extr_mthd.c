#include <gn_extr_mthd.h>

__attribute__((nonnull))
void
gn_extr_mthd (gn_conn_t * const conn)
{
    size_t recv_buf_i = 0;
    for ( ;
         recv_buf_i < conn->recv_buf_len &&
         conn->mthd_len < conn->mthd_sz - 1 &&
         conn->recv_buf[recv_buf_i] != ' ';
         recv_buf_i++, conn->mthd_len++)
    {
        conn->mthd[conn->mthd_len] = conn->recv_buf[recv_buf_i];
    }

    conn->mthd[conn->mthd_len] = '\0';
    if (conn->recv_buf[recv_buf_i] == ' ')
    {
        printf ("Request method (%u) \"%s\".\n", conn->mthd_len, conn->mthd); // TODO: Remove.
        // Move the rest of the data to the beginning of the receive buffer.
        size_t i = 0;
        size_t j = conn->mthd_len + 1;
        while (j < conn->recv_buf_len)
        {
            conn->recv_buf[i] = conn->recv_buf[j];
            i++;
            j++;
        }
        conn->recv_buf_len -= conn->mthd_len + 1;
        conn->recv_buf[conn->recv_buf_len] = '\0';
        printf ("Remaining (%u) \"%s\"\n", conn->recv_buf_len, conn->recv_buf); // TODO: Remove.

        conn->prev_step = GN_CONN_STEP_INVALID;
        conn->step = GN_CONN_STEP_EXTR_URI; // TODO: Go to next step.
    }
    else
    {
        if (conn->mthd_len == conn->mthd_sz - 1)
        {
            fprintf (stderr, "Request method too long.\n"); // TODO: Remove.
            conn->step = GN_CONN_STEP_CLOSE;
        }
        else conn->step = GN_CONN_STEP_RECV_DATA;
    }
}