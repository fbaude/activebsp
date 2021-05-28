#ifndef __BSPLIB_H__
#define __BSPLIB_H__

#include <functional>

#include <mpi.h>

namespace activebsp
{

class BSPlib
{
private:
    MPI_Comm _comm;
    std::function<void(BSPlib *)> _init_f;

public:
    BSPlib(MPI_Comm comm, std::function<void(BSPlib *)> init_f);

    MPI_Comm getMPICommDup();

    void init();
    void proceed();

    int nprocs ();
    int pid ();
    double time ();

    void sync();

    // BSP MP
    void send (int, const void *, const void *, size_t);
    void qsize (int *, size_t *);
    void get_tag (int *, void *);
    void move (void *, size_t);
    void set_tagsize (size_t *);

    // BSP DRMA
    void push_reg (const void *ident, size_t size);
    void pop_reg (const void *ident);
    void put (int pid, const void *src, void *dst, long int offset, size_t nbytes);
    void get (int pid, const void *src, long int offset, void *dst, size_t nbytes);
};

} // namespace activebsp

#endif // __BSPLIB_H__
