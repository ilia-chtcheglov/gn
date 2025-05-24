#include <gn_set_sig_hndlr.h>

__attribute__((warn_unused_result))
bool
gn_set_sig_hndlr (const int signum, const __sighandler_t hndlr)
{
    struct sigaction sa;
    memset (&sa, 0, sizeof (sa));
    sa.sa_handler = hndlr;

    const int rsigaction = sigaction (signum, &sa, NULL);
    switch (rsigaction)
    {
        case 0:
            return EXIT_SUCCESS;
        case -1:
        {
            fprintf (stderr, "Failed to set signal %i handler. %s.\n", signum, strerror (errno));
            return EXIT_FAILURE;
        }
        default:
        {
            fprintf (stderr, "sigaction() returned undocumented value %i.\n", rsigaction);
            return EXIT_FAILURE;
        }
    }
}