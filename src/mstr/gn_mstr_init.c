#include <gn_mstr_init.h>

typedef struct gn_vhst_conf_list_t gn_vhst_conf_list_t;

struct gn_vhst_conf_list_t
{
    gn_vhst_conf_list_t * prev;
    gn_vhst_conf_list_t * next;
};

#include <dirent.h>

#include <json-c/json.h>

#define GN_VHSTS_CONF_DIR_PATH "/etc/gn/vhosts/enabled/"

void
gn_load_vhsts_conf (gn_vhst_conf_list_t * const vhst_conf_list);

void
gn_load_vhsts_conf (gn_vhst_conf_list_t * const vhst_conf_list)
{
    (void)vhst_conf_list; // TODO: Remove.

    DIR * ropendir = opendir (GN_VHSTS_CONF_DIR_PATH);
    if (ropendir == NULL)
    {
        fprintf (stderr, "Failed to open virtual hosts configuration directory. %s.\n", strerror (errno));
        return;
    }

    errno = 0;
    struct dirent * ent = NULL;
    while ((ent = readdir (ropendir)) != NULL)
    {
        if (strcmp (ent->d_name, ".") == 0 || strcmp (ent->d_name, "..") == 0) continue;
        // TODO: Check ent->d_type.
        if (strcmp (ent->d_name, ".conf") == 0)
        {
            fprintf (stderr, "Invalid virtual host configuration file name \".conf\".\n");
            break;
        }
        if (strlen (ent->d_name) < 6 ||
            strcmp (&ent->d_name[strlen (ent->d_name) - 5], ".conf") != 0)
        {
            fprintf (stderr, "Virtual host configuration file \"%s\" doesn't end with \".conf\".\n", ent->d_name);
            break;
        }

        printf ("d_name == \"%s\"\n", ent->d_name); // TODO: Remove.
        char * path = (char *)malloc (strlen (GN_VHSTS_CONF_DIR_PATH) + strlen (ent->d_name) + 1);
        if (path == NULL)
        {
            // TODO: Display error message.
            break;
        }

        strcpy (path, GN_VHSTS_CONF_DIR_PATH);
        strcat (path, ent->d_name);

        json_object * root = json_object_from_file (path);
        if (root == NULL)
        {
            fprintf (stderr, "Failed to parse virtual host configuration file \"%s\".\n", path);
            free (path);
            path = NULL;
            break;
        }

        json_object_put (root);
        root = NULL;

        free (path);
        path = NULL;
    }

    if (errno != 0)
    {
        fprintf (stderr, "Error while reading virtual hosts configuration directory. %s.\n", strerror (errno));
        // TODO: if (errno == XYZ)...
        ropendir = NULL;
    }

    // TODO: Check closedir() return value and errno.
    closedir (ropendir);
    ropendir = NULL;
}

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

__attribute__((warn_unused_result))
bool
gn_ipc_lstn (const int ipc_sock);

__attribute__((warn_unused_result))
bool
gn_ipc_lstn (const int ipc_sock)
{
    const int rlisten = listen (ipc_sock, 8);
    switch (rlisten)
    {
        case 0:
            return EXIT_SUCCESS;
        case -1:
        {
            fprintf (stderr, "Failed to listen on IPC socket. %s.\n", strerror (errno));
            return EXIT_FAILURE;
        }
        default:
        {
            fprintf (stderr, "listen() returned undocumented value %i.\n", rlisten);
            return EXIT_FAILURE;
        }
    }
}

__attribute__((warn_unused_result))
bool
gn_ipc_bind (const int ipc_sock, struct sockaddr_un * const ipc_addr);

__attribute__((warn_unused_result))
bool
gn_ipc_bind (const int ipc_sock, struct sockaddr_un * const ipc_addr)
{
    const int rbind = bind (ipc_sock, (struct sockaddr *)ipc_addr, sizeof (struct sockaddr_un));
    switch (rbind)
    {
        case 0:
            return EXIT_SUCCESS;
        case -1:
        {
            fprintf (stderr, "Failed to bind IPC socket. %s.\n", strerror (errno));
            return EXIT_FAILURE;
        }
        default:
        {
            fprintf (stderr, "bind() returned undocumented value %i.\n", rbind);
            return EXIT_FAILURE;
        }
    }
}

