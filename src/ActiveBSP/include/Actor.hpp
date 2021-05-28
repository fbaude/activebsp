#ifndef __ACTOR_HPP__
#define __ACTOR_HPP__

#include "Actor.h"

#include <mpi.h>

#include "force_cast.hpp"

namespace activebsp
{

template <class T>
void ActorBase::register_spmd (void (T::*spmd_function)())
{
    uint64_t addr = force_cast<uint64_t>(spmd_function);
    _spmd_function_ids[addr] = _spmd_functions.size();

    _spmd_functions.push_back(std::bind(spmd_function, static_cast<T*>(this)));
}

template <class T>
void ActorBase::bsp_run (void (T::*spmd_function)())
{
    uint64_t addr = force_cast<uint64_t>(spmd_function);
    int id = _spmd_function_ids[addr];

    run_function_id(id);
}

template <class T>
int ActorBase::store_result(const T * buf, size_t size)
{
    return store_result((char *) buf, size * sizeof(T));
}

template <class T>
void ActorBase::register_single_part_result(const T * buf, size_t size, size_t offset)
{
    register_single_part_result( (char *) buf, size * sizeof(T), offset * sizeof(T));
}

template <class T>
std::vector<T> ActorBase::get_part(const vector_distribution<T> & dv, size_t offset, size_t size)
{
    std::vector<T> vec(size);
    get_part<T>(dv, offset, vec.data(), size);

    return vec;
}

template <class T>
void ActorBase::get_part(const vector_distribution<T> & dv, size_t offset, T * out_buf, size_t size)
{
    get_part(dv, offset * sizeof(T), (char *) out_buf, size * sizeof(T));
}

template <class T>
vector_distribution<T> ActorBase::gather_distr_parts()
{
    vector_distribution<T> dv (_cur_vector_distribution.nparts() * bsp_nprocs());

    char * base = (char *) _cur_vector_distribution.getBuf();
    char * dst  = (char *) dv.getBuf();
    size_t part_size = _cur_vector_distribution.nparts() * vector_distribution_base::parts_size;

    for (int s = 0; s < bsp_nprocs(); ++s)
    {
        bsp_get(s, base,  vector_distribution_base::parts_offset,
                   dst  + vector_distribution_base::parts_offset + s * part_size, part_size);
    }

    bsp_seq_sync();

    return dv;
}


} // namespace activebsp

#endif // __ACTOR_HPP__
