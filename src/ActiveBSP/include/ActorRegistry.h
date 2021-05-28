#ifndef __ACTOR_REGISTRY_H__
#define __ACTOR_REGISTRY_H__

#include <map>
#include <memory>

#include <mpi.h>

#include "ActiveBSPlib.h"
#include "IntraActorCommunicator.h"
#include "management/MasterProxy.h"

namespace activebsp
{

typedef void (*actor_handler_t)(const std::shared_ptr<IntraActorCommunicator> &,
                                const std::shared_ptr<MasterProxy> &,
                                BSPlib *);
typedef std::map <std::string, actor_handler_t> actor_registry_t;

class ActorRegistry
{
private:
    static ActorRegistry * _instance;

public:
    actor_registry_t & getActorRegistry();

    static ActorRegistry * getInstance();

};


class ActorMaker
{

public:

protected:
    ActorMaker(const std::string name, const actor_handler_t & handler);
};

}

#endif // __ACTOR_REGISTRY_H__
