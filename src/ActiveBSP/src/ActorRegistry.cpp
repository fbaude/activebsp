#include "ActorRegistry.h"

namespace activebsp
{

ActorRegistry * ActorRegistry::_instance = NULL;

actor_registry_t & ActorRegistry::getActorRegistry()
{
    static actor_registry_t * actor_handlers = new actor_registry_t();
    return *actor_handlers;
}

ActorRegistry * ActorRegistry::getInstance()
{
    if (_instance == NULL)
    {
        _instance = new ActorRegistry();
    }

    return _instance;
}


ActorMaker::ActorMaker(const std::string name, const actor_handler_t & handler)
{
    ActorRegistry::getInstance()->getActorRegistry()[name] = handler;
}

} // namespace activebsp
