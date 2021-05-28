#ifndef __ACTIVEBSP_H__
#define __ACTIVEBSP_H__

#include <string>
#include <vector>

#include <mpi.h>

#include "ActiveBSPlib.h"
#include "Proxy.h"
#include "decl_actor.h"
#include "vector_distribution.h"

namespace activebsp
{

class ActorRegistryBase
{
    void registerActors() {}
};

void activebsp_init();
void activebsp_init(int *argc, char **argv[]);
void activebsp_finalize();

int absp_nprocs();
int absp_pid();

template <class T>
Proxy<T> createActiveObject(const std::vector <int> & pids);

std::vector<char> dv_get_part(const vector_distribution_base & dv, size_t offset, size_t size);
void dv_get_part(const vector_distribution_base & dv, size_t offset, char * out_buf, size_t size);

template <class T>
std::vector<T> dv_get_part(const vector_distribution<T> & dv, size_t offset, size_t size);

template <class T>
void dv_get_part(const vector_distribution<T> & dv, size_t offset, T * out_buf, size_t size);

void dv_release(const vector_distribution_base & dv);

} // namespace activebsp

#include "activebsp.hpp"

#endif // __ACTIVEBSP_H__
