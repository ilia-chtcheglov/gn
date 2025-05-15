#include <main.h>

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
    if (argc < GN_CMDL_ARGS_MIN)
    {
        fprintf (stderr, "Number of command line arguments (%i) too low (minimum %i).\n", argc, GN_CMDL_ARGS_MIN);
        return EXIT_FAILURE;
    }
    if (argc > GN_CMDL_ARGS_MAX)
    {
        fprintf (stderr, "Number of command line arguments (%i) too high (maximum %i).\n", argc, GN_CMDL_ARGS_MAX);
        return EXIT_FAILURE;
    }

    // Checking @argv.
    if (argv[0] == NULL)
    {
        fprintf (stderr, "argv[0] should not be NULL but is.\n");
        return EXIT_FAILURE;
    }
    if (argv[argc] != NULL)
    {
        fprintf (stderr, "argv[%i] should be NULL but is not.\n", argc);
        return EXIT_FAILURE;
    }

    /*
     * Contains the address of the Unix socket used for
     * IPC between the master process and the worker processes.
     * If this variable is not NULL the process will be a worker process.
     */
    const char * ipc_addr_str = NULL;

    // Parse command line arguments.
    for (int argi = 1; argi < argc; argi++)
    {
        if (argv[argi] == NULL)
        {
            fprintf (stderr, "argv[%i] should not be NULL but is.\n", argi);
            return EXIT_FAILURE;
        }

        if (strcmp (argv[argi], "--ipc") == 0)
        {
            if (ipc_addr_str != NULL)
            {
                fprintf (stderr, "Command line argument \"--ipc\" already used.\n");
                return EXIT_FAILURE;
            }
            if (++argi >= argc)
            {
                fprintf (stderr, "Missing value after \"--ipc\" command line argument.\n");
                return EXIT_FAILURE;
            }
            if (argv[argi] == NULL)
            {
                fprintf (stderr, "argv[%i] should not be NULL but is.\n", argi);
                return EXIT_FAILURE;
            }

            const struct sockaddr_un sun = { 0 };
            const size_t ipc_addr_str_len = strlen (argv[argi]);
            if (ipc_addr_str_len < 1)
            {
                fprintf (stderr, "IPC address too short (minimum 1).\n");
                return EXIT_FAILURE;
            }
            if (ipc_addr_str_len >= sizeof (sun.sun_path))
            {
                fprintf (stderr, "IPC address too long (maximum %lu).\n", sizeof (sun.sun_path) - 1);
                return EXIT_FAILURE;
            }

            ipc_addr_str = argv[argi];
        }
        else
        {
            fprintf (stderr, "Unsupported command line argument \"%s\".\n", argv[argi]);
            return EXIT_FAILURE;
        }
    }

    /*
     * Open a Unix IPC socket. It will be used by:
     * - The master process to control worker processes.
     * - Worker processes to receive commands, send statistics to the master, etc.
     */
    int ipc_sock = socket (AF_UNIX, SOCK_STREAM | SOCK_CLOEXEC | SOCK_NONBLOCK, 0);
    if (ipc_sock < 0)
    {
        fprintf (stderr, "Failed to create IPC socket. %s.\n", strerror (errno));
        return EXIT_FAILURE;
    }

    // List of server sockets.
    gn_serv_sock_list_t serv_sock_list;
    memset (&serv_sock_list, 0, sizeof (gn_serv_sock_list_t));

    if (ipc_addr_str == NULL) gn_mstr_main (ipc_sock, &serv_sock_list);
    else gn_wrkr_main (ipc_sock, &serv_sock_list, ipc_addr_str);

    // Close server sockets.
    gn_close_serv_socks (&serv_sock_list);

    // Close the IPC socket.
    gn_close (&ipc_sock);

    return EXIT_SUCCESS;
}