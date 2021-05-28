#ifndef __THREADED_ACTOR_PROCESS_PROXY_H__
#define __THREADED_ACTOR_PROCESS_PROXY_H__

#include <memory>
#include <vector>

#include <pthread.h>

#include "ActorWorkerProxy.h"
#include "ActorRegistry.h"
#include "ActorCommunicator.h"
#include "mpi/MPIWrapper.h"
#include "Actor.h"
#include "ActiveBSPlib.h"
#include "shmem/ActorSharedMemory.h"
#include "shmem/SharedMemoryRequestQueue.h"

namespace activebsp
{

class ThreadedActorWorkerProxy : public ActorWorkerProxy
{
private:
std::shared_ptr <SharedMemoryRequestQueue> _queue;
std::shared_ptr <ActorSharedMemory> _actorShm;

pthread_t _thread;
ActorBase * _actorBase = NULL;

public:
    ThreadedActorWorkerProxy(const std::shared_ptr <SharedMemoryRequestQueue> & queue,
                             const std::shared_ptr <ActorSharedMemory> & actorShm);

    virtual ~ThreadedActorWorkerProxy();

    virtual int startActor(const std::string &name, const std::vector<int> &pids);
    virtual void callActor(const ActiveObjectRequest &req);
    virtual void stopActor();

    std::shared_ptr <SharedMemoryRequestQueue> getQueue();
};

} // namespace activebsp

#endif // __THREADED_ACTOR_PROCESS_PROXY_H__
