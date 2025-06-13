#include <gn_send_file.h>

__attribute__((nonnull))
void
gn_send_file (gn_conn_t * const conn)
{
    const size_t max_read = conn->send_buf.sz - conn->send_buf.len - 1;
    const ssize_t rread = read (conn->fd, &conn->send_buf.dat[conn->send_buf.len], max_read);
    switch (rread)
    {
        case -1:
        {
            /*
             * Documentation says: On error, -1 is returned, and errno is set appropriately.
             * IN THIS CASE it is left unspecified whether the file position (if any) changes.
             */
            switch (errno)
            {
                case EIO: // Could be a low-level I/O error.
                    break;
                case EAGAIN:
                /*
                 * read() can return EAGAIN or EWOULDBLOCK.
                 * But if they have the same value we can use only one of them.
                 */
                #if EAGAIN != EWOULDBLOCK
                case EWOULDBLOCK:
                #endif
                case EINTR:
                {
                    conn->last_io = time (NULL);
                    break;
                }
                case EBADF:
                {
                    fprintf (stderr, "Failed to read file. %s.\n", strerror (errno));

                    /*
                     * If conn->fd is not invalid (> -1) but we get EBADF it means the fd value was changed/corrupted.
                     * Close the connection without closing conn->fd. We don't know what fd refers to now.
                     * If fd is invalid (< 0), I don't know why gn_send_file() was called. Close the connection anyway.
                     */
                    if (conn->fd > -1)
                    {
                        conn->fd = -1;
                        // TODO: Stop the worker process.
                    }
                    conn->step = GN_CONN_STEP_CLOSE;
                    break;
                }
                case EISDIR:
                {
                    fprintf (stderr, "Failed to read file. %s.\n", strerror (errno));

                    /*
                     * This program doesn't open directories. If we get EISDIR it means
                     * the fd value was changed/corrupted. Close the connection without
                     * closing conn->fd. We don't know what fd refers to now.
                     */
                    conn->fd = -1;
                    conn->step = GN_CONN_STEP_CLOSE;
                    break;
                }
                default:
                {
                    fprintf (stderr, "Failed to read file. %s.\n", strerror (errno));
                    conn->step = GN_CONN_STEP_CLOSE;
                }
            }
            break;
        }
        case 0:
        {
            if (max_read > 0)
            {
                conn->step = GN_CONN_STEP_CLOSE;
                break;
            }
            __attribute__((fallthrough));
        }
        default:
        {
            conn->send_buf.len += (gn_str_len_t)rread;
            conn->send_buf.dat[conn->send_buf.len] = '\0';
            gn_send_data (conn);
        }
    }
}