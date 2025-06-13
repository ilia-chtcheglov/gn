#include <gn_fcgi_conn.h>

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