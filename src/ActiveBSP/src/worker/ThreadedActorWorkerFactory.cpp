#include "worker/ThreadedActorWorkerFactory.h"

#include "IntraActorCommunicatorFactory.h"
#include "future/LazyResultPublisher.h"
#include "worker/ThreadedActorWorkerProxy.h"
#include "shmem/SharedMemoryMasterProxy.h"
#include "shmem/SharedMemoryRequestQueue.h"
#include "management/ActorServer.h"

#include <functional>

namespace activebsp
{

ThreadedActorWorkerFactory::ThreadedActorWorkerFactory(const std::shared_ptr<MPIWrapper> & mpi,
                                                       const std::shared_ptr<ActorCommunicator> & actorComm,
                                                       const std::shared_ptr<IntraActorCommunicatorFactory> &intraActorCommFact,
                                                       ActorRegistry * registry)
    : _registry(registry), _mpi(mpi), _actorComm(actorComm), _intraActorCommFact(intraActorCommFact) {}


ThreadedActorWorkerFactory::~ThreadedActorWorkerFactory() {}

std::shared_ptr <ActorWorkerProxy> ThreadedActorWorkerFactory::createActorProcessProxy(ActorServer * actorServer)
{
    std::shared_ptr<SharedMemoryRequestQueue> queue = std::make_shared<SharedMemoryRequestQueue>();
    _actorShm = std::make_shared<ActorSharedMemory>();
    _actorShm->setFactory(this);
    _actorShm->setActorServer(actorServer);

    return std::make_shared<ThreadedActorWorkerProxy> (queue, _actorShm);
}

std::shared_ptr<BSPlib> ThreadedActorWorkerFactory::createBSPlib(const std::string & name, const std::vector<int> & pids)
{
    MPI_Comm subset = _mpi->createSubset(_mpi->getWorld(), pids);
    std::shared_ptr<IntraActorCommunicator> intraActorComm = _intraActorCommFact->createIntraActorCommunicator(pids);
    actor_handler_t handler = _registry->getActorRegistry()[name];

    std::shared_ptr<SharedMemoryRequestQueue> queue = _actorShm->getQueue();
    if (!queue) {
        std::cerr << "createWorkerMasterProxy didn't get the request queue" <<std::endl;
    }

    // Give the management thread its own intra actor comm so that messages from both do not interfere
    std::shared_ptr<IntraActorCommunicator> masterIntraActorComm = _intraActorCommFact->createIntraActorCommunicator(pids);
    _actorShm->getActorServer()->setIntraActorComm(masterIntraActorComm);

    std::shared_ptr<ResultPublisher> resultPublisher = createResultPublisher();
    std::shared_ptr<MasterProxy> masterProxy = std::make_shared<SharedMemoryMasterProxy>(_actorComm, queue, resultPublisher, _actorShm);
    _actorComm->setMasterProxy(masterProxy);
    std::shared_ptr<BSPlib> bsplib = std::make_shared<BSPlib>(subset, std::function<void(BSPlib *)> (std::bind(handler, intraActorComm, masterProxy, std::placeholders::_1)));

    return bsplib;
}


std::shared_ptr<ResultPublisher> ThreadedActorWorkerFactory::createResultPublisher()
{
    return std::make_shared<LazyResultPublisher>(_actorComm);
}


} // namespace activebsp
