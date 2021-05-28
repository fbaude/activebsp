#ifndef __ACTIVEBSP_HPP__
#define __ACTIVEBSP_HPP__

#include "activebsp.h"

namespace activebsp
{

template <class T>
std::vector<T> dv_get_part(const vector_distribution<T> & dv, size_t offset, size_t size)
{
    std::vector<T> vec(size);
    dv_get_part<T>(dv, offset, vec.data(), size);

    return vec;
}

template <class T>
void dv_get_part(const vector_distribution<T> & dv, size_t offset, T * out_buf, size_t size)
{
    std::shared_ptr<ActorCommunicator> comm = SyntaxHelper::getInstance()->getComm();
    comm->dv_get_part(dv, offset * sizeof(T), (char *) out_buf, size * sizeof(T));
}

} // namespace activebsp

#endif // __ACTIVEBSP_HPP__
