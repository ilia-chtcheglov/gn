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

__attribute__((warn_unused_result))
gn_vhst_conf_t *
gn_vhst_conf_list_pop (gn_vhst_conf_list_t * const list);

__attribute__((warn_unused_result))
gn_vhst_conf_t *
gn_vhst_conf_list_pop (gn_vhst_conf_list_t * const list)
{
    switch (list->len)
    {
        case 0:
        {
            return NULL;
        }
        case 1:
        {
            gn_vhst_conf_t * const ret = list->head;
            list->head = list->tail = NULL;
            list->len--;
            return ret;
        }
        default:
        {
            gn_vhst_conf_t * const ret = list->head;
            list->head = list->head->next;

            list->head->prev = list->tail;
            list->tail->next = list->head;

            list->len--;
            return ret;
        }
    }
}

#include <fcntl.h>
#include <sys/file.h>

__attribute__((nonnull))
void
gn_mstr_init (int ipc_sock,
              gn_serv_sock_list_t * const serv_sock_list)
{
    if (gn_set_sig_hndlr (SIGINT, gn_sigint_handler) != EXIT_SUCCESS) return;

    int lock_fd = open ("/run/gn.lock", O_RDONLY | O_CLOEXEC | O_CREAT, S_IRUSR | S_IRGRP);
    if (lock_fd < 0)
    {
        fprintf (stderr, "Failed to create/open the lock file. %s.\n", strerror (errno));
        return;
    }

    bool try_lock = true;
    while (try_lock)
    {
        const int rflock = flock (lock_fd, LOCK_EX | LOCK_NB);
        switch (rflock)
        {
            case 0:
            {
                try_lock = false;
                break;
            }
            case -1:
            {
                switch (errno)
                {
                    case EINTR:
                        break;
                    default:
                    {
                        fprintf (stderr, "Failed to lock the lock file. %s.\n", strerror (errno));
                        gn_close (&lock_fd);
                        return;
                    }
                }
                break;
            }
            default:
            {
                fprintf (stderr, "flock() returned undocumented value %i.\n", rflock);
                gn_close (&lock_fd);
                return;
            }
        }
    }

    // Detect the absolute path of the program.
    char * self_path = gn_self_path ();
    if (self_path == NULL) return;

    char * ipc_addr_str = gn_ipc_prep (ipc_sock);
    if (ipc_addr_str == NULL) goto labl_free_self_path;

    gn_mstr_conf_t mstr_conf = GN_MSTR_CONF_INIT;
    gn_load_mstr_conf (&mstr_conf);

    gn_vhst_conf_list_t vhst_conf_list;
    memset (&vhst_conf_list, 0, sizeof (vhst_conf_list));
    gn_load_vhsts_conf (&vhst_conf_list);

    // Test code.
    gn_vhst_conf_t * vhst_conf = vhst_conf_list.head;
    for (uint32_t i = 0; i < vhst_conf_list.len; vhst_conf = vhst_conf->next, i++)
    {
        printf (" - Vhost:\n");
        printf ("   * document_root: \"%s\".\n", vhst_conf->document_root);
    }

    // Open server sockets.
    int rgn_open_serv_sock = gn_open_serv_sock (serv_sock_list, "0.0.0.0", 8080);
    if (rgn_open_serv_sock != 0) fprintf (stderr, "Failed to open server socket.\n");
    rgn_open_serv_sock = gn_open_serv_sock (serv_sock_list, "127.0.0.1", 8081);
    if (rgn_open_serv_sock != 0) fprintf (stderr, "Failed to open server socket.\n");

    // Test code.
    gn_serv_sock_t * serv_sock = serv_sock_list->head;
    for (uint16_t i = 0; i < serv_sock_list->len; serv_sock = serv_sock->next, i++)
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

    // Start worker processes.
    gn_start_wrkrs (&wrkr_data_list, repoll_create1, mstr_conf.workers,
                    self_path, ipc_sock, ipc_addr_str, serv_sock_list);

    gn_mstr_main (&wrkr_data_list, &repoll_create1, self_path,
                  ipc_sock, ipc_addr_str, serv_sock_list);

    // Stop worker processes.
    gn_stop_wrkrs (&wrkr_data_list);

    gn_close (&repoll_create1);

    while (vhst_conf_list.len > 0)
    {
        gn_vhst_conf_t * const conf = gn_vhst_conf_list_pop (&vhst_conf_list);
        free (conf->document_root);
        free (conf);
    }

    free (ipc_addr_str);
    ipc_addr_str = NULL;

    labl_free_self_path:
    free (self_path);
    self_path = NULL;
}