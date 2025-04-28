#include <arpa/inet.h>
#include <errno.h>
#include <poll.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/un.h>
#include <time.h>
#include <unistd.h>

#include <gn_serv_sock_list_t.h>

__attribute__((warn_unused_result))
int
gn_open_serv_sock (gn_serv_sock_list_t * const list, const char * const addr, const uint16_t port);

void
gn_send_serv_sock (const int ipc_sock, const gn_serv_sock_t * const serv_sock);

void
gn_start_wrkr (const char * const path, int ipc_sock,
               const char * const ipc_addr_str, gn_serv_sock_list_t * const list);

void
gn_start_wrkr (const char * const path, int ipc_sock,
               const char * const ipc_addr_str, gn_serv_sock_list_t * const list)
{
    printf ("Starting worker process.\n");
    // Fork the master process to start a worker process.
    pid_t rfork = fork ();
    switch (rfork)
    {
        case 0: // Child.
        {
            // Start a worker process.
            char ** argv = (char **)malloc (4 * sizeof (char *));
            if (argv != NULL)
            {
                argv[0] = (char *)malloc (strlen (path) + 1);
                if (argv[0] != NULL) strcpy (argv[0], path);
                argv[1] = (char *)malloc (strlen ("--ipc") + 1);
                if (argv[1] != NULL) strcpy (argv[1], "--ipc");
                argv[2] = (char *)malloc (strlen (ipc_addr_str) + 1);
                if (argv[2] != NULL) strcpy (argv[2], ipc_addr_str);
                argv[3] = NULL;

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
            (void)list; // TODO: Remove.
            // Wait for a connection from the worker process we just started.
            struct pollfd pfd = {
                .fd = ipc_sock,
                .events = POLLIN,
                .revents = 0
            };

            const int rpoll = poll (&pfd, 1, 3000);
            switch (rpoll)
            {
                case 1:
                {
                    // Accept the connection from the worker process.
                    const int raccept4 = accept4 (ipc_sock, NULL, NULL, SOCK_CLOEXEC | SOCK_NONBLOCK);
                    if (raccept4 > -1)
                    {
                        // Send configuration, server sockets, etc. to the worker process.
                        gn_serv_sock_t * serv_sock = list->head;
                        for (size_t i = 0; i < list->len; serv_sock = serv_sock->next, i++)
                        {
                            gn_send_serv_sock (raccept4, serv_sock);
                        }

                        close (raccept4); // TODO: Don't close IPC socket if no errors occured.
                    }
                    else if (raccept4 == -1)
                    {
                        fprintf (stderr, "Failed to accept IPC connection. %s.\n", strerror (errno));
                    }
                    else
                    {
                        fprintf (stderr, "accept4() returned unexpected value %i.\n", raccept4);
                    }

                    break;
                }
                case 0:
                {
                    fprintf (stderr, "Timeout while waiting to accept IPC connection. %s.\n", strerror (errno));
                    break;
                }
                case -1:
                {
                    fprintf (stderr, "Error while waiting to accept IPC connection. %s.\n", strerror (errno));
                    break;
                }
                default:
                {
                    fprintf (stderr, "poll() returned unexpected value %i.\n", rpoll);
                }
            }
        }
    }
}

void
gn_mstr_main (int ipc_sock, gn_serv_sock_list_t * const serv_sock_list);

void
gn_mstr_main (int ipc_sock, gn_serv_sock_list_t * const serv_sock_list)
{
    // Get current time to generate a path for the IPC socket.
    struct timespec ts;
    memset (&ts, 0, sizeof (ts));
    const int rclock_gettime = clock_gettime (CLOCK_REALTIME, &ts); // 0 OR -1 ERRNO
    switch (rclock_gettime)
    {
        case 0:
        {
            break;
        }
        case -1:
        {
            fprintf (stderr, "Failed to get current time. %s.\n", strerror (errno));
            break;
        }
        default:
        {

        }
    }

    struct sockaddr_un sun;
    memset (&sun, 0, sizeof (sun));

    sun.sun_family = AF_UNIX;
    // sun.sun_path is sun_path[108].

    int rsnprintf = snprintf (sun.sun_path, sizeof (sun.sun_path), "Z%X%lX%lX",
                              (unsigned int)getpid (), (long unsigned int)ts.tv_sec, (long unsigned int)ts.tv_nsec);
    printf ("rsnprintf: %i, sun_path: \"%s\".\n", rsnprintf, sun.sun_path);

    const int rbind = bind (ipc_sock, (struct sockaddr *)&sun, sizeof (sun));
    switch (rbind)
    {
        case 0:
        {
            const int rlisten = listen (ipc_sock, 8);
            switch (rlisten)
            {
                case 0:
                {
                    break;
                }
                case -1:
                {
                    fprintf (stderr, "Failed to listen on IPC socket. %s.\n", strerror (errno));
                    return;
                }
                default:
                {
                    fprintf (stderr, "listen() returned unexpected value %i.\n", rlisten);
                    return;
                }
            }
            break;
        }
        case -1:
        {
            fprintf (stderr, "Failed to bind socket. %s.\n", strerror (errno));
            return;
        }
        default:
        {
            fprintf (stderr, "bind() returned unexpected value %i.\n", rbind);
            return;
        }
    }

    // Open server sockets.
    int rgn_open_serv_sock = gn_open_serv_sock (serv_sock_list, "0.0.0.0", 8080);
    if (rgn_open_serv_sock != 0) fprintf (stderr, "Failed to open server socket.\n");
    rgn_open_serv_sock = gn_open_serv_sock (serv_sock_list, "127.0.0.1", 8081);
    if (rgn_open_serv_sock != 0) fprintf (stderr, "Failed to open server socket.\n");

    // Test code.
    gn_serv_sock_t * serv_sock = serv_sock_list->head;
    for (size_t i = 0; i < serv_sock_list->len; serv_sock = serv_sock->next, i++)
    {
        printf ("Server socket .fd: %i, .addr: [%s], .port: %i.\n", serv_sock->fd, serv_sock->addr, serv_sock->port);
    }

    // Detect the absolute path of the program.
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
            // Start worker processes.
            for (uint8_t i = 0; i < 2; i++)
            {
                gn_start_wrkr (self_path, ipc_sock, sun.sun_path, serv_sock_list);
            }
        }
    }

    // Main loop.
    while (true)
    {
        sleep (1);
    }
}