#include <gn_open_serv_sock.h>

__attribute__((warn_unused_result))
int
gn_open_serv_sock (gn_serv_sock_list_t * const list, const char * const addr, const uint16_t port)
{
    if (port < 1) return 1;

    gn_serv_sock_t * serv_sock = (gn_serv_sock_t *)malloc (sizeof (gn_serv_sock_t));
    if (serv_sock == NULL) return 1;

    serv_sock->addr = (char *)malloc (strlen (addr) + 1);
    if (serv_sock->addr == NULL)
    {
        free (serv_sock);
        return 1;
    }

    int rsocket = socket (AF_INET, SOCK_STREAM | SOCK_CLOEXEC | SOCK_NONBLOCK, IPPROTO_TCP);
    if (rsocket > -1)
    {
        printf ("Opened FD %i.\n", rsocket);
        struct sockaddr_in sin;
        memset (&sin, 0, sizeof (sin));

        sin.sin_addr.s_addr = inet_addr (addr);
        sin.sin_family = AF_INET;
        sin.sin_port = htons (port);

        const int rbind = bind (rsocket, (struct sockaddr *)&sin, sizeof (sin));
        switch (rbind)
        {
            case 0:
            {
                const int rlisten = listen (rsocket, SOMAXCONN);
                switch (rlisten)
                {
                    case 0:
                    {
                        serv_sock->fd = rsocket;
                        strcpy (serv_sock->addr, addr);
                        serv_sock->port = port;

                        gn_serv_sock_list_push_back (list, serv_sock);
                        return 0;
                    }
                    case -1:
                    {
                        fprintf (stderr, "Failed to listen on socket. %s.\n", strerror (errno));
                        break;
                    }
                    default:
                    {
                        fprintf (stderr, "listen() returned unexpected value %i.\n", rlisten);
                    }
                }
                break;
            }
            case -1:
            {
                fprintf (stderr, "Failed to bind socket. %s.\n", strerror (errno));
                break;
            }
            default:
            {
                fprintf (stderr, "bind() returned unexpected value %i.\n", rbind);
            }
        }

        gn_close (&rsocket);
    }
    else
    {
        fprintf (stderr, "Failed to create socket. %s.\n", strerror (errno));
    }

    free (serv_sock->addr);
    free (serv_sock);
    return 1;
}