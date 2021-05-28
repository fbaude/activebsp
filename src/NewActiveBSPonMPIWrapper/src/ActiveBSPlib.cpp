#include "ActiveBSPlib.h"

#include <iostream>

#include "log.h"

#include "bsp.h"
#include "bsp_mpi_extension.h"

namespace activebsp
{

BSPlib * bsp_obj;
std::function<void(BSPlib *)> next_spmd;
int bsp_begin_nprocs;

BSPlib::BSPlib(MPI_Comm comm, std::function<void(BSPlib *)> init_f)
    : _comm(comm), _init_f(init_f)
{
    LOG_TRACE("%s", "Giving subset communicator to BSPonMPI");

    bsp_use_world_subset(comm);
}

MPI_Comm BSPlib::getMPICommDup()
{
    MPI_Comm comm;
    MPI_Comm_dup(_comm, &comm);

    return comm;;
}

void prepare_spmd();

void BSPlib::init()
{
    int rank;
    next_spmd = _init_f;
    bsp_obj = this;


    LOG_TRACE("%s", "Querying communicator");

    MPI_Comm_rank(_comm, &rank);
    MPI_Comm_size(_comm, &bsp_begin_nprocs);

    LOG_TRACE("Process %d of %d within subset communicator entering bsp_init", rank, bsp_begin_nprocs);

    bsp_init(prepare_spmd, 0, NULL);

    if (rank == 0)
    {
        prepare_spmd();
    }
}

void prepare_spmd()
{
    LOG_TRACE("Calling bsp_begin with %d procs", bsp_begin_nprocs);

    bsp_begin(bsp_begin_nprocs);

    LOG_TRACE("%s", "Proceeding to active object handler");

    bsp_obj->proceed();

    LOG_TRACE("%s", "Calling bsp_end");

    bsp_end();

    LOG_TRACE("%s", "Exiting bsp_end");

}

void BSPlib::proceed()
{
    next_spmd(this);
}

int BSPlib::nprocs ()
{
    return bsp_nprocs();
}

int BSPlib::pid ()
{
    return bsp_pid();
}

double BSPlib::time ()
{
    return bsp_time();
}

void BSPlib::sync()
{
    bsp_sync();
}

void BSPlib::send (int pid, const void *tag, const void *payload, size_t payload_nbytes)
{
    bsp_send(pid, tag, payload, payload_nbytes);
}

void BSPlib::qsize (int * nmessages, size_t * accum_nbytes)
{
    bsp_size_t nbytes;
    bsp_qsize(nmessages, &nbytes);
    *accum_nbytes = nbytes;
}

void BSPlib::get_tag (int * status , void * tag)
{
    bsp_get_tag(status, tag);
}

void BSPlib::move (void *payload, size_t reception_nbytes)
{
    bsp_move(payload, reception_nbytes);
}

void BSPlib::set_tagsize (size_t *tag_nbytes)
{
    bsp_size_t nbytes = *tag_nbytes;
    bsp_set_tagsize(&nbytes);
}

// BSP DRMA
void BSPlib::push_reg (const void *ident, size_t size)
{
    bsp_push_reg(ident, size);
}

void BSPlib::pop_reg (const void *ident)
{
    bsp_pop_reg(ident);
}

void BSPlib::put (int pid, const void *src, void *dst, long int offset, size_t nbytes)
{
    bsp_put(pid, src, dst, offset, nbytes);
}

void BSPlib::get (int pid, const void *src, long int offset, void *dst, size_t nbytes)
{
    bsp_get(pid, src, offset, dst, nbytes);
}

} // namespace activeBSP
