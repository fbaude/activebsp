#ifndef MEASURE_H
#define MEASURE_H

#include <time.h>

#include <string>

struct timespec now();

double diff_to_sec(const struct timespec * beg, const struct timespec * end);


#endif // MEASURE_H
