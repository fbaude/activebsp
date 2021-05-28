#include "actormsg/StopActorProcessMessage.h"

namespace activebsp
{

StopActorProcessMessage::StopActorProcessMessage(int src, char * buf, int bufsize)
    : ActorMessage(src, buf, bufsize) {}

StopActorProcessMessage::StopActorProcessMessage()
    : ActorMessage()
{
          allocateOwnBuffer(sizeof(instruction_envelope_t));

          instruction_envelope_t * instruction_envelope = (instruction_envelope_t *) _buf;

          instruction_envelope->instruction = INSTRUCTION_STOP;
}


StopActorProcessMessage::~StopActorProcessMessage() {}

} // namepace activebsp
