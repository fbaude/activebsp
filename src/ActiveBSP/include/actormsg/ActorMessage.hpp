#ifndef __ACTOR_MESSAGE_HPP__
#define __ACTOR_MESSAGE_HPP__

#include "actormsg/ActorMessage.h"

namespace activebsp
{

template <class T>
T * ActorMessage::getContentEnvelope() const
{
    instruction_envelope_t * instruction_envelope = getInstructionEnvelope();
    T * env = (T *) &instruction_envelope->data;

    return env;
}

} // namespace activebsp

#endif // __ACTOR_MESSAGE_HPP__
