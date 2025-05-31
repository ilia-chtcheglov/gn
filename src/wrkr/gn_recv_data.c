#include <gn_recv_data.h>

__attribute__((nonnull))
void
gn_recv_data (gn_conn_t * const conn)
{
    /*
     * Receive data from connection.
     * The received data will be stored after the data already present in the receive buffer.
     * The recv() function will receive (buffer size - 1) - (data already in the buffer).
     *
     * If buffer size is 10 bytes and length (data inside) is 4, recv() will receive 10 - 4 - 1 = 5.
     * The final buffer length will be 4 + 5 = 9 which is less than 10, with room for the NULL byte.
     */
    const ssize_t rrecv = recv (conn->sock, &conn->recv_buf[conn->recv_buf_len],
                                conn->recv_buf_sz - conn->recv_buf_len - 1, 0);
    switch (rrecv)
    {
        case -1: // Error occurred.
        {
            switch (errno)
            {
                case EINTR:
                {
                    // recv() was interrupted by a signal. Update the last_io variable.
                    conn->last_io = time (NULL);
                    break;
                }
                case EAGAIN:
                {
                    // recv() received nothing. Check if connection is idle.
                    const time_t t = time (NULL);
                    if (t == conn->last_io + 10) // Timeout after 10 seconds.
                    {
                        // Connection is idle. Close it.
                        conn->step = GN_CONN_STEP_CLOSE;
                        printf ("Connection timeout.\n"); // TODO: Remove.
                    }
                    break;
                }
                // TODO: Check for other errno values such as EBADF.
                default:
                {
                    // Other errors. Close connection.
                    conn->step = GN_CONN_STEP_CLOSE;
                    fprintf (stderr, "Failed to receive HTTP client data. %s.\n", strerror (errno));
                }
            }
            break;
        }
        case 0: // Client disconnected.
        {
            printf ("Client disconnected.\n"); // TODO: Remove.
            conn->step = GN_CONN_STEP_CLOSE;
            break;
        }
        default: // Data received.
        {
            // Update the receive buffer length.
            conn->recv_buf_len += (uint32_t)rrecv;
            // NULL terminate the receive buffer.
            conn->recv_buf[conn->recv_buf_len] = '\0';
            // Update the last_io variable.
            conn->last_io = time (NULL);
            // Go back to the previous connection step.
            conn->step = conn->prev_step;
            printf ("Received (%u) \"%s\"\n", conn->recv_buf_len, conn->recv_buf); // TODO: Remove.
        }
    }
}