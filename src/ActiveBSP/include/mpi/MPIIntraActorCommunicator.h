#ifndef __MPI_INTRA_ACTOR_COMMUNICATOR_H__
#define __MPI_INTRA_ACTOR_COMMUNICATOR_H__

#include <memory>

#include <mpi.h>

#include "IntraActorCommunicator.h"
#include "MPIWrapper.h"

namespace activebsp
{

class MPIIntraActorCommunicator : public IntraActorCommunicator
{
private:
    std::shared_ptr<MPIWrapper> _mpi;
    MPI_Comm _comm;

public:
    MPIIntraActorCommunicator(const std::shared_ptr<MPIWrapper> & mpi, MPI_Comm comm);

    virtual int pid();
    virtual int nprocs();

    virtual ~MPIIntraActorCommunicator();

    virtual int receiveSPMDFunctionId();
    virtual void sendSPMDFunctionID(int functionId);

    virtual void gatherKeys(int * recvbuf, int key);
    virtual void broadcastVd(vector_distribution_base & vd);

};

} // namespace activebsp

#endif // __MPI_INTRA_ACTOR_COMMUNICATOR_H__
