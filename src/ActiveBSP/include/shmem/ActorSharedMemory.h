#ifndef __ACTOR_SHARED_MEMORY_H__
#define __ACTOR_SHARED_MEMORY_H__

#include <memory>

#include "SharedMemoryRequestQueue.h"
#include "ActiveBSPlib.h"

namespace activebsp
{

class ActorServer;
class ThreadedActorWorkerFactory;

class ActorSharedMemory
{
private:
    std::shared_ptr<SharedMemoryRequestQueue> _queue;
    std::vector<int> _pids;
    ThreadedActorWorkerFactory * _factory;
    std::string _name;
    ActorServer * _actorServer;

public:
    void setActorInitParams(const std::shared_ptr<SharedMemoryRequestQueue> & queue,
                            const std::string & name,
                            const std::vector<int> & pids);

    std::shared_ptr<SharedMemoryRequestQueue> getQueue() const;
    std::vector<int> & getPids();
    std::string & getName();

    void setFactory(ThreadedActorWorkerFactory * factory);
    void setActorServer(ActorServer * actorServer);

    ThreadedActorWorkerFactory * getFactory();
    ActorServer * getActorServer();

};

} // namespace activebsp

#endif // __ACTOR_SHARED_MEMORY_H__
