#ifndef __THREADED_ACTOR_WORKER_FACTORY_H__
#define __THREADED_ACTOR_WORKER_FACTORY_H__

#include <vector>
#include <memory>

#include "ActorWorkerFactory.h"
#include "IntraActorCommunicatorFactory.h"
#include "ActorWorkerProxy.h"
#include "ActorRegistry.h"
#include "ActiveBSPlib.h"
#include "management/MasterProxy.h"
#include "future/ResultPublisher.h"
#include "ActorCommunicator.h"
#include "shmem/ActorSharedMemory.h"
#include "mpi/MPIWrapper.h"

namespace activebsp
{

class ThreadedActorWorkerFactory : public ActorWorkerFactory
{
private:
    ActorRegistry * _registry;
    std::shared_ptr<MPIWrapper> _mpi;
    std::shared_ptr<ActorCommunicator> _actorComm;
    std::shared_ptr<IntraActorCommunicatorFactory> _intraActorCommFact;

    std::shared_ptr<ActorSharedMemory> _actorShm;

public:
    ThreadedActorWorkerFactory(const std::shared_ptr<MPIWrapper> &,
                               const std::shared_ptr<ActorCommunicator> &,
                               const std::shared_ptr<IntraActorCommunicatorFactory>  &,
                               ActorRegistry *);

    virtual ~ThreadedActorWorkerFactory();

    virtual std::shared_ptr <ActorWorkerProxy> createActorProcessProxy(ActorServer * actorServer);

    virtual std::shared_ptr<ResultPublisher> createResultPublisher();
    //virtual std::shared_ptr<WorkerMasterProxy> createWorkerMasterProxy(const std::string & name, const std::vector<int> & pids);
    virtual std::shared_ptr<BSPlib> createBSPlib(const std::string & name, const std::vector<int> & pids);
};

} // namespace activebsp

#endif // __THREADED_ACTOR_WORKER_FACTORY_H__
