#ifndef __ACTOR_WORKER_FACTORY_H__
#define __ACTOR_WORKER_FACTORY_H__

#include <string>
#include <memory>
#include <vector>

#include "ActorWorkerProxy.h"

namespace activebsp
{

class ActorServer;

class ActorWorkerFactory
{
public:
    virtual ~ActorWorkerFactory() {}

    virtual std::shared_ptr <ActorWorkerProxy> createActorProcessProxy(ActorServer *) = 0;

};

} // namespace activebsp

#endif // __ACTOR_WORKER_FACTORY_H__
