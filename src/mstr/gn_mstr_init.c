#include <gn_mstr_init.h>

bool sigint_rcvd = false;

void
gn_sigint_handler (const int signum)
{
    if (signum != SIGINT)
    {
        fprintf (stderr, "SIGINT handler called for signal %i.\n", signum);
        // TODO: Maybe stop process in this case.
        return;
    }

    sigint_rcvd = true;
}

void
gn_mstr_init (int ipc_sock, gn_serv_sock_list_t * const serv_sock_list)
{
    if (gn_set_sig_hndlr (SIGINT, gn_sigint_handler) != EXIT_SUCCESS) return;

    char * ipc_addr_str = gn_ipc_prep (ipc_sock);
    if (ipc_addr_str == NULL) return;

    gn_mstr_conf_t mstr_conf = GN_MSTR_CONF_INIT;
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