__attribute__((malloc))
__attribute__((warn_unused_result))
char *
gn_ipc_prep (const int ipc_sock);

__attribute__((malloc))
__attribute__((warn_unused_result))
char *
gn_ipc_prep (const int ipc_sock)
{
    // Get current time to generate a path for the IPC socket.
    struct timespec ts;
    memset (&ts, 0, sizeof (ts));

    const int rclock_gettime = clock_gettime (CLOCK_REALTIME, &ts);
    switch (rclock_gettime)
    {
        case 0:
            break;
        case -1:
        {
            fprintf (stderr, "Failed to get current time. %s.\n", strerror (errno));
            return NULL;
        }
        default:
        {
            fprintf (stderr, "clock_gettime() returned undocumented value %i.\n", rclock_gettime);
            return NULL;
        }
    }

    /*
     * Generate an address for the IPC socket. The address is made of:
     * - Current process ID in hexadecimal.
     * - Time in seconds in hexadecimal.
     * - Nanoseconds in hexadecimal.
     * - The first byte will be replaced with '\0' to make the address an abstract one.
     */
    struct sockaddr_un sun;
    memset (&sun, 0, sizeof (sun));

    sun.sun_family = AF_UNIX;
    // sun.sun_path is sun_path[108].

    int rsnprintf = snprintf (sun.sun_path, sizeof (sun.sun_path), "Z%X%lX%lX",
                              (unsigned int)getpid (), (long unsigned int)ts.tv_sec, (long unsigned int)ts.tv_nsec);
    printf ("rsnprintf: %i, sun_path: \"%s\".\n", rsnprintf, sun.sun_path);

    /*
     * Setting the first byte of .sun_path to '\0' will turn the socket address inside .sun_path
     * into an abstract socket address. No files will be created on disk.
     */
    sun.sun_path[0] = '\0';

    if (gn_ipc_bind (ipc_sock, &sun) != EXIT_SUCCESS) return NULL;

    // Set sun_path[0] back to some byte because some gn_* functions can't work if @sun_path starts with '\0'.
    sun.sun_path[0] = 'Z';

    if (gn_ipc_lstn (ipc_sock) != EXIT_SUCCESS) return NULL;

    char * ipc_addr_str = (char *)malloc (strlen (sun.sun_path) + 1);
    if (ipc_addr_str == NULL)
    {
        fprintf (stderr, "Failed to allocate buffer for IPC address string.\n");
        return NULL;
    }

    strcpy (ipc_addr_str, sun.sun_path);
    return ipc_addr_str;
}

void
gn_mstr_init (int ipc_sock, gn_serv_sock_list_t * const serv_sock_list)
{
    if (gn_set_sig_hndlr (SIGINT, gn_sigint_handler) != EXIT_SUCCESS) return;

    char * ipc_addr_str = gn_ipc_prep (ipc_sock);
    if (ipc_addr_str == NULL) return;

    gn_mstr_conf_t mstr_conf;
    mstr_conf.workers = 1;
    mstr_conf.connection_acceptance_threads = 1;
    mstr_conf.connection_management_threads = 1;

    gn_load_mstr_conf (&mstr_conf);

    gn_vhst_conf_list_t vhst_conf_list;
    memset (&vhst_conf_list, 0, sizeof (vhst_conf_list));
    gn_load_vhsts_conf (&vhst_conf_list);

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

    // Create an epoll instance for workers.
    int repoll_create1 = epoll_create1 (EPOLL_CLOEXEC);
    if (repoll_create1 < 0)
    {
        fprintf (stderr, "Failed to create workers epoll instance. %s.\n", strerror (errno));
        return;
    }

    // List of worker data structures (PID, IPC socket, etc).
    gn_wrkr_data_list_t wrkr_data_list;
    memset (&wrkr_data_list, 0, sizeof (gn_wrkr_data_list_t));

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
            gn_start_wrkrs (&wrkr_data_list, repoll_create1, mstr_conf.workers, self_path,
                            ipc_sock, ipc_addr_str, serv_sock_list);
        }
    }

    gn_mstr_main (&wrkr_data_list, &repoll_create1, self_path, ipc_sock, ipc_addr_str, serv_sock_list);

    // Stop worker processes.
    gn_stop_wrkrs (&wrkr_data_list);

    gn_close (&repoll_create1);

    free (ipc_addr_str);
    ipc_addr_str = NULL;
}