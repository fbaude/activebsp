#include "mpi/MPIIntraActorCommunicatorFactory.h"

#include "mpi/MPIIntraActorCommunicator.h"

namespace activebsp
{

MPIIntraActorCommunicatorFactory::MPIIntraActorCommunicatorFactory(const std::shared_ptr<MPIWrapper> & mpi)
    : _mpi(mpi) {}

MPIIntraActorCommunicatorFactory::~MPIIntraActorCommunicatorFactory() {}

std::shared_ptr<IntraActorCommunicator> MPIIntraActorCommunicatorFactory::createIntraActorCommunicator(const std::vector<int> & pids)
{
    return std::make_shared<MPIIntraActorCommunicator>(_mpi, _mpi->createSubset(_mpi->getWorld(), pids));
}

} // namespace activebsp
