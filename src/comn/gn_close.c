#include <gn_close.h>

int
gn_close (int * const fd)
{
    if (fd == NULL)
    {
        errno = EINVAL;
        return -1;
    }

    const int rclose = close (*fd);
    *fd = -1;
    return rclose;
}