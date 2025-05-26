#include <gn_prse_cmdl_args.h>

__attribute__((nonnull))
__attribute__((warn_unused_result))
bool
gn_prse_cmdl_args (const int argc,
                   const char * const * const argv,
                   const char ** const ipc_addr_str)
{
    for (int argi = 1; argi < argc; argi++)
    {
        if (argv[argi] == NULL)
        {
            fprintf (stderr, "argv[%i] should not be NULL but is.\n", argi);
            return EXIT_FAILURE;
        }

        if (strcmp (argv[argi], "--ipc") == 0)
        {
            if (*ipc_addr_str != NULL)
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
            if (ipc_addr_str_len < 4)
            {
                fprintf (stderr, "IPC address too short (minimum 4 bytes).\n");
                return EXIT_FAILURE;
            }
            if (ipc_addr_str_len >= sizeof (sun.sun_path))
            {
                fprintf (stderr, "IPC address too long (maximum %lu bytes).\n", sizeof (sun.sun_path) - 1);
                return EXIT_FAILURE;
            }

            *ipc_addr_str = argv[argi];
        }
        else
        {
            fprintf (stderr, "Unsupported command line argument \"%s\".\n", argv[argi]);
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}