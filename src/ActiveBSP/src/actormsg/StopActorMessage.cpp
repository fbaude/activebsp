#include "actormsg/StopActorMessage.h"

namespace activebsp
{

StopActorMessage::StopActorMessage(int src, char * buf, int buf_size)
    : ActorMessage(src, buf, buf_size) {}

StopActorMessage::StopActorMessage(bool askAnswer /* = true */)
    : ActorMessage()
{
    allocateOwnBuffer(sizeof(instruction_envelope_t) + sizeof (stop_actor_envelope_t));

    instruction_envelope_t * instruction_envelope = (instruction_envelope_t *) _buf;
    stop_actor_envelope_t * stop_actor_envelope = (stop_actor_envelope_t *) &instruction_envelope->data;

    instruction_envelope->instruction = INSTRUCTION_DESTROY_ACTOR;
    stop_actor_envelope->ask_answer = askAnswer;
}

StopActorMessage::~StopActorMessage() {}

bool StopActorMessage::isAskingAnswer() const
{
    instruction_envelope_t * instruction_envelope = (instruction_envelope_t *) _buf;
    stop_actor_envelope_t * stop_actor_envelope = (stop_actor_envelope_t *) &instruction_envelope->data;

    return stop_actor_envelope->ask_answer;
}

} // namespace activebsp
