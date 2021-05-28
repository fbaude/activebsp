#ifndef __ACTOR_WORKER_H__
#define __ACTOR_WORKER_H__

#include <functional>
#include <map>
#include <string>
#include <vector>
#include <memory>

#include "IntraActorCommunicator.h"
#include "ActiveBSPlib.h"
#include "management/MasterProxy.h"
#include "member_traits.h"
#include "serialization/serialization.h"
#include "Actor.h"

namespace activebsp
{

class ActorWorker
{
private:
    ActorBase * _obj;

    int _run;

    typedef std::map <std::string, std::function<std::vector<char>(char *, int, ActorBase *)> > function_map_t;

    function_map_t _functions;

    std::shared_ptr<ActorCommunicator> _actorComm;
    std::shared_ptr<IntraActorCommunicator> _intraActorComm;
    std::shared_ptr<MasterProxy> _masterProxy;

public:
    ActorWorker(ActorBase * obj,
                const std::shared_ptr<ActorCommunicator> &actorComm,
                const std::shared_ptr<IntraActorCommunicator> & intraActorComm,
                const std::shared_ptr<MasterProxy> & MasterProxy);
    virtual ~ActorWorker();

    void register_function(const std::string & name, std::function< std::vector<char> (char *, int, ActorBase *)> f);

    void handler();

    void master_loop();
    void slave_loop();

    bool is_master();

    void destroyActor();
};

} // namespace activebsp

#endif // __ACTOR_WORKER_H__
