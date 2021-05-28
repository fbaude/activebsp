#ifndef __THREADED_MPI_ACTOR_SERVER_FACTORY_H__
#define __THREADED_MPI_ACTOR_SERVER_FACTORY_H__

#include <memory>

#include "ActorServer.h"
#include "ActorCommunicator.h"
#include "mpi/MPIWrapper.h"
#include "IntraActorCommunicatorFactory.h"
#include "ActorRegistry.h"
#include "worker/ActorWorkerFactory.h"

namespace activebsp
{

class ThreadedMPIActorServerFactory
{
public:
    std::shared_ptr<MPIWrapper> createMPIWrapper();
    std::shared_ptr<ActorCommunicator> createActorCommunicator (const std::shared_ptr<MPIWrapper> &mpi);
    std::shared_ptr<ActorWorkerFactory> createActorWorkerFactory (const std::shared_ptr <MPIWrapper> & mpi,
                                                                  const std::shared_ptr<ActorCommunicator> actorComm,
                                                                  const std::shared_ptr<IntraActorCommunicatorFactory> & intraActorCommFact,
                                                                  ActorRegistry * registry);

    std::shared_ptr<IntraActorCommunicatorFactory> createIntraActorCommunicatorFactory(const std::shared_ptr<MPIWrapper> &mpi);

    std::shared_ptr<ActorServer> createActorServer(const std::shared_ptr<ActorCommunicator> & comm,
                                                   const std::shared_ptr<ActorWorkerFactory> & ActorWorkerFactory);

};

} // namespace activebsp

#endif // __THREADED_MPI_ACTOR_SERVER_FACTORY_H__
