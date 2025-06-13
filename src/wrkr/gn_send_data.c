#include <gn_send_data.h>

__attribute__((nonnull))
void
gn_send_data (gn_conn_t * const conn)
{
    printf ("Sending (%u) \"%s\".\n", conn->send_buf.len, conn->send_buf.dat); // TODO: Remove.
    const ssize_t rsend = send (conn->sock, conn->send_buf.dat, conn->send_buf.len, 0);
    switch (rsend)
    {
        // TODO: Check if the client doesn't want to read what we're sending.
        case 0:
            break;
        case -1:
        {
            switch (errno)
            {
                case EAGAIN:
                case EINTR:
                case ENOBUFS:
                case ENOMEM:
                {
                    conn->last_io = time (NULL);
                    break;
                }
                case EBADF:
                case EFAULT:
                case EINVAL:
                case ENOTCONN:
                case ENOTSOCK:
                case EPIPE:
                default:
                {
                    gn_close (&conn->sock);
                    conn->step = GN_CONN_STEP_CLOSE;
                    break;
                }
            }
            break;
        }
        default:
        {
            conn->last_io = time (NULL);
            // Move the rest of the data to the beginning of the send buffer.
            (void)! gn_str_lshift (&conn->send_buf, (gn_str_len_t)rsend);

            if (conn->step == GN_CONN_STEP_SEND_HDRS && conn->send_buf.len == 0)
            {
                printf ("Response headers sent.\n");
                switch (conn->status)
                {
                    case 200:
                    {
                        printf ("Sending file.\n");
                        conn->step = GN_CONN_STEP_SEND_FILE;
                        break;
                    }
                    default:
                    {
                        printf ("Closing connection.\n");
                        conn->step = GN_CONN_STEP_CLOSE;
                    }
                }
            }
        }
    }
}