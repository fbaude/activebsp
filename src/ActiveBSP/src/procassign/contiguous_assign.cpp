#include "procassign/contiguous_assign.h"

#include <iostream>

namespace activebsp
{

std::vector<int> allocate_contiguous_procs(int offset, int p)
{
    std::vector<int> pids(p);
    for (int i = 0; i < p; ++i)
    {
        pids[i] = offset + i;
    }

    return pids;
}

ContiguousProcessAllocator::ContiguousProcessAllocator(int offset, int p)
    : _offset(offset), _cur_offset(offset), _p(p), _cur_p(p)
{}

std::vector<int> ContiguousProcessAllocator::allocate(int p)
{
    std::vector<int> pids;

    if (_cur_p < p)
    {
        std::cerr << "Not enough processes remaining to assign " << p << " processes, " << _cur_p << " remaining." << std::endl;
        return pids;
    }

    pids = allocate_contiguous_procs(_cur_offset, p);

    _cur_p -= p;
    _cur_offset += p;

    return pids;
}

} // namespace activebsp
