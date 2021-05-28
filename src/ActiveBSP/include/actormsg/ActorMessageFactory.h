#ifndef __ACTOR_MESSAGE_FACTORY_H__
#define __ACTOR_MESSAGE_FACTORY_H__

#include <memory>

#include "actormsg/ActorMessage.h"

namespace activebsp
{

class ActorMessageFactory
{
private:
    static ActorMessageFactory * _instance;

public:
    static ActorMessageFactory * getInstance();

    std::shared_ptr <ActorMessage> createMessage(int src, char * buf, int bufsize);
};

}

#endif // __ACTOR_MESSAGE_FACTORY_H__
