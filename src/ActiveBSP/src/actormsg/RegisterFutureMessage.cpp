#include "actormsg/RegisterFutureMessage.h"

#include <string.h>

namespace activebsp
{

RegisterFutureMessage::RegisterFutureMessage(int src, char * buf, int buf_size)
    : ActorMessage(src, buf, buf_size) {}

RegisterFutureMessage::RegisterFutureMessage(int future_key, std::vector<char> future_data)
{
    int future_data_size = future_data.size();

    allocateOwnBuffer(sizeof(instruction_envelope_t) + sizeof(register_future_data_envelope_t) + future_data_size);

    instruction_envelope_t * instruction_envelope = (instruction_envelope_t *) _buf;
    register_future_data_envelope_t * data_envelope = (register_future_data_envelope_t *) &instruction_envelope->data;

    instruction_envelope->instruction = INSTRUCTION_REGISTER_FUTURE_DATA;

    data_envelope->future_key = future_key;
    data_envelope->data_size  = future_data_size;

    data_envelope->data_offset = 0;

    memcpy(ENVELOPE_OFFSET(data_envelope, data_offset), &future_data[0], future_data_size);
}

RegisterFutureMessage::~RegisterFutureMessage() {}

int RegisterFutureMessage::getFutureKey()
{
    register_future_data_envelope_t * data_envelope = (register_future_data_envelope_t *) getEnvelopeContent();

    return data_envelope->future_key;
}

std::vector<char> RegisterFutureMessage::getFutureData()
{
    register_future_data_envelope_t * data_envelope = (register_future_data_envelope_t *) getEnvelopeContent();

    std::vector<char> ret;
    ret.resize(data_envelope->data_size);
    memcpy(&ret[0], ENVELOPE_OFFSET(data_envelope, data_offset), data_envelope->data_size);

    return ret;
}

} // namespace activebsp
