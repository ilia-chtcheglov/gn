#include <gn_acpt_conn.h>

typedef struct gn_conn_t gn_conn_t;

struct gn_conn_t
{
    int         fd;
    char *      saddr;
    uint16_t    sport;
    gn_conn_t * prev;
    gn_conn_t * next;
};

__attribute__((warn_unused_result))
int
gn_acpt_conn (const gn_serv_sock_t * const serv_sock)
{
    struct sockaddr_in sin;
    memset (&sin, 0, sizeof (sin));
    socklen_t sin_sz = sizeof (sin);

    int raccept4 = accept4 (serv_sock->fd, &sin, &sin_sz, SOCK_CLOEXEC | SOCK_NONBLOCK);
    if (raccept4 > -1)
    {
        char saddr[INET_ADDRSTRLEN];
        memset (saddr, 0, sizeof (saddr));

        const char * const rinet_ntop = inet_ntop (AF_INET, &sin.sin_addr, saddr, sizeof (saddr));
        if (rinet_ntop == NULL)
        {
            fprintf (stderr, "inet_ntop() failed. %s.\n", strerror (errno));
        }

        const uint16_t sport = ntohs (sin.sin_port);
        printf ("Accepted connection from [%s]:%i to [%s]:%i.\n", saddr, sport, serv_sock->addr, serv_sock->port);

        gn_conn_t * conn = (gn_conn_t *)malloc (sizeof (gn_conn_t));
        if (conn != NULL)
        {

        }
        else
        {
            fprintf (stderr, "Failed to allocate structure for connection.\n");
        }

        close (raccept4);
    }
    else
    {
        switch (errno)
        {
            case EAGAIN:
            case ECONNABORTED:
            case EINTR:
                break;
            case EMFILE:
            case ENFILE:
            case ENOBUFS:
            case ENOMEM:
            case EPERM:
            {
                fprintf (stderr, "Failed to accept connection. %s.\n", strerror (errno));
                break;
            }
            case EBADF:
            case EFAULT:
            case EINVAL:
            case ENOTSOCK:
            case EOPNOTSUPP:
            case EPROTO:
            {
                fprintf (stderr, "Failed to accept connection. %s.\n", strerror (errno));
                return EXIT_FAILURE;
            }
            default:
            {
                fprintf (stderr, "Failed to accept connection. %s.\n", strerror (errno));
                return EXIT_FAILURE;
            }
        }
    }

    return EXIT_SUCCESS;
}