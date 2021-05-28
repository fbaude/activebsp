#include "measure.h"

struct timespec now()
{
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);

    return t;
}

double diff_to_sec(const struct timespec * beg, const struct timespec * end)
{
    return end->tv_sec - beg->tv_sec + 1e-9 * (end->tv_nsec - beg->tv_nsec);
}
