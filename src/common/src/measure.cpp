#include "measure.h"

namespace activebsp
{

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

Measure::Measure(std::string name)
    : _name(name) {}

void Measure::start()
{
    _start = now();
}

void Measure::end()
{
    _end = now();

}

std::string Measure::getName() const
{
    return _name;
}

double Measure::getTime() const
{
    return diff_to_sec(&_start, &_end);
}


}
