#include "vector_distribution.h"

#include <algorithm>
#include <iostream>

namespace activebsp
{

vector_distribution_base::vector_distribution_base(int nelems /* = 1 */)
    : _buf(size_for_elems(nelems))
{
    vector_distribution_buf_t * buf = getStructBuf();
    buf->nelem = nelems;
}

vector_distribution_base::vector_distribution_base(const vector_distribution_base & other)
    : _buf(other._buf) {}
/*
vector_distribution::vector_distribution(const vector_distribution && other)
    : _buf(std::move(other._buf)) {}
*/
void vector_distribution_base::swap(vector_distribution_base & other)
{
    std::swap(_buf, other._buf);
}

vector_distribution_base & vector_distribution_base::operator = (const vector_distribution_base & other)
{
    vector_distribution_base temp(other);
    swap(temp);

    return *this;
}
/*
vector_distribution & vector_distribution::operator = (vector_distribution && other)
{
    vector_distribution temp(other);
    swap(temp);

    return *this;
}
*/
vector_distribution_base::vector_distribution_buf_t * vector_distribution_base::getStructBuf() const
{
    return (vector_distribution_buf_t *) getBuf();
}

int vector_distribution_base::size_for_elems(int nelem)
{
    return 2 * sizeof(int) + nelem * sizeof(vector_distribution_part_t);
}

bool vector_distribution_base::range_overlap(int x1, int x2, int y1, int y2)
{
    return x1 <= y2 && y1 <= x2;
}

void vector_distribution_base::register_part(int i, int pid, int resid, size_t size, size_t offset)
{
    getStructBuf()->parts[i] = {pid,resid,size,offset};
}


vector_distribution_part_t * vector_distribution_base::getPart(int i)
{
    return &getStructBuf()->parts[i];
}

const vector_distribution_part_t * vector_distribution_base::getPart(int i) const
{
    return &getStructBuf()->parts[i];
}

size_t vector_distribution_base::getBufSize() const
{
    return _buf.size();
}

const char * vector_distribution_base::getBuf() const
{
    return &_buf[0];
}

char * vector_distribution_base::getBuf()
{
    return &_buf[0];
}

int vector_distribution_base::nparts() const
{
    return getStructBuf()->nelem;
}

void vector_distribution_base::resize(size_t elems)
{
    _buf.resize(size_for_elems(elems));
}

size_t vector_distribution_base::getVecSize() const
{
    size_t max_size = 0;

    for (int i = 0; i < nparts(); ++i)
    {
        const vector_distribution_part_t * part = getPart(i);
        max_size = std::max(max_size, part->offset + part->size);
    }

    return max_size;
}

std::ostream & operator << (std::ostream & os, const vector_distribution_base & dv)
{
    int nparts = dv.nparts();

    os << "dv of " << nparts << " parts : {";

    for (int i = 0; i < nparts; ++i)
    {
        const vector_distribution_part_t * part = dv.getPart(i);

        if (i != 0)
        {
            os << ", ";
        }

        os << "(" << part->pid << "," << part->resid << "," << part->offset << "," << part->size << ")";
    }

    os << "}";

    return os;
}

} // namespace activebsp

