#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Maximum number of command line arguments.
#define GN_MAX_CMDL_ARGS 8

typedef struct gn_serv_sock_t gn_serv_sock_t;

struct gn_serv_sock_t
{
    int              fd;
    uint32_t         pad0;
    gn_serv_sock_t * prev;
    gn_serv_sock_t * next;
};

__attribute__((warn_unused_result))
gn_serv_sock_t *
gn_open_serv_sock (const char * const addr, const uint16_t port);

__attribute__((warn_unused_result))
gn_serv_sock_t *
gn_open_serv_sock (const char * const addr, const uint16_t port)
{
    if (port < 1) return NULL;

    gn_serv_sock_t * serv_sock = (gn_serv_sock_t *)malloc (sizeof (gn_serv_sock_t));
    if (serv_sock == NULL) return NULL;

    const int rsocket = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (rsocket > -1)
    {
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
                        return serv_sock;
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

        close (rsocket);
    }
    else
    {
        fprintf (stderr, "Failed to create socket. %s.\n", strerror (errno));
    }

    free (serv_sock);
    return NULL;
}

int
main (const int argc,
      __attribute__((unused)) const char * const * const argv)
{
    // Checking @argc.
    if (argc < 0)
    {
        fprintf (stderr, "Negative number (%i) of command line arguments.\n", argc);
        return EXIT_FAILURE;
    }
    if (argc > GN_MAX_CMDL_ARGS)
    {
        fprintf (stderr, "Number of command line arguments (%i) too high (maximum %i).\n", argc, GN_MAX_CMDL_ARGS);
        return EXIT_FAILURE;
    }

    gn_serv_sock_t * serv_sock = gn_open_serv_sock ("0.0.0.0", 8080);
    if (serv_sock != NULL)
    {
        close (serv_sock->fd);
        free (serv_sock);
    }
    serv_sock = gn_open_serv_sock ("127.0.0.1", 8081);
    if (serv_sock != NULL)
    {
        close (serv_sock->fd);
        free (serv_sock);
    }

    return EXIT_SUCCESS;
}