#include <gn_chck_cmdl_args.h>

__attribute__((warn_unused_result))
bool
gn_chck_cmdl_args (const int argc,
                   const char * const * const argv)
{
    // Checking argc.
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

    // Checking argv.
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

    return EXIT_SUCCESS;
}