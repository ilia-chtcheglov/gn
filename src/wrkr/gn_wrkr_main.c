#include <gn_wrkr_main.h>

void
gn_wrkr_main (const int ipc_sock)
{
    struct pollfd pfd = {
        .fd = ipc_sock,
        .events = POLLIN,
        .revents = 0
    };

    // Main loop.
    bool main_loop = true;
    while (main_loop)
    {
        const int rpoll = poll (&pfd, 1, 1000);
        switch (rpoll)
        {
            case 1:
            {
                if (pfd.revents & POLLHUP)
                {
                    printf ("IPC connection closed.\n");
                    main_loop = false;
                }
                break;
            }
            case 0:
            {
                break;
            }
            case -1:
            {
                switch (errno)
                {
                    case EINTR:
                    case ENOMEM:
                        break;
                    default:
                        fprintf (stderr, "Failed to poll IPC socket. %s.\n", strerror (errno));
                }
                break;
            }
            default:
            {
                
            }
        }
    }
}