#include <gn_mstr_main.h>

bool sigint_rcvd = false;

void
gn_sigint_handler (const int signum);

void
gn_sigint_handler (const int signum)
{
    (void)signum;
    printf ("Master process received SIGINT.\n");
    sigint_rcvd = true;
}

#include <json-c/json.h>

typedef struct
{
    uint8_t workers;
    uint8_t connection_acceptance_threads;
    uint8_t connection_management_threads;
} gn_mstr_conf_t;

void
gn_load_mstr_conf (gn_mstr_conf_t * const mstr_conf);

void
gn_load_mstr_conf (gn_mstr_conf_t * const mstr_conf)
{
    json_object * root = json_object_from_file ("/etc/gn/master.conf");
    if (root == NULL)
    {
        fprintf (stderr, "Failed to parse master configuration file.\n");
        return;
    }

    json_object_object_foreach(root, key, val)
    {
        if (strcmp (key, "workers") == 0)
        {
            if (json_object_get_type (val) != json_type_int)
            {
                fprintf (stderr, "The value of directive \"%s\" must be an integer.\n", key);
                break;
            }

            const int32_t num = json_object_get_int (val);
            if (num < 0)
            {
                fprintf (stderr, "Workers count cannot be lesser than zero.\n");
                break;
            }
            if (num > UINT8_MAX)
            {
                fprintf (stderr, "Workers count cannot be greater than %i.\n", UINT8_MAX);
                break;
            }

            mstr_conf->workers = (uint8_t)num;

            printf ("\"%s\": %i\n", key, mstr_conf->workers);
        }
        else if (strcmp (key, "connection_acceptance_threads") == 0)
        {
            if (json_object_get_type (val) != json_type_int)
            {
                fprintf (stderr, "The value of directive \"%s\" must be an integer.\n", key);
                break;
            }

            const int32_t num = json_object_get_int (val);
            if (num < 0)
            {
                fprintf (stderr, "Connection acceptance threads count cannot be lesser than zero.\n");
                break;
            }
            if (num > UINT8_MAX)
            {
                fprintf (stderr, "Connection acceptance threads count cannot be greater than %i.\n", UINT8_MAX);
                break;
            }

            mstr_conf->connection_acceptance_threads = (uint8_t)num;

            printf ("\"%s\": %i\n", key, mstr_conf->connection_acceptance_threads);
        }
        else if (strcmp (key, "connection_management_threads") == 0)
        {
            if (json_object_get_type (val) != json_type_int)
            {
                fprintf (stderr, "The value of directive \"%s\" must be an integer.\n", key);
                break;
            }

            const int32_t num = json_object_get_int (val);
            if (num < 0)
            {
                fprintf (stderr, "Connection management threads count cannot be lesser than zero.\n");
                break;
            }
            if (num > UINT8_MAX)
            {
                fprintf (stderr, "Connection management threads count cannot be greater than %i.\n", UINT8_MAX);
                break;
            }

            mstr_conf->connection_management_threads = (uint8_t)num;

            printf ("\"%s\": %i\n", key, mstr_conf->connection_management_threads);
        }
        else
        {
            fprintf (stderr, "Unsupported configuration directive \"%s\".\n", key);
            break;
        }
    }

    json_object_put (root);
}

void
gn_mstr_main (int ipc_sock, gn_serv_sock_list_t * const serv_sock_list)
{
    // TODO: Use sigaction() instead of signal().
    if (signal (SIGINT, gn_sigint_handler) == SIG_ERR)
    {
        fprintf (stderr, "Failed to set SIGINT handler.\n");
        return;
    }

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

    /*
     * Generate an address for the IPC socket. The address is made of:
     * - Current process ID in hexadecimal.
     * - Time in seconds in hexadecimal.
     * - Nanoseconds in hexadecimal.
     * - The first byte will be replaced with '\0' to make the address an abstract one.
     */
    int rsnprintf = snprintf (sun.sun_path, sizeof (sun.sun_path), "Z%X%lX%lX",
                              (unsigned int)getpid (), (long unsigned int)ts.tv_sec, (long unsigned int)ts.tv_nsec);
    printf ("rsnprintf: %i, sun_path: \"%s\".\n", rsnprintf, sun.sun_path);

    /*
     * Setting the first byte of .sun_path to '\0' will turn the socket address inside .sun_path
     * into an abstract socket address. No files will be created on disk.
     */
    sun.sun_path[0] = '\0';

    const int rbind = bind (ipc_sock, (struct sockaddr *)&sun, sizeof (sun));
    switch (rbind)
    {
        case 0:
        {
            // Set sun_path[0] back to some byte because some gn_* functions can't work if @sun_path starts with '\0'.
            sun.sun_path[0] = 'Z';

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

    gn_mstr_conf_t mstr_conf;
    mstr_conf.workers = 1;
    mstr_conf.connection_acceptance_threads = 1;
    mstr_conf.connection_management_threads = 1;

    gn_load_mstr_conf (&mstr_conf);

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
            gn_start_wrkrs (mstr_conf.workers, self_path, ipc_sock, sun.sun_path, serv_sock_list);
        }
    }

    // Main loop.
    while (true)
    {
        if (sigint_rcvd) break;
        sleep (1);
    }
}