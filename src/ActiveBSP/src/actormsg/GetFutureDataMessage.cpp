#include "actormsg/GetFutureDataMessage.h"

namespace activebsp
{

GetFutureDataMessage::GetFutureDataMessage(int src, char * buf, int buf_size)
    : ActorMessage(src, buf, buf_size) {}

GetFutureDataMessage::GetFutureDataMessage(int future_key)
{
    allocateOwnBuffer(sizeof(instruction_envelope_t) + sizeof(get_future_data_envelope_t));

    instruction_envelope_t * instruction_envelope = (instruction_envelope_t *) _buf;
    get_future_data_envelope_t * data_envelope = (get_future_data_envelope_t *) &instruction_envelope->data;

    instruction_envelope->instruction = INSTRUCTION_GET_FUTURE_DATA;

    data_envelope->future_key = future_key;
}

GetFutureDataMessage::~GetFutureDataMessage() {}

int GetFutureDataMessage::getFutureKey()
{
    get_future_data_envelope_t * data_envelope = (get_future_data_envelope_t *) getEnvelopeContent();

    return data_envelope->future_key;
}

} // namespace activebsp
