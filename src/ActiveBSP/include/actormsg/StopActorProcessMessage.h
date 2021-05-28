#ifndef __STOP_ACTOR_PROCESS_MESSAGE_H__
#define __STOP_ACTOR_PROCESS_MESSAGE_H__

#include "actormsg/ActorMessage.h"

namespace activebsp
{

class StopActorProcessMessage : public ActorMessage
{
public:
    StopActorProcessMessage(int src, char *buf, int bufsize);
    StopActorProcessMessage();

    virtual ~StopActorProcessMessage();

};

} // namespace activebsp

#endif // __STOP_ACTOR_PROCESS_MESSAGE_H__
