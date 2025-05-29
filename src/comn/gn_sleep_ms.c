#include <gn_sleep_ms.h>

void
gn_sleep_ms (const uint32_t ms)
{
    const struct timespec ts = {
        .tv_sec = 0,
        .tv_nsec = ms * 1000000
    };
    /* const int rclock_nanosleep = */ clock_nanosleep (CLOCK_MONOTONIC, 0, &ts, NULL);
    // TODO: Check rclock_nanosleep.
}