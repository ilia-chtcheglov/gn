#include <stdio.h>
#include <stdlib.h>

// Maximum number of command line arguments.
#define GN_MAX_CMDL_ARGS 8

int
main (const int argc,
      __attribute__((unused)) const char * const * const argv)
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

    return EXIT_SUCCESS;
}