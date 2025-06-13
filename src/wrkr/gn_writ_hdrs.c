#include <gn_writ_hdrs.h>

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