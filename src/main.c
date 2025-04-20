#include <arpa/inet.h>
#include <errno.h>
#include <stdbool.h>
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

gn_serv_sock_t *
gn_serv_sock_list_pop (gn_serv_sock_list_t * const list);

gn_serv_sock_t *
gn_serv_sock_list_pop (gn_serv_sock_list_t * const list)
{
    switch (list->len)
    {
        case 0:
        {
            return NULL;
        }
        case 1:
        {
            gn_serv_sock_t * const ret = list->head;
            list->head = list->tail = NULL;
            list->len--;
            return ret;
        }
        default:
        {
            gn_serv_sock_t * const ret = list->head;
            list->head = list->head->next;

            list->head->prev = list->tail;
            list->tail->next = list->head;

            list->len--;
            return ret;
        }
    }
}

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

void
gn_start_wrkr (const char * const path, gn_serv_sock_list_t * const list);

void
gn_start_wrkr (const char * const path, gn_serv_sock_list_t * const list)
{
    printf ("Starting worker process.\n");
    pid_t rfork = fork ();
    switch (rfork)
    {
        case 0: // Child.
        {
            char ** argv = (char **)malloc (3 * sizeof (char *));
            if (argv != NULL)
            {
                argv[0] = (char *)malloc (strlen (path) + 1);
                if (argv[0] != NULL) strcpy (argv[0], path);
                argv[1] = (char *)malloc (strlen ("--worker") + 1);
                if (argv[1] != NULL) strcpy (argv[1], "--worker");
                argv[2] = NULL;
                execv (path, argv);
                fprintf (stderr, "Failed to start worker process. %s.\n", strerror (errno));
            }

            exit (1);
        }
        case -1:
        {
            fprintf (stderr, "Failed to fork master process. %s.\n", strerror (errno));
            break;
        }
        default: // Parent.
        {
            (void)list;
        }
    }
}

void
gn_mstr_main (void);

void
gn_mstr_main (void)
{
    gn_serv_sock_list_t serv_sock_list;
    memset (&serv_sock_list, 0, sizeof (gn_serv_sock_list_t));

    int rgn_open_serv_sock = gn_open_serv_sock (&serv_sock_list, "0.0.0.0", 8080);
    if (rgn_open_serv_sock != 0) fprintf (stderr, "Failed to open server socket.\n");
    rgn_open_serv_sock = gn_open_serv_sock (&serv_sock_list, "127.0.0.1", 8081);
    if (rgn_open_serv_sock != 0) fprintf (stderr, "Failed to open server socket.\n");

    char self_path[1024];
    memset (self_path, 0, sizeof (self_path));

    const ssize_t rreadlink = readlink ("/proc/self/exe", self_path, sizeof (self_path) - 1);
    switch (rreadlink)
    {
        case -1:
        {
            fprintf (stderr, "Failed to read path from \"/proc/self/exe\". %s.\n", strerror (errno));
            break;
        }
        case 0:
        {
            break;
        }
        default:
        {
            printf ("Starting \"%s\".\n", self_path);
            for (uint8_t i = 0; i < 2; i++)
            {
                gn_start_wrkr (self_path, &serv_sock_list);
            }
        }
    }

    while (serv_sock_list.len > 0)
    {
        gn_serv_sock_t * serv_sock = gn_serv_sock_list_pop (&serv_sock_list);
        printf ("Closing FD %i.\n", serv_sock->fd);
        close (serv_sock->fd);
        free (serv_sock);
    }
}

void
gn_wrkr_main (void);

void
gn_wrkr_main (void)
{
    printf ("[%i] Worker started.\n", getpid ());
}

int
main (const int argc,
      const char * const * const argv)
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

    bool worker = false;

    // Parse command line arguments.
    for (int argi = 1; argi < argc; argi++)
    {
        if (strcmp (argv[argi], "--worker") == 0)
        {
            worker = true;
        }
        else
        {
            fprintf (stderr, "Unsupported command line argument \"%s\".\n", argv[argi]);
            return 1;
        }
    }

    if (!worker) gn_mstr_main ();
    else gn_wrkr_main ();

    return EXIT_SUCCESS;
}