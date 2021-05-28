#include "mpi/MPIIntraActorCommunicator.h"

#include "mpi.h"

namespace activebsp
{

MPIIntraActorCommunicator::MPIIntraActorCommunicator(const std::shared_ptr<MPIWrapper> &mpi, MPI_Comm comm)
    : _mpi(mpi), _comm(comm) {}


MPIIntraActorCommunicator::~MPIIntraActorCommunicator() {}

int MPIIntraActorCommunicator::pid()
{
    return _mpi->getRank(_comm);
}

int MPIIntraActorCommunicator::nprocs()
{
    return _mpi->getSize(_comm);

}

int MPIIntraActorCommunicator::receiveSPMDFunctionId()
{
    int function_id;
    MPI_Bcast(&function_id, 1, MPI_INT, 0, _comm);

    return function_id;
}

void MPIIntraActorCommunicator::sendSPMDFunctionID(int functionId)
{
    MPI_Bcast(&functionId, 1, MPI_INT, 0, _comm);
}

void MPIIntraActorCommunicator::gatherKeys(int * recvbuf, int key)
{
    MPI_Gather(&key, 1, MPI_INT, recvbuf, 1, MPI_INT, 0, _comm);
}

void MPIIntraActorCommunicator::broadcastVd(vector_distribution_base & vd)
{
    int s,nparts;

    s = pid();

    if (s == 0)
    {
        nparts = vd.nparts();
    }

    MPI_Bcast(&nparts, 1, MPI_INT, 0, _comm);

    if (s != 0)
    {
        vd.resize(nparts);
    }

    MPI_Bcast(vd.getBuf(), vd.getBufSize(), MPI_CHAR, 0, _comm);
}


} // namespace activebsp
