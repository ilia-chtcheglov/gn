#include <gn_process_conn.h>

#include <errno.h>
#include <poll.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

int
gn_close (int * const fd);

__attribute__((nonnull))
void
gn_open_file (gn_conn_t * const conn);

__attribute__((nonnull))
__attribute__((warn_unused_result))
gn_str_err_e
gn_str_lshift (gn_str_t * const str,
               const gn_str_len_t num);

__attribute__((nonnull))
void
gn_writ_hdrs (gn_conn_t * const conn);

__attribute__((nonnull))
void
gn_writ_hdrs (gn_conn_t * const conn)
{
    switch (conn->status)
    {
        case 200:
        {
            strcpy (conn->send_buf.dat, "HTTP/1.1 200 OK\r\n\r\n");
            break;
        }
        case 400:
        {
            strcpy (conn->send_buf.dat, "HTTP/1.1 400 Bad Request\r\n\r\n");
            break;
        }
        case 403:
        {
            strcpy (conn->send_buf.dat, "HTTP/1.1 403 Forbidden\r\n\r\n");
            break;
        }
        case 404:
        {
            strcpy (conn->send_buf.dat, "HTTP/1.1 404 Not Found\r\n\r\n");
            break;
        }
        case 414:
        {
            strcpy (conn->send_buf.dat, "HTTP/1.1 414 URI Too Long\r\n\r\n");
            break;
        }
        case 500:
        {
            strcpy (conn->send_buf.dat, "HTTP/1.1 500 Internal Server Error\r\n\r\n");
            break;
        }
        case 501:
        {
            strcpy (conn->send_buf.dat, "HTTP/1.1 501 Not Implemented\r\n\r\n");
            break;
        }
        case 502:
        {
            strcpy (conn->send_buf.dat, "HTTP/1.1 502 Bad Gateway\r\n\r\n");
            break;
        }
        default:
        {
            fprintf (stderr, "Unexpected response code %i.\n", conn->status);
            conn->status = 500;
            strcpy (conn->send_buf.dat, "HTTP/1.1 500 Internal Server Error\r\n\r\n");
        }
    }

    conn->send_buf.len = (gn_str_len_t)strlen (conn->send_buf.dat);

    conn->prev_step = GN_CONN_STEP_WRIT_HDRS;
    conn->step = GN_CONN_STEP_SEND_HDRS;
}

__attribute__((nonnull))
void
gn_send_hdrs (gn_conn_t * const conn);

__attribute__((nonnull))
void
gn_send_hdrs (gn_conn_t * const conn)
{
    printf ("Sending headers (%u) \"%s\".\n", conn->send_buf.len, conn->send_buf.dat);

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
            printf ("Remaining (%u) \"%s\"\n", conn->send_buf.len, conn->send_buf.dat); // TODO: Remove.

            if (conn->send_buf.len == 0)
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

__attribute__((nonnull))
void
gn_send_file (gn_conn_t * const conn);

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
            conn->send_buf.len += (uint32_t)rread;
            conn->send_buf.dat[conn->send_buf.len] = '\0';
            gn_send_hdrs (conn);
        }
    }
}

__attribute__((nonnull))
void
gn_fcgi_conn (gn_conn_t * const conn);

__attribute__((nonnull))
void
gn_fcgi_conn (gn_conn_t * const conn)
{
    if (conn->fd < 0)
    {
        conn->fd = socket (AF_INET, SOCK_STREAM | SOCK_CLOEXEC | SOCK_NONBLOCK, IPPROTO_TCP);
        if (conn->fd < 0)
        {
            conn->status = 500;
            conn->step = GN_CONN_STEP_WRIT_HDRS;
            return;
        }

        struct sockaddr_in sin = { 0 };
        sin.sin_addr.s_addr = inet_addr ("127.0.0.1");
        sin.sin_family = AF_INET;
        sin.sin_port = htons (9000);

        if (connect (conn->fd, &sin, sizeof (sin)) == 0)
        {
            printf ("Connected to PHP-FPM.\n");
            conn->step = GN_CONN_STEP_CLOSE;
            return;
        }
        else
        {
            switch (errno)
            {
                case EINPROGRESS:
                    return;
                default:
                {
                    fprintf (stderr, "Failed to connect to PHP-FPM. %s.\n", strerror (errno));
                    gn_close (&conn->fd);
                    conn->status = 502;
                    conn->step = GN_CONN_STEP_WRIT_HDRS;
                }
            }
        }
    }

    struct pollfd pfd = {
        .fd = conn->fd,
        .events = POLLOUT,
        .revents = 0
    };

    const int rpoll = poll (&pfd, 1, 0);
    switch (rpoll)
    {
        case 0:
        {
            break;
        }
        case -1:
        {
            fprintf (stderr, "poll() failed. %s.\n", strerror (errno));
            gn_close (&conn->fd);
            conn->status = 502;
            conn->step = GN_CONN_STEP_WRIT_HDRS;
            break;
        }
        case 1:
        default:
        {
            printf ("Connected to PHP-FPM.\n");
            conn->step = GN_CONN_STEP_CLOSE;
        }
    }
}

__attribute__((nonnull))
__attribute__((warn_unused_result))
bool
gn_process_conn (gn_conn_t * const conn)
{
    switch (conn->step)
    {
        case GN_CONN_STEP_EXTR_MTHD:
        {
            gn_extr_mthd (conn);
            if (conn->step != GN_CONN_STEP_EXTR_URI) break;
            __attribute__((fallthrough));
        }
        case GN_CONN_STEP_EXTR_URI:
        {
            gn_extr_uri (conn);
            if (conn->step != GN_CONN_STEP_EXTR_PROT) break;
            __attribute__((fallthrough));
        }
        case GN_CONN_STEP_EXTR_PROT:
        {
            gn_extr_prot (conn);
            if (conn->step != GN_CONN_STEP_EXTR_HDRN) break;
            __attribute__((fallthrough));
        }
        case GN_CONN_STEP_EXTR_HDRN:
        {
            gn_extr_hdrn (conn);
            if (conn->step != GN_CONN_STEP_EXTR_HDRV) break;
            __attribute__((fallthrough));
        }
        case GN_CONN_STEP_EXTR_HDRV:
        {
            gn_extr_hdrv (conn);
            break;
        }
        case GN_CONN_STEP_OPEN_FILE:
        case GN_CONN_STEP_OPEN_LIST:
        {
            gn_open_file (conn);
            break;
        }
        case GN_CONN_STEP_WRIT_HDRS:
        {
            gn_writ_hdrs (conn);
            break;
        }
        case GN_CONN_STEP_SEND_HDRS:
        {
            gn_send_hdrs (conn);
            break;
        }
        case GN_CONN_STEP_SEND_FILE:
        {
            gn_send_file (conn);
            break;
        }
        case GN_CONN_STEP_RECV_DATA:
        {
            gn_recv_data (conn);
            break;
        }
        case GN_CONN_STEP_FCGI_CONN:
        {
            gn_fcgi_conn (conn);
            break;
        }
        case GN_CONN_STEP_CLOSE:
        {
            printf ("Closing connection %p\n", (void *)conn);
            gn_close_conn (conn);
            return true;
        }
        case GN_CONN_STEP_INVALID:
        default:
        {
            fprintf (stderr, "Invalid connection step %u.\n", conn->step);
            conn->step = GN_CONN_STEP_CLOSE;
        }
    }

    return false;
}