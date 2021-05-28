#include "distributions/block_distribution.h"

namespace activebsp
{

void div_range (unsigned long n, int s, int p, unsigned long * beg, unsigned long * end)
{
    int block = n / p;
    int rem   = n % p;
    if (s < rem)
    {
        *beg = (block + 1) * s;
        *end = (block + 1) + *beg;
    }
    else
    {
        *beg = n - (p - s) * block;
        *end = block + *beg;
    }
}

}
