#include <gn_process_conn.h>

#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>

int
gn_close (int * const fd);

__attribute__((nonnull))
void
gn_open_file (gn_conn_t * const conn);

__attribute__((nonnull))
void
gn_open_file (gn_conn_t * const conn)
{
    conn->prev_step = GN_CONN_STEP_OPEN_FILE;

    const char * const document_root = "/srv/www/testa.local";
    const uint32_t document_root_len = strlen (document_root);

    char * abs_path = (char *)malloc (document_root_len + conn->uri_len + 1);
    if (abs_path != NULL)
    {
        strcpy (abs_path, document_root);
        strcat (abs_path, conn->uri);
        printf ("Absolute path: \"%s\".\n", abs_path);

        conn->fd = open (abs_path, O_RDONLY | O_NONBLOCK);
        if (conn->fd > -1)
        {
            printf ("Opened \"%s\".\n", abs_path);
            struct stat st = { 0 };
            const int rfstat = fstat (conn->fd, &st);
            if (rfstat == 0)
            {
                strcpy (conn->send_buf, "HTTP/1.1 200 OK\r\n");
                conn->status = 200;
                char tmp[48];
                sprintf (tmp, "Content-Length: %li\r\n\r\n", st.st_size);
                strcat (conn->send_buf, tmp);
            }
            else
            {
                strcpy (conn->send_buf, "HTTP/1.1 500 Internal Server Error\r\n\r\n");
                conn->status = 500;
            }
        }
        else
        {
            fprintf (stderr, "Failed to open \"%s\". %s.\n", abs_path, strerror (errno));
            switch (errno)
            {
                case ENOENT:
                {
                    strcpy (conn->send_buf, "HTTP/1.1 404 Not Found\r\n\r\n");
                    conn->status = 404;
                    break;
                }
                default:
                {
                    strcpy (conn->send_buf, "HTTP/1.1 500 Internal Server Error\r\n\r\n");
                    conn->status = 500;
                }
            }
        }

        free (abs_path);
        abs_path = NULL;
    }
    else
    {
        fprintf (stderr, "Failed to allocate buffer for absolute file path.\n");
        strcpy (conn->send_buf, "HTTP/1.1 500 Internal Server Error\r\n\r\n");
        conn->status = 500;
    }

    conn->send_buf_len = (uint32_t)strlen (conn->send_buf);
    conn->step = GN_CONN_STEP_SEND_HDRS;
}

__attribute__((nonnull))
void
gn_send_hdrs (gn_conn_t * const conn);

__attribute__((nonnull))
void
gn_send_hdrs (gn_conn_t * const conn)
{
    printf ("Sending headers (%u) \"%s\".\n", conn->send_buf_len, conn->send_buf);
    const ssize_t rsend = send (conn->sock, conn->send_buf, conn->send_buf_len, 0);
    switch (rsend)
    {
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
                    break;
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
            // Move the rest of the data to the beginning of the send buffer.
            uint32_t i = 0;
            uint32_t j = (uint32_t)rsend;
            while (j < conn->send_buf_len)
            {
                conn->send_buf[i] = conn->send_buf[j];
                i++;
                j++;
            }
            conn->send_buf_len -= (uint32_t)rsend;
            conn->send_buf[conn->send_buf_len] = '\0';
            printf ("Remaining (%u) \"%s\"\n", conn->send_buf_len, conn->send_buf); // TODO: Remove.

            if (conn->send_buf_len == 0)
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
__attribute__((warn_unused_result))
bool
gn_process_conn (gn_conn_t * const conn)
{
    switch (conn->step)
    {
        case GN_CONN_STEP_EXTR_MTHD:
        {
            gn_extr_mthd (conn);
            break;
        }
        case GN_CONN_STEP_EXTR_URI:
        {
            gn_extr_uri (conn);
            break;
        }
        case GN_CONN_STEP_EXTR_PROT:
        {
            gn_extr_prot (conn);
            break;
        }
        case GN_CONN_STEP_EXTR_HDRN:
        {
            gn_extr_hdrn (conn);
            break;
        }
        case GN_CONN_STEP_EXTR_HDRV:
        {
            gn_extr_hdrv (conn);
            break;
        }
        case GN_CONN_STEP_OPEN_FILE:
        {
            gn_open_file (conn);
            break;
        }
        case GN_CONN_STEP_SEND_HDRS:
        {
            gn_send_hdrs (conn);
            break;
        }
        case GN_CONN_STEP_SEND_FILE:
        {
            break;
        }
        case GN_CONN_STEP_RECV_DATA:
        {
            gn_recv_data (conn);
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