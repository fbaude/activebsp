#include "shmem/ActorSharedMemory.h"

namespace activebsp
{

void ActorSharedMemory::setActorInitParams(const std::shared_ptr<SharedMemoryRequestQueue> & queue,
                                           const std::string &name,
                                           const std::vector<int> &pids)
{
    _queue = queue;
    _name = name;
    _pids = pids;
}

std::shared_ptr<SharedMemoryRequestQueue> ActorSharedMemory::getQueue() const
{
    return _queue;
}

std::vector<int> & ActorSharedMemory::getPids()
{
    return _pids;
}

ThreadedActorWorkerFactory * ActorSharedMemory::getFactory()
{
    return _factory;
}

std::string & ActorSharedMemory::getName()
{
    return _name;
}

void ActorSharedMemory::setFactory(ThreadedActorWorkerFactory * factory)
{
    _factory = factory;
}

void ActorSharedMemory::setActorServer(ActorServer * actorServer)
{
    _actorServer = actorServer;
}

ActorServer * ActorSharedMemory::getActorServer()
{
    return _actorServer;
}




} // namespace activebsp
