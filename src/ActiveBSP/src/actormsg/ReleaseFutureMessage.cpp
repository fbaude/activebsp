#include "actormsg/ReleaseFutureMessage.h"

namespace activebsp
{

ReleaseFutureMessage::ReleaseFutureMessage(int src, char * buf, int buf_size)
    : ActorMessage(src, buf, buf_size) {}

ReleaseFutureMessage::ReleaseFutureMessage(int future_key)
{
    allocateOwnBuffer(sizeof(instruction_envelope_t) + sizeof(get_future_data_envelope_t));

    instruction_envelope_t * instruction_envelope = (instruction_envelope_t *) _buf;
    release_future_envelope_t * data_envelope = (release_future_envelope_t *) &instruction_envelope->data;

    instruction_envelope->instruction = INSTRUCTION_RELEASE_FUTURE;

    data_envelope->future_key = future_key;
}

ReleaseFutureMessage::~ReleaseFutureMessage() {}

int ReleaseFutureMessage::getFutureKey()
{
    release_future_envelope_t * data_envelope = (release_future_envelope_t *) getEnvelopeContent();

    return data_envelope->future_key;
}

} // namespace activebsp
