#ifndef __STOP_ACTOR_MESSAGE_H__
#define __STOP_ACTOR_MESSAGE_H__

#include "actormsg/ActorMessage.h"

namespace activebsp
{

class StopActorMessage : public activebsp::ActorMessage
{
public:
    StopActorMessage(bool askAnswer = true);
    StopActorMessage(int src, char *, int size);

    virtual ~StopActorMessage();

    bool isAskingAnswer() const;
};

} // namespace activebsp

#endif // __STOP_ACTOR_MESSAGE_H__
