#include <gn_acpt_conns.h>

__attribute__((warn_unused_result))
int
gn_acpt_conns (int * const repoll_create1)
{
    struct epoll_event epoll_evts[64];
    memset (epoll_evts, 0, sizeof (epoll_evts));

    const int repoll_wait = epoll_wait (*repoll_create1, epoll_evts,
                                        sizeof (epoll_evts) / sizeof (struct epoll_event), 1000);
    switch (repoll_wait)
    {
        case 0:
        {
            // Timeout.
            break;
        }
        case -1:
        {
            switch (errno)
            {
                case EINTR:
                    break;
                case EBADF:
                case EINVAL:
                {
                    *repoll_create1 = -1;
                    __attribute__((fallthrough));
                }
                case EFAULT:
                {
                    fprintf (stderr, "epoll_wait() failed. %s.\n", strerror (errno));
                    return EXIT_FAILURE;
                }
                default:
                {
                    fprintf (stderr, "epoll_wait() returned undocumented errno code %i.\n", errno);
                    return EXIT_FAILURE;
                }
            }
            break;
        }
        default:
        {
            if (repoll_wait < -1)
            {
                fprintf (stderr, "epoll_wait() returned unexpected value %i.\n", repoll_wait);
                return EXIT_FAILURE;
            }

            for (unsigned int i = 0; i < (unsigned int)repoll_wait; i++)
            {
                const struct gn_serv_sock_t * const serv_sock = (gn_serv_sock_t *)epoll_evts[i].data.ptr;
                if (gn_acpt_conn (serv_sock) == EXIT_FAILURE) return EXIT_FAILURE;
            }
        }
    }

    return EXIT_SUCCESS;
}