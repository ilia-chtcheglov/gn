#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/un.h>
#include <unistd.h>

// Maximum number of command line arguments.
#define GN_MAX_CMDL_ARGS 8

void
gn_mstr_main (int ipc_sock);

void
gn_wrkr_main (int ipc_sock,
              const char * const ipc_addr_str);

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

    const char * ipc_addr_str = NULL;

    // Parse command line arguments.
    for (int argi = 1; argi < argc; argi++)
    {
        if (strcmp (argv[argi], "--ipc") == 0)
        {
            if (ipc_addr_str != NULL)
            {
                fprintf (stderr, "Command line argument \"--ipc\" already used.\n");
                return 1;
            }
            if (++argi < argc)
            {
                ipc_addr_str = argv[argi];
            }
            else
            {
                fprintf (stderr, "Missing value after \"--ipc\" command line argument.\n");
                return 1;
            }
        }
        else
        {
            fprintf (stderr, "Unsupported command line argument \"%s\".\n", argv[argi]);
            return 1;
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

    if (ipc_addr_str == NULL) gn_mstr_main (ipc_sock);
    else gn_wrkr_main (ipc_sock, ipc_addr_str);

    // Close the IPC socket.
    close (ipc_sock);
    ipc_sock = -1;

    return EXIT_SUCCESS;
}