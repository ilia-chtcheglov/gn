#include <arpa/inet.h>
#include <errno.h>
#include <stdint.h>
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

typedef struct gn_serv_sock_list_t gn_serv_sock_list_t;

struct gn_serv_sock_list_t
{
    gn_serv_sock_t * head;
    gn_serv_sock_t * tail;
    size_t           len;
};

int
gn_serv_sock_list_push_back (gn_serv_sock_list_t * const list, gn_serv_sock_t * const sock);

int
gn_serv_sock_list_push_back (gn_serv_sock_list_t * const list, gn_serv_sock_t * const sock)
{
    switch (list->len)
    {
        case 0:
        {
            list->head = list->tail = sock->prev = sock->next = sock;
            break;
        }
        case SIZE_MAX:
        {
            return 1;
        }
        default:
        {
            list->tail->next = sock;
            sock->prev = list->tail;
            list->head->prev = sock;
            sock->next = list->head;
            list->tail = sock;
        }
    }

    list->len++;
    return 0;
}

__attribute__((warn_unused_result))
int
gn_open_serv_sock (gn_serv_sock_list_t * const list, const char * const addr, const uint16_t port);

__attribute__((warn_unused_result))
int
gn_open_serv_sock (gn_serv_sock_list_t * const list, const char * const addr, const uint16_t port)
{
    if (port < 1) return 1;

    gn_serv_sock_t * serv_sock = (gn_serv_sock_t *)malloc (sizeof (gn_serv_sock_t));
    if (serv_sock == NULL) return 1;

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

        close (rsocket);
    }
    else
    {
        fprintf (stderr, "Failed to create socket. %s.\n", strerror (errno));
    }

    free (serv_sock);
    return 1;
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

    gn_serv_sock_list_t serv_sock_list;
    memset (&serv_sock_list, 0, sizeof (gn_serv_sock_list_t));

    int rgn_open_serv_sock = gn_open_serv_sock (&serv_sock_list, "0.0.0.0", 8080);
    if (rgn_open_serv_sock != 0) fprintf (stderr, "Failed to open server socket.\n");
    rgn_open_serv_sock = gn_open_serv_sock (&serv_sock_list, "127.0.0.1", 8081);
    if (rgn_open_serv_sock != 0) fprintf (stderr, "Failed to open server socket.\n");

    // TODO: Empty @serv_sock_list and close sockets.

    return EXIT_SUCCESS;
}