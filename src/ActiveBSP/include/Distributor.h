#ifndef DISTRIBUTOR_H
#define DISTRIBUTOR_H

#include "distributions/block_distribution.h"

namespace activebsp
{

typedef struct
{
    size_t offset;
    size_t size;
} distribution_item_t;

typedef std::vector<distribution_item_t> distribution_t;

class BlockDistributor
{
    int _s;
    int _p;

  public:
    BlockDistributor(int s, int p) : _s(s), _p(p) {}

    distribution_t distribution_part(size_t size)
    {
        distribution_t res;

        size_t offset, part_size, part_end;

        div_range(size, _s, _p, &offset, &part_end);
        part_size = part_end - offset;

        res.push_back({offset,part_size});

        return res;
    }

    class index_iterator
    {
        size_t _index;
        size_t _mapped_index;
     public:
        index_iterator()
            : _index(0), _mapped_index(0) {}

        size_t getIndex() const
        {
            return _index;
        }

        void next()
        {

        }
    };


};


}

#endif // DISTRIBUTOR_H
