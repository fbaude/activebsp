#ifndef __CONTIGUOUS_ASSIGN_H__
#define __CONTIGUOUS_ASSIGN_H__

#include <vector>

namespace activebsp
{

std::vector<int> allocate_contiguous_procs(int offset, int p);

class ContiguousProcessAllocator
{
private:
    int _offset;
    int _cur_offset;
    int _p;
    int _cur_p;

public:
    ContiguousProcessAllocator(int offset, int p);

    std::vector<int> allocate(int p);
};


} // namespace activebsp


#endif // __CONTIGUOUS_ASSIGN_H__
