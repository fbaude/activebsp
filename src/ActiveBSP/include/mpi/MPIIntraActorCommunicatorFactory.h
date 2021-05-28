#ifndef __MPI_INTRA_ACTOR_COMMUNICATOR_FACTORY_H__
#define __MPI_INTRA_ACTOR_COMMUNICATOR_FACTORY_H__

#include <memory>

#include "IntraActorCommunicatorFactory.h"
#include "MPIIntraActorCommunicator.h"
#include "MPIWrapper.h"

namespace activebsp
{

class MPIIntraActorCommunicatorFactory : public IntraActorCommunicatorFactory
{
private:
    std::shared_ptr<MPIWrapper> _mpi;

public:
    MPIIntraActorCommunicatorFactory(const std::shared_ptr<MPIWrapper> & mpi);

    virtual ~MPIIntraActorCommunicatorFactory();

    virtual std::shared_ptr<IntraActorCommunicator> createIntraActorCommunicator(const std::vector<int> & pids);
};

} // namespace activebsp


#endif // __MPI_INTRA_ACTOR_COMMUNICATOR_FACTORY_H__
