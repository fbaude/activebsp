#ifndef __ACTOR_WORKER_PROXY_H__
#define __ACTOR_WORKER_PROXY_H__

#include <string>

#include "actormsg/ActiveObjectRequest.h"

namespace activebsp
{

class ActorWorkerProxy
{
public:
    virtual ~ActorWorkerProxy() {}

    virtual int startActor(const std::string &name, const std::vector<int> &pids) = 0;
    virtual void callActor(const ActiveObjectRequest &req) = 0;
    virtual void stopActor() = 0;
};

} // namespace activebsp

#endif // __ACTOR_WORKER_PROXY_H__
