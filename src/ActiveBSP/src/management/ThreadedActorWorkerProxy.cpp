#include "worker/ThreadedActorWorkerProxy.h"

#include <pthread.h>

#include "Actor.h"
#include "worker/ThreadedActorWorkerFactory.h"

#include "log.h"

namespace activebsp
{

ThreadedActorWorkerProxy::ThreadedActorWorkerProxy(const std::shared_ptr <SharedMemoryRequestQueue> & queue,
                                                   const std::shared_ptr <ActorSharedMemory> & actorShm)
    : _queue(queue), _actorShm(actorShm) {}

ThreadedActorWorkerProxy::~ThreadedActorWorkerProxy() {}


void * create_actor_thread_from_proxy (void * args)
{
    std::shared_ptr<ActorSharedMemory> shm = *(std::shared_ptr<ActorSharedMemory>*) args;

    ThreadedActorWorkerFactory * fact = shm->getFactory();

    LOG_TRACE("%s", "Creating BSPlib object");

    std::shared_ptr<BSPlib> bsplib = fact->createBSPlib(shm->getName(), shm->getPids());

    LOG_TRACE("%s", "Entering bsplib object init (executing worker handler function)");

    bsplib->init();

    return NULL;
}


int ThreadedActorWorkerProxy::startActor(const std::string & name, const std::vector<int> & pids)
{
    LOG_TRACE("Starting actor \"%s\" worker thread", name.c_str());

    _actorShm->setActorInitParams(_queue, name, pids);

    pthread_create(&_thread, NULL, create_actor_thread_from_proxy, &_actorShm);

    return 0;
}

void ThreadedActorWorkerProxy::callActor(const ActiveObjectRequest & req)
{
    LOG_TRACE("%s", "Posting request into worker queue");

    _queue->postMessage(req);
}

void ThreadedActorWorkerProxy::stopActor()
{
    _queue->postMessage(ActiveObjectRequest(std::make_shared<CallActorMessage>(-1, nullptr, 0), -1));

    pthread_join(_thread, NULL);
}

} // namespace activebsp
