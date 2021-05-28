#include "management/ThreadedMPIActorServerFactory.h"

#include "mpi/MPIActorCommunicator.h"
#include "mpi/MPIIntraActorCommunicatorFactory.h"
#include "worker/ThreadedActorWorkerFactory.h"

namespace activebsp
{

std::shared_ptr<MPIWrapper> ThreadedMPIActorServerFactory::createMPIWrapper()
{
    return std::make_shared<MPIWrapper>();
}

std::shared_ptr<ActorCommunicator> ThreadedMPIActorServerFactory::createActorCommunicator (const std::shared_ptr <MPIWrapper> & mpi)
{
    return std::make_shared<MPIActorCommunicator>(mpi);
}

std::shared_ptr<ActorWorkerFactory> ThreadedMPIActorServerFactory::createActorWorkerFactory (const std::shared_ptr <MPIWrapper> & mpi,
                                                                                             const std::shared_ptr<ActorCommunicator> actorComm,
                                                                                             const std::shared_ptr<IntraActorCommunicatorFactory> & intraActorCommFact,
                                                                                             ActorRegistry * registry)
{
    return std::make_shared<ThreadedActorWorkerFactory>(mpi, actorComm, intraActorCommFact, registry);
}

std::shared_ptr<IntraActorCommunicatorFactory> ThreadedMPIActorServerFactory::createIntraActorCommunicatorFactory(const std::shared_ptr<MPIWrapper> &mpi)
{
    return std::make_shared<MPIIntraActorCommunicatorFactory>(mpi);
}

std::shared_ptr<ActorServer> ThreadedMPIActorServerFactory::createActorServer(const std::shared_ptr<ActorCommunicator> & comm,
                                                                              const std::shared_ptr<ActorWorkerFactory> & ActorWorkerFactory)
{
    return std::make_shared<ActorServer>(comm, ActorWorkerFactory);
}

} // namespace activebsp
